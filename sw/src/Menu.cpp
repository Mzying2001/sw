#include "Menu.h"

sw::MenuBase::MenuBase(MenuItem *root)
    : _root(root),

      Root(
          Property<MenuItem *>::Init(this)
              .Getter([](MenuBase *self) -> MenuItem * {
                  return self->_root.get();
              })),

      Handle(
          Property<HMENU>::Init(this)
              .Getter([](MenuBase *self) -> HMENU {
                  return self->_root ? self->_root->Handle.Get() : NULL;
              }))
{
}

bool sw::MenuBase::RaiseClickEvent(int menuItemId)
{
    if (_root == nullptr) {
        return false;
    }

    MenuItem *item =
        _root->FindChildById(menuItemId);

    if (item == nullptr) {
        return false;
    }

    // TODO: 触发菜单项点击事件
    return true;
}

sw::Menu::Menu()
    : MenuBase(MenuItem::CreateRoot(false))
{
}

sw::Menu::Menu(std::initializer_list<MenuItemDesc> items)
    : Menu()
{
    Root->ResetChildren(items);
}

sw::ContextMenu::ContextMenu()
    : MenuBase(MenuItem::CreateRoot(true))
{
}

sw::ContextMenu::ContextMenu(std::initializer_list<MenuItemDesc> items)
    : ContextMenu()
{
    Root->ResetChildren(items);
}
