#include "ComboBox.h"

sw::ComboBox::ComboBox()
{
    this->InitControl(L"COMBOBOX", L"", WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST, 0);
    this->Rect = sw::Rect(0, 0, 100, 24);
}

int sw::ComboBox::GetItemsCount()
{
    // TODO
    return 0;
}

int sw::ComboBox::GetSelectedIndex()
{
    // TODO
    return 0;
}

void sw::ComboBox::SetSelectedIndex(int index)
{
    // TODO
}

std::wstring sw::ComboBox::GetSelectedItem()
{
    // TODO
    return std::wstring();
}

void sw::ComboBox::Clear()
{
    // TODO
}

std::wstring sw::ComboBox::GetItemAt(int index)
{
    // TODO
    return std::wstring();
}

bool sw::ComboBox::AddItem(const std::wstring &item)
{
    // TODO
    return false;
}

bool sw::ComboBox::InsertItem(int index, const std::wstring &item)
{
    // TODO
    return false;
}

bool sw::ComboBox::UpdateItem(int index, const std::wstring &newValue)
{
    // TODO
    return false;
}

bool sw::ComboBox::RemoveItemAt(int index)
{
    // TODO
    return false;
}
