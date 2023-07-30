#include "Menu.h"

sw::Menu::Menu()
    : MenuBase()
{
}

sw::Menu::Menu(std::initializer_list<MenuItem> items)
    : MenuBase()
{
    this->SetItems(items);
}

int sw::Menu::IndexToID(int index)
{
    return index;
}

int sw::Menu::IDToIndex(int id)
{
    return id;
}
