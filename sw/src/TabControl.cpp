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
              result = (int)TabCtrl_GetCurSel(this->Handle);
              return result;
          },
          // set
          [&](const int &value) {
              TabCtrl_SetCurSel(this->Handle, value);
              this->UpdateChildVisible();
          })
{
    this->InitControl(WC_TABCONTROLW, L"", WS_CHILD | WS_VISIBLE | TCS_FOCUSNEVER, 0);
    this->Rect = sw::Rect(0, 0, 200, 200);
}

int sw::TabControl::GetTabCount()
{
    return (int)TabCtrl_GetItemCount(this->Handle);
}

void sw::TabControl::UpdateTab()
{
    int childCount = this->ChildCount;
    int tabCount   = this->GetTabCount();

    HWND hwnd = this->Handle;

    TCITEMW tci{};
    tci.mask = TCIF_TEXT;

    if (tabCount == childCount) {
        for (int i = 0; i < childCount; ++i) {
            tci.pszText = (LPWSTR)(*this)[i].Text->c_str();
            TabCtrl_SetItem(hwnd, i, &tci);
        }
    } else {
        int selectedIndex = this->SelectedIndex;
        TabCtrl_DeleteAllItems(hwnd);
        for (int i = 0; i < childCount; ++i) {
            tci.pszText = (LPWSTR)(*this)[i].Text->c_str();
            TabCtrl_InsertItem(hwnd, i, &tci);
        }
        if (selectedIndex >= 0) {
            this->SelectedIndex = selectedIndex;
        }
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
        TCITEMW tci{};
        tci.mask    = TCIF_TEXT;
        tci.pszText = (LPWSTR)(*this)[index].Text->c_str();
        TabCtrl_SetItem(this->Handle, index, &tci);
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
    TCITEMW tci{};
    tci.mask    = TCIF_TEXT;
    tci.pszText = (LPWSTR)L"";

    TabCtrl_InsertItem(this->Handle, this->GetTabCount(), &tci);
    this->UpdateTab();
}

void sw::TabControl::OnRemovedChild(UIElement &element)
{
    TabCtrl_DeleteItem(this->Handle, this->GetTabCount() - 1);
    this->UpdateTab();
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

void sw::TabControl::OnNotified(NMHDR *pNMHDR)
{
    if (pNMHDR->code == TCN_SELCHANGE) {
        this->UpdateChildVisible();
    }
}
