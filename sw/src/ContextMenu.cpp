#include "ContextMenu.h"

/**
 * @brief 上下文菜单ID的起始位置，用于与普通菜单ID区分
 */
static constexpr int _ContextMenuIDFirst = 50000;

sw::ContextMenu::ContextMenu()
{
    this->InitMenuBase(CreatePopupMenu());
}

sw::ContextMenu::ContextMenu(std::initializer_list<MenuItem> items)
    : ContextMenu()
{
    this->SetItems(items);
}

bool sw::ContextMenu::IsContextMenuID(int id)
{
    return id >= _ContextMenuIDFirst;
}

int sw::ContextMenu::IndexToID(int index)
{
    return index + _ContextMenuIDFirst;
}

int sw::ContextMenu::IDToIndex(int id)
{
    return id - _ContextMenuIDFirst;
}
