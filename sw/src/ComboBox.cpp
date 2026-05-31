#include "ComboBox.h"

sw::ComboBox::ComboBox()
    : Items(
          Property<ObservableCollection<std::wstring> *>::Init(this)
              .Getter([](ComboBox *self) -> ObservableCollection<std::wstring> * {
                  return &self->_items;
              })),

      IsEditable(
          Property<bool>::Init(this)
              .Getter([](ComboBox *self) -> bool {
                  return (self->GetStyle() & (CBS_DROPDOWN | CBS_DROPDOWNLIST)) == CBS_DROPDOWN;
              })
              .Setter([](ComboBox *self, bool value) {
                  if (self->IsEditable != value) {
                      auto baseStyle = self->GetStyle() & ~(CBS_DROPDOWN | CBS_DROPDOWNLIST);
                      self->SetStyle(baseStyle | (value ? CBS_DROPDOWN : CBS_DROPDOWNLIST));
                      self->ResetHandle();
                      self->Refresh();
                      self->OnSelectionChanged();
                      self->RaisePropertyChanged(&ComboBox::IsEditable);
                  }
              }))
{
    InitControl(
        L"COMBOBOX", L"",
        WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | CBS_AUTOHSCROLL | CBS_DROPDOWNLIST | CBS_HASSTRINGS, 0);

    Rect    = sw::Rect(0, 0, 100, 24);
    TabStop = true;
    UpdateCurrentItemsSource();
}

void sw::ComboBox::Refresh()
{
    int index = GetSelectedIndex();
    SendMessageW(WM_SETREDRAW, FALSE, 0);
    _UpdateItems();
    SendMessageW(WM_SETREDRAW, TRUE, 0);
    SetSelectedIndex(index);
    _UpdateSelectedText();
    Redraw();
}

void sw::ComboBox::ShowDropDown()
{
    SendMessageW(CB_SHOWDROPDOWN, TRUE, 0);
}

void sw::ComboBox::CloseDropDown()
{
    SendMessageW(CB_SHOWDROPDOWN, FALSE, 0);
}

sw::IList *sw::ComboBox::GetDefaultItemsSource()
{
    return &_items;
}

void sw::ComboBox::OnCurrentItemsSourceChanged(IList *oldItemsSource, IList *newItemsSource)
{
    Refresh();
}

void sw::ComboBox::OnCurrentItemsSourceCollectionChanged(const NotifyCollectionChangedEventArgs &args)
{
    int index = GetSelectedIndex();
    SendMessageW(WM_SETREDRAW, FALSE, 0);

    switch (args.action) {
        case NotifyCollectionChangedAction::Add:
            _AddString(GetDisplayText(args.index, args.list->GetVariantAt(args.index)));
            break;

        case NotifyCollectionChangedAction::Remove:
            if (index == args.index) {
                index = -1;
            } else if (index > args.index) {
                index--;
            }
            _DeleteString(args.index);
            break;

        case NotifyCollectionChangedAction::Reset:
            _UpdateItems();
            break;

        case NotifyCollectionChangedAction::Replace:
            _DeleteString(args.index);
            _InsertString(args.index, GetDisplayText(args.index, args.list->GetVariantAt(args.index)));
            break;

        case NotifyCollectionChangedAction::Move:
            if (index == args.oldIndex) {
                index = args.index;
            } else if (index > args.oldIndex && index <= args.index) {
                index--;
            } else if (index < args.oldIndex && index >= args.index) {
                index++;
            }
            _DeleteString(args.oldIndex);
            _InsertString(args.index, GetDisplayText(args.index, args.list->GetVariantAt(args.index)));
            break;
    }

    SetSelectedIndex(index);
    SendMessageW(WM_SETREDRAW, TRUE, 0);
    Redraw();
}

int sw::ComboBox::GetSelectedIndex()
{
    return (int)SendMessageW(CB_GETCURSEL, 0, 0);
}

void sw::ComboBox::SetSelectedIndex(int index)
{
    if (GetSelectedIndex() != index) {
        SendMessageW(CB_SETCURSEL, index, 0);
        OnSelectionChanged();
    }
}

void sw::ComboBox::OnCommand(int code)
{
    if (code == CBN_SELCHANGE) {
        OnSelectionChanged();
    }
}

void sw::ComboBox::OnSelectionChanged()
{
    _UpdateSelectedText();
    ItemsControl::OnSelectionChanged();
}

std::wstring sw::ComboBox::GetDisplayText(int index, const Variant &item)
{
    std::wstring text;

    if (item.IsType<std::wstring>()) {
        text = item.UnsafeCast<std::wstring>();
    }
    return text;
}

void sw::ComboBox::_UpdateSelectedText()
{
    auto items = GetCurrentItemsSource();
    int index  = GetSelectedIndex();

    if (items == nullptr || index < 0 || index >= items->Count()) {
        SetInternalText(L"");
    } else {
        SetInternalText(GetDisplayText(index, items->GetVariantAt(index)));
    }
}

void sw::ComboBox::_UpdateItems()
{
    SendMessageW(CB_RESETCONTENT, 0, 0);

    IList *items = GetCurrentItemsSource();
    if (items == nullptr) return;

    for (int i = 0; i < items->Count(); i++) {
        std::wstring text = GetDisplayText(i, items->GetVariantAt(i));
        SendMessageW(CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(text.c_str()));
    }
}

void sw::ComboBox::_AddString(const std::wstring &str)
{
    SendMessageW(CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(str.c_str()));
}

void sw::ComboBox::_InsertString(int index, const std::wstring &str)
{
    SendMessageW(CB_INSERTSTRING, index, reinterpret_cast<LPARAM>(str.c_str()));
}

void sw::ComboBox::_DeleteString(int index)
{
    SendMessageW(CB_DELETESTRING, index, 0);
}
