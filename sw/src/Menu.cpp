#include "Menu.h"

sw::MenuBase::MenuBase(MenuItem *root)
    : _root(root),

      ItemClicked(
          Event<MenuItemClickedEventHandler>::Init(this)
              .Delegate([](MenuBase *self) -> MenuItemClickedEventHandler & {
                  return self->_itemClicked;
              })),

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

bool sw::MenuBase::RaiseClickedEvent(int menuItemId)
{
    if (_root == nullptr) {
        return false;
    }

    MenuItem *item =
        _root->FindChildById(menuItemId);

    if (item == nullptr) {
        return false;
    }

    if (_itemClicked) {
        MenuItemClickedEventArgs args{};
        args.clickedItem = item;
        _itemClicked(*this, args);
    }
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
