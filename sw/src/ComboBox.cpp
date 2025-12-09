#include "ComboBox.h"

namespace
{
    constexpr DWORD _ComboBoxStyle_Default  = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | CBS_AUTOHSCROLL | CBS_HASSTRINGS | CBS_DROPDOWNLIST;
    constexpr DWORD _ComboBoxStyle_Editable = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | CBS_AUTOHSCROLL | CBS_HASSTRINGS | CBS_DROPDOWN;
}

sw::ComboBox::ComboBox()
    : IsEditable(
          Property<bool>::Init(this)
              .Getter([](ComboBox *self) -> bool {
                  return self->GetStyle() == _ComboBoxStyle_Editable;
              })
              .Setter([](ComboBox *self, bool value) {
                  if (self->IsEditable != value) {
                      self->SetStyle(value ? _ComboBoxStyle_Editable : _ComboBoxStyle_Default);
                      self->ResetHandle();
                      self->SetInternalText(self->WndBase::GetInternalText()); // 使切换后文本框内容能够保留
                  }
              }))
{
    this->InitControl(L"COMBOBOX", L"", _ComboBoxStyle_Default, 0);
    this->Rect    = sw::Rect(0, 0, 100, 24);
    this->TabStop = true;
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
    this->OnSelectionChanged();
}

std::wstring sw::ComboBox::GetSelectedItem()
{
    return this->GetItemAt(this->GetSelectedIndex());
}

std::wstring &sw::ComboBox::GetInternalText()
{
    if (this->_isTextChanged) {
        this->UpdateInternalText();
        this->_isTextChanged = false;
    }
    return this->WndBase::GetInternalText();
}

void sw::ComboBox::SetInternalText(const std::wstring &value)
{
    // 当组合框可编辑时，直接调用WndBase::SetInternalText以更新文本框
    // 不可编辑时，直接修改_text字段（WndBase中定义，用于保存窗体文本）
    // 修改IsEditable属性后会重新创建句柄，会直接将_text字段设为新的文本
    // 这里直接修改_text以实现在IsEditable为false时修改的Text能够在IsEditable更改为true时文本框内容能正确显示

    if (this->IsEditable) {
        this->WndBase::SetInternalText(value);
    } else {
        this->WndBase::GetInternalText() = value;
        this->_isTextChanged             = false;
    }
}

void sw::ComboBox::OnCommand(int code)
{
    switch (code) {
        case CBN_EDITCHANGE:
            this->_isTextChanged = true;
            this->OnTextChanged();
            break;

        case CBN_SELCHANGE:
            this->OnSelectionChanged();
            this->OnTextChanged();
            break;

        default:
            break;
    }
}

void sw::ComboBox::OnSelectionChanged()
{
    this->_isTextChanged             = false;
    this->WndBase::GetInternalText() = this->GetSelectedItem();

    this->ItemsControl::OnSelectionChanged();
}

void sw::ComboBox::Clear()
{
    this->SendMessageW(CB_RESETCONTENT, 0, 0);
    this->RaisePropertyChanged(&ComboBox::ItemsCount);
}

std::wstring sw::ComboBox::GetItemAt(int index)
{
    int len = (int)this->SendMessageW(CB_GETLBTEXTLEN, index, 0);

    if (len <= 0) {
        return std::wstring{};
    }

    std::wstring result;
    result.resize(len + 1);
    this->SendMessageW(CB_GETLBTEXT, index, reinterpret_cast<LPARAM>(&result[0]));
    result.resize(len);
    return result;
}

bool sw::ComboBox::AddItem(const std::wstring &item)
{
    int count = this->GetItemsCount();
    this->SendMessageW(CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(item.c_str()));

    bool success = this->GetItemsCount() == count + 1;
    if (success) {
        this->RaisePropertyChanged(&ComboBox::ItemsCount);
    }
    return success;
}

bool sw::ComboBox::InsertItem(int index, const std::wstring &item)
{
    int count = this->GetItemsCount();
    this->SendMessageW(CB_INSERTSTRING, index, reinterpret_cast<LPARAM>(item.c_str()));
    this->RaisePropertyChanged(&ComboBox::ItemsCount);

    bool success = this->GetItemsCount() == count + 1;
    if (success) {
        this->RaisePropertyChanged(&ComboBox::ItemsCount);
    }
    return success;
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

    bool success = this->GetItemsCount() == count - 1;
    if (success) {
        this->RaisePropertyChanged(&ComboBox::ItemsCount);
    }
    return success;
}

void sw::ComboBox::ShowDropDown()
{
    this->SendMessageW(CB_SHOWDROPDOWN, TRUE, 0);
}

void sw::ComboBox::CloseDropDown()
{
    this->SendMessageW(CB_SHOWDROPDOWN, FALSE, 0);
}
