#include "ListView.h"

sw::ListView::ListView()
{
    this->InitControl(WC_LISTVIEWW, L"", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_BORDER | LVS_REPORT, 0);
    this->Rect    = sw::Rect(0, 0, 200, 200);
    this->TabStop = true;
}

int sw::ListView::GetItemsCount()
{
    // TODO
    return 0;
}

int sw::ListView::GetSelectedIndex()
{
    // TODO
    return 0;
}

void sw::ListView::SetSelectedIndex(int index)
{
    // TODO
}

sw::StrList sw::ListView::GetSelectedItem()
{
    // TODO
    return StrList();
}

void sw::ListView::Clear()
{
    // TODO
}

sw::StrList sw::ListView::GetItemAt(int index)
{
    // TODO
    return StrList();
}

bool sw::ListView::AddItem(const StrList &item)
{
    // TODO
    return false;
}

bool sw::ListView::InsertItem(int index, const StrList &item)
{
    // TODO
    return false;
}

bool sw::ListView::UpdateItem(int index, const StrList &newValue)
{
    // TODO
    return false;
}

bool sw::ListView::RemoveItemAt(int index)
{
    // TODO
    return false;
}
