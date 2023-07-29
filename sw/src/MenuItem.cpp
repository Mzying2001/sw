#include "MenuItem.h"
#include <algorithm>

sw::MenuItem::MenuItem(const std::wstring text)
    : _text(text), _isSeparator(text == L"-")
{
}

const std::wstring &sw::MenuItem::GetText()
{
    return this->_text;
}

bool sw::MenuItem::IsSparator()
{
    return this->_isSeparator;
}

int sw::MenuItem::GetSubItemCount()
{
    return (int)this->_subItems.size();
}

sw::MenuItem &sw::MenuItem::GetSubItemAt(int index)
{
    return this->_subItems[index];
}

sw::MenuItem &sw::MenuItem::AppendSubItem(const std::wstring &itemText)
{
    this->_subItems.emplace_back(itemText);
    return *this->_subItems.rbegin();
}

void sw::MenuItem::AppendSeparator()
{
    this->AppendSubItem(L"-");
}
