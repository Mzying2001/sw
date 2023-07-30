#include "Menu.h"

sw::Menu::Menu()
{
    this->_hMenu = CreateMenu();
}

sw::Menu::Menu(std::initializer_list<MenuItem> items)
    : Menu()
{
    for (const MenuItem &item : items) {
        std::shared_ptr<MenuItem> pItem = std::make_shared<MenuItem>(item);
        this->items.push_back(pItem);
    }
    this->Update();
}

sw::Menu::Menu(const Menu &menu)
    : Menu()
{
    this->items = menu.items;
    this->Update();
}

sw::Menu::~Menu()
{
    this->ClearAddedItems();
    DestroyMenu(this->_hMenu);
}

HMENU sw::Menu::GetHandle()
{
    return this->_hMenu;
}

void sw::Menu::Update()
{
    this->ClearAddedItems();

    for (std::shared_ptr<MenuItem> pItem : this->items) {
        if (!pItem->IsSeparator())
            this->AppendMenuItem(this->_hMenu, pItem);
    }
}

sw::MenuItem *sw::Menu::GetMenuItem(int id)
{
    return id >= 0 && id < this->_leaves.size() ? this->_leaves[id].get() : nullptr;
}

void sw::Menu::ClearAddedItems()
{
    while (GetMenuItemCount(this->_hMenu) > 0) {
        RemoveMenu(this->_hMenu, 0, MF_BYPOSITION);
    }

    for (HMENU hMenu : this->_popupMenus) {
        DestroyMenu(hMenu);
    }

    this->_popupMenus.clear();
    this->_leaves.clear();
}

void sw::Menu::AppendMenuItem(HMENU hMenu, std::shared_ptr<MenuItem> pItem)
{
    if (pItem->IsSeparator()) {
        AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);
        return;
    }

    if (pItem->subItems.size() == 0) {
        AppendMenuW(hMenu, MF_STRING, this->_leaves.size(), pItem->text.c_str());
        this->_leaves.push_back(pItem);
        return;
    }

    HMENU hSubMenu = CreatePopupMenu();
    this->_popupMenus.push_back(hSubMenu);
    AppendMenuW(hMenu, MF_POPUP, reinterpret_cast<UINT_PTR>(hSubMenu), pItem->text.c_str());

    for (std::shared_ptr<MenuItem> pSubItem : pItem->subItems) {
        this->AppendMenuItem(hSubMenu, pSubItem);
    }
}

void sw::Menu::RaiseMenuItemCommand(MenuItem &item)
{
    if (item.command)
        item.command(item);
}
