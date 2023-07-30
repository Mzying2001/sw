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

bool sw::Menu::GetEnabled(MenuItem &item, bool &out)
{
    auto dependencyInfo = this->GetMenuItemDependencyInfo(item);

    if (dependencyInfo == nullptr) {
        return false;
    }

    MENUITEMINFOW info{};
    info.cbSize = sizeof(info);
    info.fMask  = MIIM_STATE;

    if (!GetMenuItemInfoW(dependencyInfo->hParent, dependencyInfo->index, TRUE, &info)) {
        return false;
    }

    out = (info.fState & MFS_DISABLED) != MFS_DISABLED;
    return true;
}

bool sw::Menu::SetEnabled(MenuItem &item, bool value)
{
    auto dependencyInfo = this->GetMenuItemDependencyInfo(item);

    if (dependencyInfo == nullptr) {
        return false;
    }

    MENUITEMINFOW info{};
    info.cbSize = sizeof(info);
    info.fMask  = MIIM_STATE;

    if (!GetMenuItemInfoW(dependencyInfo->hParent, dependencyInfo->index, TRUE, &info)) {
        return false;
    }

    if (value) {
        info.fState &= ~MFS_DISABLED;
    } else {
        info.fState |= MFS_DISABLED;
    }

    return SetMenuItemInfoW(dependencyInfo->hParent, dependencyInfo->index, TRUE, &info);
}

bool sw::Menu::GetChecked(MenuItem &item, bool &out)
{
    auto dependencyInfo = this->GetMenuItemDependencyInfo(item);

    if (dependencyInfo == nullptr) {
        return false;
    }

    MENUITEMINFOW info{};
    info.cbSize = sizeof(info);
    info.fMask  = MIIM_STATE;

    if (!GetMenuItemInfoW(dependencyInfo->hParent, dependencyInfo->index, TRUE, &info)) {
        return false;
    }

    out = (info.fState & MFS_CHECKED) == MFS_CHECKED;
    return true;
}

bool sw::Menu::SetChecked(MenuItem &item, bool value)
{
    auto dependencyInfo = this->GetMenuItemDependencyInfo(item);

    if (dependencyInfo == nullptr) {
        return false;
    }

    MENUITEMINFOW info{};
    info.cbSize = sizeof(info);
    info.fMask  = MIIM_STATE;

    if (!GetMenuItemInfoW(dependencyInfo->hParent, dependencyInfo->index, TRUE, &info)) {
        return false;
    }

    if (value) {
        info.fState |= MFS_CHECKED;
    } else {
        info.fState &= ~MFS_CHECKED;
    }

    return SetMenuItemInfoW(dependencyInfo->hParent, dependencyInfo->index, TRUE, &info);
}

bool sw::Menu::SetText(MenuItem &item, const std::wstring &value)
{
    auto dependencyInfo = this->GetMenuItemDependencyInfo(item);

    if (dependencyInfo == nullptr) {
        return false;
    }

    MENUITEMINFOW info{};
    info.cbSize     = sizeof(info);
    info.fMask      = MIIM_STRING;
    info.dwTypeData = const_cast<LPWSTR>(value.c_str());

    bool success = SetMenuItemInfoW(dependencyInfo->hParent, dependencyInfo->index, TRUE, &info);

    if (success) {
        item.text = value;
    }

    return success;
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

sw::Menu::_MenuItemDependencyInfo *sw::Menu::GetMenuItemDependencyInfo(MenuItem &item)
{
    MenuItem *p = &item;
    return this->_dependencyInfoMap.count(p) ? &this->_dependencyInfoMap[p] : nullptr;
}
