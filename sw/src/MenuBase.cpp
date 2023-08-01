#include "MenuBase.h"

sw::MenuBase::MenuBase()
{
    this->_hMenu = CreateMenu();
}

sw::MenuBase::MenuBase(const MenuBase &menu)
    : MenuBase()
{
    this->items = menu.items;
    this->Update();
}

sw::MenuBase::~MenuBase()
{
    this->_ClearAddedItems();
    DestroyMenu(this->_hMenu);
}

sw::MenuBase &sw::MenuBase::operator=(const MenuBase &menu)
{
    this->items = menu.items;
    this->Update();
    return *this;
}

HMENU sw::MenuBase::GetHandle()
{
    return this->_hMenu;
}

void sw::MenuBase::Update()
{
    this->_ClearAddedItems();

    int i = 0;
    for (std::shared_ptr<MenuItem> pItem : this->items) {
        if (!pItem->IsSeparator())
            this->_AppendMenuItem(this->_hMenu, pItem, i++);
    }
}

void sw::MenuBase::SetItems(std::initializer_list<MenuItem> items)
{
    this->_ClearAddedItems();

    for (const MenuItem &item : items) {
        std::shared_ptr<MenuItem> pItem = std::make_shared<MenuItem>(item);
        this->items.push_back(pItem);
    }

    this->Update();
}

bool sw::MenuBase::SetSubItems(MenuItem &item, std::initializer_list<MenuItem> subItems)
{
    auto dependencyInfo = this->_GetMenuItemDependencyInfo(item);

    if (dependencyInfo == nullptr) {
        return false;
    }

    if (dependencyInfo->hSelf == NULL) {
        item = MenuItem(item.text, subItems);
        this->Update();
        return true;
    }

    while (GetMenuItemCount(dependencyInfo->hSelf) > 0) {
        RemoveMenu(dependencyInfo->hSelf, 0, MF_BYPOSITION);
    }

    item.subItems.clear();

    int i = 0;
    for (const MenuItem &subItem : subItems) {
        std::shared_ptr<MenuItem> pSubItem(new MenuItem(subItem));
        item.subItems.push_back(pSubItem);
        this->_AppendMenuItem(dependencyInfo->hSelf, pSubItem, i++);
    }

    return true;
}

void sw::MenuBase::AddItem(const MenuItem &item)
{
    std::shared_ptr<MenuItem> pItem(new MenuItem(item));
    this->_AppendMenuItem(this->_hMenu, pItem, (int)this->items.size());
    this->items.push_back(pItem);
}

bool sw::MenuBase::AddSubItem(MenuItem &item, const MenuItem &subItem)
{
    auto dependencyInfo = this->_GetMenuItemDependencyInfo(item);

    if (dependencyInfo == nullptr) {
        return false;
    }

    if (dependencyInfo->hSelf == NULL) {
        item.subItems.emplace_back(new MenuItem(subItem));
        this->Update();
        return true;
    }

    std::shared_ptr<MenuItem> pSubItem(new MenuItem(subItem));
    this->_AppendMenuItem(dependencyInfo->hSelf, pSubItem, (int)item.subItems.size());
    item.subItems.push_back(pSubItem);
    return true;
}

sw::MenuItem *sw::MenuBase::GetMenuItem(int id)
{
    int index = this->IDToIndex(id);
    return index >= 0 && index < this->_leaves.size() ? this->_leaves[index].get() : nullptr;
}

sw::MenuItem *sw::MenuBase::GetMenuItem(std::initializer_list<int> path)
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

sw::MenuItem *sw::MenuBase::GetMenuItem(std::initializer_list<std::wstring> path)
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

sw::MenuItem *sw::MenuBase::GetParent(MenuItem &item)
{
    auto dependencyInfo = this->_GetMenuItemDependencyInfo(item);

    if (dependencyInfo == nullptr) {
        return nullptr;
    }

    for (auto &tuple : this->_popupMenus) {
        if (std::get<1>(tuple) == dependencyInfo->hParent) {
            return std::get<0>(tuple).get();
        }
    }

    return nullptr;
}

bool sw::MenuBase::GetEnabled(MenuItem &item, bool &out)
{
    auto dependencyInfo = this->_GetMenuItemDependencyInfo(item);

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

bool sw::MenuBase::SetEnabled(MenuItem &item, bool value)
{
    auto dependencyInfo = this->_GetMenuItemDependencyInfo(item);

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

bool sw::MenuBase::GetChecked(MenuItem &item, bool &out)
{
    auto dependencyInfo = this->_GetMenuItemDependencyInfo(item);

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

bool sw::MenuBase::SetChecked(MenuItem &item, bool value)
{
    auto dependencyInfo = this->_GetMenuItemDependencyInfo(item);

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

bool sw::MenuBase::SetText(MenuItem &item, const std::wstring &value)
{
    auto dependencyInfo = this->_GetMenuItemDependencyInfo(item);

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

void sw::MenuBase::_ClearAddedItems()
{
    while (GetMenuItemCount(this->_hMenu) > 0) {
        RemoveMenu(this->_hMenu, 0, MF_BYPOSITION);
    }

    for (auto &tuple : this->_popupMenus) {
        DestroyMenu(std::get<1>(tuple));
    }

    this->_dependencyInfoMap.clear();
    this->_popupMenus.clear();
    this->_leaves.clear();
}

void sw::MenuBase::_AppendMenuItem(HMENU hMenu, std::shared_ptr<MenuItem> pItem, int index)
{
    this->_dependencyInfoMap[pItem.get()] = {hMenu, NULL, index};

    if (pItem->IsSeparator()) {
        AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);
        return;
    }

    if (pItem->subItems.size() == 0) {
        int id = this->IndexToID(int(this->_leaves.size()));
        AppendMenuW(hMenu, MF_STRING, id, pItem->text.c_str());
        this->_leaves.push_back(pItem);
        return;
    }

    HMENU hSubMenu = CreatePopupMenu();
    this->_popupMenus.push_back({pItem, hSubMenu});
    this->_dependencyInfoMap[pItem.get()].hSelf = hSubMenu;
    AppendMenuW(hMenu, MF_POPUP, reinterpret_cast<UINT_PTR>(hSubMenu), pItem->text.c_str());

    int i = 0;
    for (std::shared_ptr<MenuItem> pSubItem : pItem->subItems) {
        this->_AppendMenuItem(hSubMenu, pSubItem, i++);
    }
}

sw::MenuBase::_MenuItemDependencyInfo *sw::MenuBase::_GetMenuItemDependencyInfo(MenuItem &item)
{
    MenuItem *p = &item;
    return this->_dependencyInfoMap.count(p) ? &this->_dependencyInfoMap[p] : nullptr;
}