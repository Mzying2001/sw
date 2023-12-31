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

void sw::MenuItem::CallCommand()
{
    if (this->command)
        this->command(*this);
}

uint64_t sw::MenuItem::GetTag()
{
    return this->tag;
}

void sw::MenuItem::SetTag(uint64_t tag)
{
    this->tag = tag;
}
