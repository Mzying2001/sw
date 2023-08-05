#include "Menu.h"

sw::Menu::Menu()
{
    this->InitMenuBase(CreateMenu());
}

sw::Menu::Menu(std::initializer_list<MenuItem> items)
    : Menu()
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
