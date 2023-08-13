#include "ComboBox.h"

static constexpr DWORD _ComboBoxStyle_Default  = WS_CHILD | WS_VISIBLE | CBS_HASSTRINGS | CBS_DROPDOWNLIST;
static constexpr DWORD _ComboBoxStyle_Editable = WS_CHILD | WS_VISIBLE | CBS_HASSTRINGS | CBS_DROPDOWN;

sw::ComboBox::ComboBox()
    : IsEditable(
          // get
          [&]() -> const bool & {
              static bool result;
              result = this->GetStyle() == _ComboBoxStyle_Editable;
              return result;
          },
          // set
          [&](const bool &value) {
              if (this->IsEditable != value) {
                  this->SetStyle(value ? _ComboBoxStyle_Editable : _ComboBoxStyle_Default);
                  this->ResetHandle();
              }
          })
{
    this->InitControl(L"COMBOBOX", L"", _ComboBoxStyle_Default, 0);
    this->Rect = sw::Rect(0, 0, 100, 24);
}

int sw::ComboBox::GetItemsCount()
{
    return (int)this->SendMessageW(CB_GETCOUNT, 0, 0);
}

int sw::ComboBox::GetSelectedIndex()
{
    return (int)this->SendMessageW(CB_GETCURSEL, 0, 0);
}

void sw::ComboBox::SetSelectedIndex(int index)
{
    this->SendMessageW(CB_SETCURSEL, index, 0);
}

std::wstring sw::ComboBox::GetSelectedItem()
{
    return this->GetItemAt(this->GetSelectedIndex());
}

std::wstring &sw::ComboBox::GetText()
{
    if (this->_isTextChanged) {
        this->UpdateText();
        this->_isTextChanged = false;
    }
    return this->WndBase::GetText();
}

void sw::ComboBox::OnCommand(int code)
{
    switch (code) {
        case CBN_EDITCHANGE:
            this->_isTextChanged = true;
            break;

        default:
            break;
    }
}

void sw::ComboBox::Clear()
{
    this->SendMessageW(CB_RESETCONTENT, 0, 0);
}

std::wstring sw::ComboBox::GetItemAt(int index)
{
    int len = (int)this->SendMessageW(CB_GETLBTEXTLEN, index, 0);

    if (len <= 0) {
        return L"";
    }

    wchar_t *buf = new wchar_t[len + 1];
    this->SendMessageW(CB_GETLBTEXT, index, reinterpret_cast<LPARAM>(buf));

    std::wstring result = buf;

    delete[] buf;
    return result;
}

bool sw::ComboBox::AddItem(const std::wstring &item)
{
    int count = this->GetItemsCount();
    this->SendMessageW(CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(item.c_str()));
    return this->GetItemsCount() == count + 1;
}

bool sw::ComboBox::InsertItem(int index, const std::wstring &item)
{
    int count = this->GetItemsCount();
    this->SendMessageW(CB_INSERTSTRING, index, reinterpret_cast<LPARAM>(item.c_str()));
    return this->GetItemsCount() == count + 1;
}

bool sw::ComboBox::UpdateItem(int index, const std::wstring &newValue)
{
    bool selected = this->GetSelectedIndex() == index;
    bool updated  = this->RemoveItemAt(index) && this->InsertItem(index, newValue);

    if (updated && selected) {
        this->SetSelectedIndex(index);
    }

    return updated;
}

bool sw::ComboBox::RemoveItemAt(int index)
{
    int count = this->GetItemsCount();
    this->SendMessageW(CB_DELETESTRING, index, 0);
    return this->GetItemsCount() == count - 1;
}

void sw::ComboBox::ShowDropDown()
{
    this->SendMessageW(CB_SHOWDROPDOWN, TRUE, 0);
}

void sw::ComboBox::CloseDropDown()
{
    this->SendMessageW(CB_SHOWDROPDOWN, FALSE, 0);
}
