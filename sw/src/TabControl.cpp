#include "TabControl.h"
#include "Utils.h"

sw::TabControl::TabControl()
    : ContentRect(
          // get
          [&]() -> const sw::Rect & {
              static sw::Rect result;

              RECT rect;
              GetClientRect(this->Handle, &rect);
              this->SendMessageW(TCM_ADJUSTRECT, FALSE, reinterpret_cast<LPARAM>(&rect));

              result = rect;
              return result;
          }),

      SelectedIndex(
          // get
          [&]() -> const int & {
              static int result;
              result = (int)this->SendMessageW(TCM_GETCURSEL, 0, 0);
              return result;
          },
          // set
          [&](const int &value) {
              this->SendMessageW(TCM_SETCURSEL, (WPARAM)value, 0);
              this->UpdateChildVisible();
          }),

      Alignment(
          // get
          [&]() -> const TabAlignment & {
              static TabAlignment result;
              auto style = this->GetStyle();

              if (style & TCS_VERTICAL) {
                  result = (style & TCS_RIGHT) ? TabAlignment::Right : TabAlignment::Left;
                  return result;
              } else {
                  result = (style & TCS_BOTTOM) ? TabAlignment::Bottom : TabAlignment::Top;
                  return result;
              }
          },
          // set
          [&](const TabAlignment &value) {
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
              if ((style & TCS_VERTICAL) ||                               // TCS_VERTICAL位为1
                  ((style & TCS_VERTICAL) ^ (oldStyle & TCS_VERTICAL))) { // TCS_VERTICAL位改变

                  int selectedIndex = this->SelectedIndex;
                  int childCount    = this->ChildCount;

                  std::vector<UIElement *> children;
                  children.reserve(childCount);
                  for (int i = childCount - 1; i >= 0; --i) {
                      children.push_back(&(*this)[i]);
                      this->RemoveChildAt(i);
                  }

                  this->ResetHandle();
                  for (int i = childCount - 1; i >= 0; --i) {
                      this->AddChild(children[i]);
                  }

                  this->SelectedIndex = selectedIndex;

              } else {
                  this->NotifyLayoutUpdated();
              }
          }),

      MultiLine(
          // get
          [&]() -> const bool & {
              static bool result;
              result = this->GetStyle(TCS_MULTILINE);
              return result;
          },
          // set
          [&](const bool &value) {
              this->SetStyle(TCS_MULTILINE, value);
              this->NotifyLayoutUpdated();
          })
{
    this->InitControl(WC_TABCONTROLW, L"", WS_CHILD | WS_VISIBLE | TCS_FOCUSNEVER, 0);
    this->Rect = sw::Rect(0, 0, 200, 200);
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
        this->InsertItem(tabCount, item);
        tabCount = this->GetTabCount();
    }

    while (tabCount > childCount) {
        this->DeleteItem(tabCount - 1);
        tabCount = this->GetTabCount();
    }

    for (int i = 0; i < childCount; ++i) {
        item.pszText = (LPWSTR)(*this)[i].Text->c_str();
        this->SetItem(i, item);
    }
}

void sw::TabControl::UpdateTabText(int index)
{
    if (index < 0) {
        return;
    }

    int childCount = this->ChildCount;
    int tabCount   = this->GetTabCount();

    if (index < childCount && index < tabCount) {
        TCITEMW item{};
        item.mask    = TCIF_TEXT;
        item.pszText = (LPWSTR)(*this)[index].Text->c_str();
        this->SetItem(index, item);
    }
}

void sw::TabControl::Arrange(const sw::Rect &finalPosition)
{
    this->UIElement::Arrange(finalPosition);

    int selectedIndex = this->SelectedIndex;
    if (selectedIndex < 0 || selectedIndex >= this->ChildCount) return;

    UIElement &selectedItem = this->operator[](selectedIndex);
    sw::Rect contentRect    = this->ContentRect;

    selectedItem.Measure({contentRect.width, contentRect.height});
    selectedItem.Arrange(contentRect);
}

void sw::TabControl::OnAddedChild(UIElement &element)
{
    TCITEMW item{};
    item.mask    = TCIF_TEXT;
    item.pszText = (LPWSTR)element.Text->c_str();

    int index = this->IndexOf(element);

    this->InsertItem(index, item);
    element.Visible = index == this->SelectedIndex;
}

void sw::TabControl::OnRemovedChild(UIElement &element)
{
    this->UpdateTab();
    this->UpdateChildVisible();
}

void sw::TabControl::OnNotified(NMHDR *pNMHDR)
{
    if (pNMHDR->code == TCN_SELCHANGE) {
        this->OnSelectedIndexChanged();
    }
}

void sw::TabControl::OnSelectedIndexChanged()
{
    this->UpdateChildVisible();
    this->RaiseRoutedEvent(TabControl_SelectedIndexChanged);
}

void sw::TabControl::UpdateChildVisible()
{
    int selectedIndex = this->SelectedIndex;
    int childCount    = this->ChildCount;

    for (int i = 0; i < childCount; ++i) {
        UIElement &item = (*this)[i];
        item.Visible    = i == selectedIndex;
        item.Visible    = i == selectedIndex; // 不加这个在点击按钮后立刻换页按钮会莫名其妙固定在界面上
    }
}

int sw::TabControl::InsertItem(int index, TCITEMW &item)
{
    return (int)this->SendMessageW(TCM_INSERTITEMW, (WPARAM)index, reinterpret_cast<LPARAM>(&item));
}

bool sw::TabControl::SetItem(int index, TCITEMW &item)
{
    return this->SendMessageW(TCM_SETITEMW, (WPARAM)index, reinterpret_cast<LPARAM>(&item));
}

bool sw::TabControl::DeleteItem(int index)
{
    return this->SendMessageW(TCM_DELETEITEM, (WPARAM)index, 0);
}

bool sw::TabControl::DeleteAllItems()
{
    return this->SendMessageW(TCM_DELETEALLITEMS, 0, 0);
}