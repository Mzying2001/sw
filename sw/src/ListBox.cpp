#include "ListBox.h"
#include "Dip.h"

sw::ListBox::ListBox()
    : Items(
          Property<ObservableCollection<std::wstring> *>::Init(this)
              .Getter([](ListBox *self) -> ObservableCollection<std::wstring> * {
                  return &self->_items;
              })),

      ItemHeight(
          Property<double>::Init(this)
              .Getter([](ListBox *self) -> double {
                  return Dip::PxToDipY((int)self->SendMessageW(LB_GETITEMHEIGHT, 0, 0));
              })
              .Setter([](ListBox *self, double value) {
                  if (self->ItemHeight != value) {
                      self->SendMessageW(LB_SETITEMHEIGHT, 0, Dip::DipToPxY(value));
                      self->RaisePropertyChanged(&ListBox::ItemHeight);
                  }
              })),

      TopIndex(
          Property<int>::Init(this)
              .Getter([](ListBox *self) -> int {
                  return (int)self->SendMessageW(LB_GETTOPINDEX, 0, 0);
              })
              .Setter([](ListBox *self, int value) {
                  self->SendMessageW(LB_SETTOPINDEX, value, 0);
              })),

      SelectedItemBackColor(
          Property<Color>::Init(this)
              .Getter([](ListBox *self) -> Color {
                  return self->_selectedItemBackColor;
              })
              .Setter([](ListBox *self, const Color &value) {
                  if (self->_selectedItemBackColor != value) {
                      self->_selectedItemBackColor = value;
                      self->Redraw();
                      self->RaisePropertyChanged(&ListBox::SelectedItemBackColor);
                  }
              })),

      SelectedItemTextColor(
          Property<Color>::Init(this)
              .Getter([](ListBox *self) -> Color {
                  return self->_selectedItemTextColor;
              })
              .Setter([](ListBox *self, const Color &value) {
                  if (self->_selectedItemTextColor != value) {
                      self->_selectedItemTextColor = value;
                      self->Redraw();
                      self->RaisePropertyChanged(&ListBox::SelectedItemTextColor);
                  }
              }))
{
    InitControl(
        L"LISTBOX", L"",
        WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_BORDER | WS_VSCROLL | LBS_NOTIFY | LBS_OWNERDRAWFIXED | LBS_NOINTEGRALHEIGHT | LBS_NODATA, 0);

    Rect    = sw::Rect(0, 0, 150, 200);
    TabStop = true;
    UpdateCurrentItemsSource();
}

void sw::ListBox::Refresh()
{
    _UpdateCount();
    Redraw();
}

int sw::ListBox::GetItemIndexFromPoint(const Point &point)
{
    POINT p = point;
    return (int)SendMessageW(LB_ITEMFROMPOINT, 0, MAKELPARAM(p.x, p.y));
}

sw::IList *sw::ListBox::GetDefaultItemsSource()
{
    return &_items;
}

void sw::ListBox::OnCurrentItemsSourceChanged(IList *oldItemsSource, IList *newItemsSource)
{
    Refresh();
}

void sw::ListBox::OnCurrentItemsSourceCollectionChanged(const NotifyCollectionChangedEventArgs &args)
{
    switch (args.action) {
        case NotifyCollectionChangedAction::Add:
        case NotifyCollectionChangedAction::Remove:
        case NotifyCollectionChangedAction::Reset:
            _UpdateCount();
            break;
        case NotifyCollectionChangedAction::Replace:
        case NotifyCollectionChangedAction::Move:
            break; // 数量未变无需更新
    }
    Redraw();
}

int sw::ListBox::GetSelectedIndex()
{
    return (int)SendMessageW(LB_GETCURSEL, 0, 0);
}

void sw::ListBox::SetSelectedIndex(int index)
{
    if (GetSelectedIndex() != index) {
        SendMessageW(LB_SETCURSEL, index, 0);
        OnSelectionChanged();
    }
}

void sw::ListBox::OnCommand(int code)
{
    if (code == LBN_SELCHANGE) {
        OnSelectionChanged();
    }
}

bool sw::ListBox::OnContextMenu(bool isKeyboardMsg, const Point &mousePosition)
{
    int index = GetItemIndexFromPoint(PointFromScreen(mousePosition));
    int count = ItemsCount;

    if (index >= 0 && index < count) {
        SetSelectedIndex(index);
    }
    return UIElement::OnContextMenu(isKeyboardMsg, mousePosition);
}

bool sw::ListBox::OnDrawItemSelf(DRAWITEMSTRUCT *pDrawItem)
{
    IList *items = GetCurrentItemsSource();

    int index = static_cast<int>(pDrawItem->itemID);
    int count = items ? items->Count() : 0;

    if (index < 0 || index >= count) {
        return true;
    }

    HDC hdc   = pDrawItem->hDC;
    RECT rect = pDrawItem->rcItem;

    std::wstring text =
        GetDisplayText(index, items->GetVariantAt(index));

    if (pDrawItem->itemState & ODS_SELECTED) {
        ::SetBkColor(hdc, static_cast<COLORREF>(_selectedItemBackColor));
        ::SetTextColor(hdc, static_cast<COLORREF>(_selectedItemTextColor));
    } else {
        ::SetBkColor(hdc, static_cast<COLORREF>(GetRealBackColor()));
        ::SetTextColor(hdc, static_cast<COLORREF>(GetRealTextColor()));
    }

    ::ExtTextOutW(hdc, rect.left, rect.top, ETO_OPAQUE, &rect, nullptr, 0, nullptr);
    ::DrawTextW(hdc, text.c_str(), -1, &rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    if ((pDrawItem->itemState & ODS_FOCUS) && IsFocusedViaTab) {
        ::DrawFocusRect(hdc, &rect);
    }
    return true;
}

std::wstring sw::ListBox::GetDisplayText(int index, const Variant &item)
{
    std::wstring text;

    if (item.IsType<std::wstring>()) {
        text = item.UnsafeCast<std::wstring>();
    }
    return text;
}

void sw::ListBox::_SetCount(int count)
{
    SendMessageW(LB_SETCOUNT, count, 0);
}

void sw::ListBox::_UpdateCount()
{
    int selectedIndex = GetSelectedIndex();

    IList *currentItemsSource = GetCurrentItemsSource();
    _SetCount(currentItemsSource ? currentItemsSource->Count() : 0);

    RaisePropertyChanged(&ItemsControl::ItemsCount);
    SetSelectedIndex(selectedIndex); // 尝试恢复选中项，若原选中项索引超出范围则会被重置为-1
}
