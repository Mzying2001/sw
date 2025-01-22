#include "MenuItem.h"

sw::MenuItem::MenuItem(const std::wstring &text)
    : tag(0), text(text), command(nullptr)
{
}

sw::MenuItem::MenuItem(const std::wstring &text, const MenuItemCommand &command)
    : tag(0), text(text), command(command)
{
}

sw::MenuItem::MenuItem(const std::wstring &text, std::initializer_list<MenuItem> subItems)
    : tag(0), text(text), command(nullptr)
{
    for (const MenuItem &subItem : subItems) {
        std::shared_ptr<MenuItem> pSubItem = std::make_shared<MenuItem>(subItem);
        this->subItems.push_back(pSubItem);
    }
}

sw::MenuItem::MenuItem(uint64_t tag, const std::wstring &text)
    : tag(tag), text(text), command(nullptr)
{
}

sw::MenuItem::MenuItem(uint64_t tag, const std::wstring &text, const MenuItemCommand &command)
    : tag(tag), text(text), command(command)
{
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
