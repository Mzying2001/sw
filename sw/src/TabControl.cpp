#include "TabControl.h"
#include "Utils.h"

sw::TabControl::TabControl()
    : ContentRect(
          // get
          [&]() -> const sw::Rect & {
              static sw::Rect result;
              result = this->GetContentRect(this->ClientRect);
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

void sw::TabControl::Measure(const Size &availableSize)
{
    Size measureSize = availableSize;
    this->UIElement::Measure(measureSize);

    int selectedIndex = this->SelectedIndex;
    if (selectedIndex < 0 || selectedIndex >= this->ChildCount) return;

    sw::Rect contentRect;
    contentRect = this->GetContentRect(contentRect);

    measureSize = {contentRect.width, contentRect.height};
    (*this)[selectedIndex].Measure(measureSize);
}

void sw::TabControl::Arrange(const sw::Rect &finalPosition)
{
    this->UIElement::Arrange(finalPosition);

    int selectedIndex = this->SelectedIndex;
    if (selectedIndex < 0 || selectedIndex >= this->ChildCount) return;

    (*this)[selectedIndex].Arrange(this->ContentRect);
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

sw::Rect sw::TabControl::GetContentRect(const sw::Rect &rect)
{
    RECT rt = rect;
    this->SendMessageW(TCM_ADJUSTRECT, FALSE, reinterpret_cast<LPARAM>(&rt));
    return rt;
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

void sw::TabControl::OnNotified(NMHDR *pNMHDR)
{
    if (pNMHDR->code == TCN_SELCHANGE) {
        this->UpdateChildVisible();
    }
}
