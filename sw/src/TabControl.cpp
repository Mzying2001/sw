#include "TabControl.h"
#include "Utils.h"
#include <cmath>
#include <limits>

sw::TabControl::TabControl()
    : ContentRect(
          Property<sw::Rect>::Init(this)
              .Getter([](TabControl *self) -> sw::Rect {
                  RECT rect;
                  GetClientRect(self->Handle, &rect);
                  self->_CalcContentRect(rect);
                  return rect;
              })),

      SelectedIndex(
          Property<int>::Init(this)
              .Getter([](TabControl *self) -> int {
                  return (int)self->SendMessageW(TCM_GETCURSEL, 0, 0);
              })
              .Setter([](TabControl *self, int value) {
                  if (self->SelectedIndex != value) {
                      self->SendMessageW(TCM_SETCURSEL, (WPARAM)value, 0);
                      self->OnSelectedIndexChanged();
                  }
              })),

      Alignment(
          Property<TabAlignment>::Init(this)
              .Getter([](TabControl *self) -> TabAlignment {
                  auto style = self->GetStyle();
                  if (style & TCS_VERTICAL) {
                      return (style & TCS_RIGHT) ? TabAlignment::Right : TabAlignment::Left;
                  } else {
                      return (style & TCS_BOTTOM) ? TabAlignment::Bottom : TabAlignment::Top;
                  }
              })
              .Setter([](TabControl *self, TabAlignment value) {
                  self->_SetTabAlignment(value);
              })),

      MultiLine(
          Property<bool>::Init(this)
              .Getter([](TabControl *self) -> bool {
                  return self->GetStyle(TCS_MULTILINE);
              })
              .Setter([](TabControl *self, bool value) {
                  self->SetStyle(TCS_MULTILINE, value);
                  self->InvalidateMeasure();
              })),

      AutoSize(
          Property<bool>::Init(this)
              .Getter([](TabControl *self) -> bool {
                  return self->_autoSize;
              })
              .Setter([](TabControl *self, bool value) {
                  if (self->_autoSize != value) {
                      self->_autoSize = value;
                      self->InvalidateMeasure();
                  }
              }))
{
    this->InitControl(WC_TABCONTROLW, L"", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | TCS_TABS, 0);

    this->Rect    = sw::Rect{0, 0, 200, 200};
    this->TabStop = true;
    this->LayoutUpdateCondition |= sw::LayoutUpdateCondition::FontChanged;
}

int sw::TabControl::GetTabCount()
{
    return (int)this->SendMessageW(TCM_GETITEMCOUNT, 0, 0);
}

void sw::TabControl::UpdateTab()
{
    TCITEMW item{};
    item.mask    = TCIF_TEXT;
    item.pszText = (LPWSTR)L"";

    int childCount = this->ChildCount;
    int tabCount   = this->GetTabCount();

    while (tabCount < childCount) {
        this->_InsertItem(tabCount, item);
        tabCount = this->GetTabCount();
    }

    while (tabCount > childCount) {
        this->_DeleteItem(tabCount - 1);
        tabCount = this->GetTabCount();
    }

    for (int i = 0; i < childCount; ++i) {
        auto text    = this->GetChildAt(i).Text.Get();
        item.pszText = (LPWSTR)text.c_str();
        this->_SetItem(i, item);
    }

    this->Redraw();
    this->InvalidateMeasure();
}

void sw::TabControl::UpdateTabText(int index)
{
    if (index < 0) {
        return;
    }

    int childCount = this->ChildCount;
    int tabCount   = this->GetTabCount();

    if (index >= childCount || index >= tabCount) {
        return;
    }

    std::wstring text =
        this->GetChildAt(index).Text;

    TCITEMW item{};
    item.mask    = TCIF_TEXT;
    item.pszText = (LPWSTR)text.c_str();
    this->_SetItem(index, item);

    this->Redraw();
    this->InvalidateMeasure();
}

void sw::TabControl::OnAddedChild(UIElement &element)
{
    auto text = element.Text.Get();

    TCITEMW item{};
    item.mask    = TCIF_TEXT;
    item.pszText = (LPWSTR)text.c_str();

    int index = this->IndexOf(element);
    this->_InsertItem(index, item);
    ShowWindow(element.Handle, index == this->SelectedIndex ? SW_SHOW : SW_HIDE);

    this->UIElement::OnAddedChild(element);
}

void sw::TabControl::OnRemovedChild(UIElement &element)
{
    this->UpdateTab();
    this->_UpdateChildVisible(false);
    this->UIElement::OnRemovedChild(element);
}

sw::Size sw::TabControl::MeasureOverride(const Size &availableSize)
{
    UIElement *selectedItem = this->_GetSelectedItem();

    if (!this->_autoSize || selectedItem == nullptr) {
        return this->UIElement::MeasureOverride(availableSize);
    }

    bool isWidthInf  = std::isinf(availableSize.width);
    bool isHeightInf = std::isinf(availableSize.height);

    if (isWidthInf && isHeightInf) {
        selectedItem->Measure(availableSize);
    } else {
        const int inf = (std::numeric_limits<int>::max)();

        SIZE availableSizePx{
            isWidthInf ? inf : Dip::DipToPxX(availableSize.width),
            isHeightInf ? inf : Dip::DipToPxY(availableSize.height)};

        RECT rtContent{0, 0, availableSizePx.cx, availableSizePx.cy};
        this->_CalcContentRect(rtContent);

        SIZE sizeBorder{
            availableSizePx.cx - (rtContent.right - rtContent.left),
            availableSizePx.cy - (rtContent.bottom - rtContent.top)};

        Size measureSize = availableSize;
        measureSize.width -= Dip::PxToDipX(sizeBorder.cx);
        measureSize.height -= Dip::PxToDipY(sizeBorder.cy);

        selectedItem->Measure(measureSize);
    }

    SIZE desireSize = selectedItem->GetDesireSize();
    this->_CalcIdealSize(desireSize);
    return desireSize;
}

void sw::TabControl::ArrangeOverride(const Size &finalSize)
{
    UIElement *selectedItem = this->_GetSelectedItem();
    if (selectedItem == nullptr) return;

    if (this->_autoSize) {
        selectedItem->Arrange(this->ContentRect);
    } else {
        sw::Rect contentRect = this->ContentRect;
        selectedItem->Measure(contentRect.GetSize());
        selectedItem->Arrange(contentRect);
    }
}

bool sw::TabControl::OnNotified(NMHDR *pNMHDR, LRESULT &result)
{
    if (pNMHDR->code == TCN_SELCHANGE) {
        this->OnSelectedIndexChanged();
    }
    return this->Control::OnNotified(pNMHDR, result);
}

void sw::TabControl::OnSelectedIndexChanged()
{
    this->_UpdateChildVisible();
    this->RaiseRoutedEvent(TabControl_SelectedIndexChanged);
}

void sw::TabControl::_SetTabAlignment(TabAlignment value)
{
    auto oldStyle = this->GetStyle();
    auto style    = oldStyle;

    switch (value) {
        case TabAlignment::Top: {
            style &= ~(TCS_VERTICAL | TCS_BOTTOM);
            break;
        }
        case TabAlignment::Bottom: {
            style &= ~TCS_VERTICAL;
            style |= TCS_BOTTOM;
            break;
        }
        case TabAlignment::Left: {
            style |= (TCS_VERTICAL | TCS_MULTILINE);
            style &= ~TCS_RIGHT;
            break;
        }
        case TabAlignment::Right: {
            style |= (TCS_VERTICAL | TCS_MULTILINE | TCS_RIGHT);
            break;
        }
    }

    if (style == oldStyle) {
        return;
    } else {
        this->SetStyle(style);
    }

    // 特定情况下需要重新创建控件
    if ((style & TCS_VERTICAL) ||                             // TCS_VERTICAL位为1
        ((style & TCS_VERTICAL) ^ (oldStyle & TCS_VERTICAL))) // TCS_VERTICAL位改变
    {
        this->LayoutUpdateCondition |= sw::LayoutUpdateCondition::Supressed;

        int selectedIndex = this->SelectedIndex;
        int childCount    = this->ChildCount;

        std::vector<UIElement *> children;
        children.reserve(childCount);

        for (int i = childCount - 1; i >= 0; --i) {
            children.push_back(&this->GetChildAt(i));
            this->RemoveChildAt(i);
        }

        this->ResetHandle();

        while (!children.empty()) {
            this->AddChild(children.back());
            children.pop_back();
        }

        this->SelectedIndex = selectedIndex;
        this->LayoutUpdateCondition &= ~sw::LayoutUpdateCondition::Supressed;
    }

    this->InvalidateMeasure();
}

void sw::TabControl::_UpdateChildVisible(bool invalidMeasure)
{
    int selectedIndex = this->SelectedIndex;
    int childCount    = this->ChildCount;

    for (int i = 0; i < childCount; ++i) {
        auto &item = this->GetChildAt(i);
        ShowWindow(item.Handle, i == selectedIndex ? SW_SHOW : SW_HIDE);
    }

    if (invalidMeasure) {
        this->InvalidateMeasure();
    }
}

int sw::TabControl::_InsertItem(int index, TCITEMW &item)
{
    return (int)this->SendMessageW(TCM_INSERTITEMW, (WPARAM)index, reinterpret_cast<LPARAM>(&item));
}

bool sw::TabControl::_SetItem(int index, TCITEMW &item)
{
    return this->SendMessageW(TCM_SETITEMW, (WPARAM)index, reinterpret_cast<LPARAM>(&item));
}

bool sw::TabControl::_DeleteItem(int index)
{
    return this->SendMessageW(TCM_DELETEITEM, (WPARAM)index, 0);
}

bool sw::TabControl::_DeleteAllItems()
{
    return this->SendMessageW(TCM_DELETEALLITEMS, 0, 0);
}

void sw::TabControl::_CalcContentRect(RECT &rect)
{
    this->SendMessageW(TCM_ADJUSTRECT, FALSE, reinterpret_cast<LPARAM>(&rect));
}

void sw::TabControl::_CalcIdealSize(SIZE &size)
{
    RECT rect{0, 0, size.cx, size.cy};
    this->SendMessageW(TCM_ADJUSTRECT, TRUE, reinterpret_cast<LPARAM>(&rect));
    size = SIZE{rect.right - rect.left, rect.bottom - rect.top};
}

sw::UIElement *sw::TabControl::_GetSelectedItem()
{
    int selectedIndex = this->SelectedIndex;

    if (selectedIndex < 0 || selectedIndex >= this->ChildCount) {
        return nullptr;
    } else {
        return &this->GetChildAt(selectedIndex);
    }
}
