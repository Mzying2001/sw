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

    int i = 0;
    for (std::shared_ptr<MenuItem> pItem : this->items) {
        if (!pItem->IsSeparator())
            this->AppendMenuItem(this->_hMenu, pItem, i++);
    }
}

sw::MenuItem *sw::Menu::GetMenuItem(int id)
{
    return id >= 0 && id < this->_leaves.size() ? this->_leaves[id].get() : nullptr;
}

sw::MenuItem *sw::Menu::GetMenuItem(std::initializer_list<int> path)
{
    MenuItem *result = nullptr;

    std::initializer_list<int>::iterator it  = path.begin();
    std::initializer_list<int>::iterator end = path.end();

    if (it == end) {
        return nullptr;
    }

    int index = *it++;

    if (index < 0 || index >= this->items.size()) {
        return nullptr;
    }

    result = this->items[index].get();

    while (it != end) {
        index = *it++;
        if (index < 0 || index >= result->subItems.size()) {
            return nullptr;
        }
        result = result->subItems[index].get();
    }

    return result;
}

sw::MenuItem *sw::Menu::GetMenuItem(std::initializer_list<std::wstring> path)
{
    MenuItem *result = nullptr;

    std::initializer_list<std::wstring>::iterator it  = path.begin();
    std::initializer_list<std::wstring>::iterator end = path.end();

    if (it == end) {
        return nullptr;
    }

    for (std::shared_ptr<MenuItem> pItem : this->items) {
        if (pItem->text == *it) {
            result = pItem.get();
            ++it;
            break;
        }
    }

    if (result == nullptr) {
        return nullptr;
    }

    while (it != end) {
        MenuItem *p = nullptr;

        for (std::shared_ptr<MenuItem> pItem : result->subItems) {
            if (pItem->text == *it) {
                p = pItem.get();
                ++it;
                break;
            }
        }

        if (p == nullptr) {
            return nullptr;
        }

        result = p;
    }

    return result;
}

void sw::Menu::ClearAddedItems()
{
    while (GetMenuItemCount(this->_hMenu) > 0) {
        RemoveMenu(this->_hMenu, 0, MF_BYPOSITION);
    }

    for (HMENU hMenu : this->_popupMenus) {
        DestroyMenu(hMenu);
    }

    this->_dependencyInfoMap.clear();
    this->_popupMenus.clear();
    this->_leaves.clear();
}

void sw::Menu::AppendMenuItem(HMENU hMenu, std::shared_ptr<MenuItem> pItem, int index)
{
    this->_dependencyInfoMap[pItem.get()] = {hMenu, NULL, index};

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
    this->_dependencyInfoMap[pItem.get()].hSelf = hSubMenu;
    AppendMenuW(hMenu, MF_POPUP, reinterpret_cast<UINT_PTR>(hSubMenu), pItem->text.c_str());

    int i = 0;
    for (std::shared_ptr<MenuItem> pSubItem : pItem->subItems) {
        this->AppendMenuItem(hSubMenu, pSubItem, i++);
    }
}

void sw::Menu::RaiseMenuItemCommand(MenuItem &item)
{
    if (item.command)
        item.command(item);
}
