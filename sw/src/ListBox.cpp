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

      SelectedBackColor(
          Property<Color>::Init(this)
              .Getter([](ListBox *self) -> Color {
                  return self->_selectedBackColor;
              })
              .Setter([](ListBox *self, const Color &value) {
                  if (self->_selectedBackColor != value) {
                      self->_selectedBackColor = value;
                      self->Redraw();
                  }
              })),

      SelectedTextColor(
          Property<Color>::Init(this)
              .Getter([](ListBox *self) -> Color {
                  return self->_selectedTextColor;
              })
              .Setter([](ListBox *self, const Color &value) {
                  if (self->_selectedTextColor != value) {
                      self->_selectedTextColor = value;
                      self->Redraw();
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
    SendMessageW(LB_SETCURSEL, index, 0);
    OnSelectionChanged();
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
        return false;
    }

    std::wstring *pText = nullptr;

    if (items->GetVariantAt(index).IsType(&pText)) {
        HDC hdc   = pDrawItem->hDC;
        RECT rect = pDrawItem->rcItem;

        // 绘制背景
        if (pDrawItem->itemState & ODS_SELECTED) {
            ::SetBkColor(hdc, static_cast<COLORREF>(SelectedBackColor));
            ::SetTextColor(hdc, static_cast<COLORREF>(SelectedTextColor));
        } else {
            ::SetBkColor(hdc, static_cast<COLORREF>(BackColor));
            ::SetTextColor(hdc, static_cast<COLORREF>(TextColor));
        }

        ::ExtTextOutW(hdc, rect.left, rect.top, ETO_OPAQUE, &rect, nullptr, 0, nullptr);
        ::DrawTextW(hdc, pText->c_str(), -1, &rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
        return true;
    }

    return false;
}

void sw::ListBox::_SetCount(int count)
{
    SendMessageW(LB_SETCOUNT, count, 0);
    int a = GetLastError();
}

void sw::ListBox::_UpdateCount()
{
    IList *currentItemsSource = GetCurrentItemsSource();
    _SetCount(currentItemsSource ? currentItemsSource->Count() : 0);
    RaisePropertyChanged(&ItemsControl::ItemsCount);
}
