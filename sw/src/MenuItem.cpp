#include "MenuItem.h"

sw::MenuItem::MenuItem(const std::wstring &text)
    : text(text)
{
}

sw::MenuItem::MenuItem(const std::wstring &text, std::initializer_list<MenuItem> subItems)
    : MenuItem(text)
{
    for (const MenuItem &subItem : subItems) {
        std::shared_ptr<MenuItem> pSubItem = std::make_shared<MenuItem>(subItem);
        this->subItems.push_back(pSubItem);
    }
}

sw::MenuItem::MenuItem(const std::wstring &text, const decltype(command) &command)
    : MenuItem(text)
{
    this->command = command;
}

bool sw::MenuItem::IsSeparator() const
{
    return this->text == L"-";
}
