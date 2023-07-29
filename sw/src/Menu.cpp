#include "Menu.h"

sw::Menu::Menu()
{
    this->_hMenu = CreateMenu();
}

HMENU sw::Menu::GetMenuHandle()
{
    return this->_hMenu;
}

void sw::Menu::Update()
{
    this->Clear();
    this->_itemMap.clear();

    for (MenuItem &item : this->items) {
        HMENU hSubMenu = CreatePopupMenu();

        int count = item.GetSubItemCount();
        AppendMenuW(this->_hMenu, MF_POPUP, reinterpret_cast<UINT_PTR>(hSubMenu), item.GetText().c_str());

        for (int i = 0; i < count; ++i) {
            this->AppendSubItems(hSubMenu, item.GetSubItemAt(i));
        }
    }
}

void sw::Menu::Clear()
{
    while (GetMenuItemCount(this->_hMenu) > 0) {
        RemoveMenu(this->_hMenu, 0, MF_BYPOSITION);
    }
}

void sw::Menu::AppendSubItems(HMENU hMenu, MenuItem &item)
{
    int count = item.GetSubItemCount();

    if (item.IsSparator()) {
        AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);
        return;
    }

    if (count == 0) {
        AppendMenuW(hMenu, MF_STRING, this->_itemMap.size(), item.GetText().c_str());
        this->_itemMap.push_back(&item);
        return;
    }

    HMENU hSubMenu = CreatePopupMenu();
    AppendMenuW(hMenu, MF_POPUP, reinterpret_cast<UINT_PTR>(hSubMenu), item.GetText().c_str());

    for (int i = 0; i < count; ++i) {
        MenuItem &subItem = item.GetSubItemAt(i);
        this->AppendSubItems(hSubMenu, subItem);
    }
}
