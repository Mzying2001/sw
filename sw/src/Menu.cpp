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
        args.menu = this;
        _itemClicked(*item, args);
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

bool sw::ContextMenu::Show(
    HWND hwnd, const Point &point, sw::HorizontalAlignment horz, sw::VerticalAlignment vert)
{
    UINT uFlags = 0;
    HMENU hMenu = Handle;

    if (hMenu == NULL) {
        return false;
    }

    switch (horz) {
        case sw::HorizontalAlignment::Left: {
            uFlags |= TPM_LEFTALIGN;
            break;
        }
        case sw::HorizontalAlignment::Right: {
            uFlags |= TPM_RIGHTALIGN;
            break;
        }
        case sw::HorizontalAlignment::Center:
        case sw::HorizontalAlignment::Stretch: {
            uFlags |= TPM_CENTERALIGN;
            break;
        }
    }

    switch (vert) {
        case sw::VerticalAlignment::Top: {
            uFlags |= TPM_TOPALIGN;
            break;
        }
        case sw::VerticalAlignment::Bottom: {
            uFlags |= TPM_BOTTOMALIGN;
            break;
        }
        case sw::VerticalAlignment::Center:
        case sw::VerticalAlignment::Stretch: {
            uFlags |= TPM_VCENTERALIGN;
            break;
        }
    }

    POINT pos = point;
    SetForegroundWindow(hwnd); // 确保菜单能正确关闭
    return TrackPopupMenu(hMenu, uFlags, pos.x, pos.y, 0, hwnd, nullptr);
}
