#include "ListBox.h"

sw::ListBox::ListBox()
    : TopIndex(
          // get
          [&]() -> const int & {
              static int result;
              result = (int)this->SendMessageW(LB_GETTOPINDEX, 0, 0);
              return result;
          },
          // set
          [&](const int &value) {
              this->SendMessageW(LB_SETTOPINDEX, value, 0);
          }),

      MultiSelect(
          // get
          [&]() -> const bool & {
              static bool result;
              result = this->GetStyle(LBS_MULTIPLESEL);
              return result;
          },
          // set
          [&](const bool &value) {
              if (this->GetStyle(LBS_MULTIPLESEL) != value) {
                  this->SetStyle(LBS_MULTIPLESEL, value);
                  this->ResetHandle();
              }
          }),

      SelectedCount(
          // get
          [&]() -> const int & {
              static int result;
              result = (int)this->SendMessageW(LB_GETSELCOUNT, 0, 0);
              return result;
          })
{
    this->InitControl(L"LISTBOX", L"", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_BORDER | WS_VSCROLL | LBS_NOINTEGRALHEIGHT | LBS_NOTIFY, 0);
    this->Rect    = sw::Rect(0, 0, 150, 200);
    this->TabStop = true;
}

int sw::ListBox::GetItemsCount()
{
    return (int)this->SendMessageW(LB_GETCOUNT, 0, 0);
}

int sw::ListBox::GetSelectedIndex()
{
    return (int)this->SendMessageW(LB_GETCURSEL, 0, 0);
}

void sw::ListBox::SetSelectedIndex(int index)
{
    this->SendMessageW(LB_SETCURSEL, index, 0);
    this->OnSelectionChanged();
}

std::wstring sw::ListBox::GetSelectedItem()
{
    return this->GetItemAt(this->GetSelectedIndex());
}

bool sw::ListBox::OnContextMenu(bool isKeyboardMsg, Point mousePosition)
{
    int index = this->GetItemIndexFromPoint(this->PointFromScreen(mousePosition));

    if (index >= 0 && index < this->GetItemsCount()) {
        this->SetSelectedIndex(index);
    }

    return this->UIElement::OnContextMenu(isKeyboardMsg, mousePosition);
}

void sw::ListBox::OnCommand(int code)
{
    if (code == LBN_SELCHANGE) {
        this->OnSelectionChanged();
    }
}

void sw::ListBox::Clear()
{
    this->SendMessageW(LB_RESETCONTENT, 0, 0);
}

std::wstring sw::ListBox::GetItemAt(int index)
{
    int len = (int)this->SendMessageW(LB_GETTEXTLEN, index, 0);

    if (len <= 0) {
        return L"";
    }

    wchar_t *buf = new wchar_t[len + 1];
    this->SendMessageW(LB_GETTEXT, index, reinterpret_cast<LPARAM>(buf));

    std::wstring result = buf;

    delete[] buf;
    return result;
}

bool sw::ListBox::AddItem(const std::wstring &item)
{
    int count = this->GetItemsCount();
    this->SendMessageW(LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(item.c_str()));
    return this->GetItemsCount() == count + 1;
}

bool sw::ListBox::InsertItem(int index, const std::wstring &item)
{
    int count = this->GetItemsCount();
    this->SendMessageW(LB_INSERTSTRING, index, reinterpret_cast<LPARAM>(item.c_str()));
    return this->GetItemsCount() == count + 1;
}

bool sw::ListBox::UpdateItem(int index, const std::wstring &newValue)
{
    bool selected = this->GetSelectedIndex() == index;
    bool updated  = this->RemoveItemAt(index) && this->InsertItem(index, newValue.c_str());

    if (updated && selected) {
        this->SetSelectedIndex(index);
    }

    return updated;
}

bool sw::ListBox::RemoveItemAt(int index)
{
    int count = this->GetItemsCount();
    this->SendMessageW(LB_DELETESTRING, index, 0);
    return this->GetItemsCount() == count - 1;
}

int sw::ListBox::GetItemIndexFromPoint(const Point &point)
{
    POINT p = point;
    return (int)this->SendMessageW(LB_ITEMFROMPOINT, 0, MAKELPARAM(p.x, p.y));
}

sw::List<int> sw::ListBox::GetSelectedIndices()
{
    List<int> result;
    int selectedCount = this->SelectedCount.Get();
    if (selectedCount > 0) {
        int *buf = new int[selectedCount];
        if (this->SendMessageW(LB_GETSELITEMS, selectedCount, reinterpret_cast<LPARAM>(buf)) != LB_ERR) {
            for (int i = 0; i < selectedCount; ++i) result.Append(buf[i]);
        }
        delete[] buf;
    }
    return result;
}

sw::StrList sw::ListBox::GetSelectedItems()
{
    StrList result;
    for (int i : this->GetSelectedIndices()) {
        result.Append(this->GetItemAt(i));
    }
    return result;
}

bool sw::ListBox::GetItemSelectionState(int index)
{
    return this->SendMessageW(LB_GETSEL, index, 0) > 0;
}

void sw::ListBox::SetItemSelectionState(int index, bool value)
{
    this->SendMessageW(LB_SETSEL, value, index);
}
