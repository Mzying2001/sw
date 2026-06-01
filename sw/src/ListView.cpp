#include "ListView.h"
#include "Dip.h"
#include <climits>
#include <cmath>
#include <memory>

sw::ListViewColumn::ListViewColumn(const std::wstring &header, double width)
    : header(header), width(width)
{
}

sw::ListViewColumn::ListViewColumn(const LVCOLUMNW &lvc)
{
    double scaleX = Dip::ScaleX;

    if (lvc.mask & LVCF_TEXT) {
        header = lvc.pszText;
    } else {
        header = L"";
    }

    if (lvc.mask & LVCF_WIDTH) {
        width = lvc.cx * scaleX;
    } else {
        width = 0;
    }

    if (lvc.mask & LVCF_FMT) {
        if (lvc.fmt & LVCFMT_RIGHT) {
            alignment = ListViewColumnAlignment::Right;
        } else if (lvc.fmt & LVCFMT_CENTER) {
            alignment = ListViewColumnAlignment::Center;
        } /*else { alignment = ListViewColumnAlignment::Left; }*/
    }
}

sw::ListViewColumn::operator LVCOLUMNW() const
{
    double scaleX = Dip::ScaleX;

    LVCOLUMNW lvc{};
    lvc.mask    = LVCF_TEXT | LVCF_FMT;
    lvc.pszText = const_cast<LPWSTR>(header.c_str());
    lvc.fmt     = (int)alignment;

    if (width >= 0) {
        lvc.mask |= LVCF_WIDTH;
        lvc.cx = std::lround(width / scaleX);
    }
    return lvc;
}

sw::ListViewItem::ListViewItem(ListViewItem &&other) noexcept
    : subItems(std::move(other.subItems)), imageIndex(other.imageIndex), checked(other.checked)
{
    other.imageIndex = -1;
    other.checked    = false;
}

sw::ListViewItem::ListViewItem(std::initializer_list<std::wstring> subItems)
    : subItems(subItems)
{
}

sw::ListView::ListView()
    : Items(
          Property<ObservableCollection<ListViewItem> *>::Init(this)
              .Getter([](ListView *self) -> ObservableCollection<ListViewItem> * {
                  return &self->_items;
              })),

      ColumnsCount(
          Property<int>::Init(this)
              .Getter([](ListView *self) -> int {
                  return self->_GetColCount();
              })),

      GridLines(
          Property<bool>::Init(this)
              .Getter([](ListView *self) -> bool {
                  return self->_GetExtendedListViewStyle() & LVS_EX_GRIDLINES;
              })
              .Setter([](ListView *self, bool value) {
                  DWORD style;
                  style = self->_GetExtendedListViewStyle();
                  style = value ? (style | LVS_EX_GRIDLINES) : (style & (~LVS_EX_GRIDLINES));
                  self->_SetExtendedListViewStyle(style);
              })),

      MultiSelect(
          Property<bool>::Init(this)
              .Getter([](ListView *self) -> bool {
                  return !(self->GetStyle() & LVS_SINGLESEL);
              })
              .Setter([](ListView *self, bool value) {
                  self->SetStyle(LVS_SINGLESEL, !value);
              })),

      SelectedCount(
          Property<int>::Init(this)
              .Getter([](ListView *self) -> int {
                  return (int)self->SendMessageW(LVM_GETSELECTEDCOUNT, 0, 0);
              })),

      CheckBoxes(
          Property<bool>::Init(this)
              .Getter([](ListView *self) -> bool {
                  return self->_GetExtendedListViewStyle() & LVS_EX_CHECKBOXES;
              })
              .Setter([](ListView *self, bool value) {
                  DWORD style;
                  style = self->_GetExtendedListViewStyle();
                  style = value ? (style | LVS_EX_CHECKBOXES) : (style & (~LVS_EX_CHECKBOXES));
                  self->_SetExtendedListViewStyle(style);
              })),

      TopIndex(
          Property<int>::Init(this)
              .Getter([](ListView *self) -> int {
                  return (int)self->SendMessageW(LVM_GETTOPINDEX, 0, 0);
              })),

      ShareImageLists(
          Property<bool>::Init(this)
              .Getter([](ListView *self) -> bool {
                  return self->GetStyle(LVS_SHAREIMAGELISTS);
              })
              .Setter([](ListView *self, bool value) {
                  self->SetStyle(LVS_SHAREIMAGELISTS, value);
              })),

      Editable(
          Property<bool>::Init(this)
              .Getter([](ListView *self) -> bool {
                  return self->GetStyle(LVS_EDITLABELS);
              })
              .Setter([](ListView *self, bool value) {
                  self->SetStyle(LVS_EDITLABELS, value);
              }))
{
    InitControl(
        WC_LISTVIEWW, L"",
        WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_BORDER | LVS_REPORT | LVS_OWNERDATA, 0);

    _SetExtendedListViewStyle(LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);

    Rect    = sw::Rect(0, 0, 200, 200);
    TabStop = true;
    UpdateCurrentItemsSource();
}

void sw::ListView::Refresh()
{
    _UpdateCount();
    Redraw();
}

bool sw::ListView::AddColumn(const ListViewColumn &column)
{
    return InsertColumn(_GetColCount(), column);
}

bool sw::ListView::AddColumn(const std::wstring &header)
{
    ListViewColumn column(header);
    return InsertColumn(_GetColCount(), column);
}

bool sw::ListView::InsertColumn(int index, const ListViewColumn &column)
{
    LVCOLUMNW lvc = column;
    return SendMessageW(LVM_INSERTCOLUMNW, index, reinterpret_cast<LPARAM>(&lvc)) != -1;
}

bool sw::ListView::InsertColumn(int index, const std::wstring &header)
{
    ListViewColumn column(header);
    return InsertColumn(index, column);
}

bool sw::ListView::SetColumnHeader(int index, const std::wstring &header)
{
    LVCOLUMNW lvc;
    lvc.mask    = LVCF_TEXT;
    lvc.pszText = const_cast<LPWSTR>(header.c_str());
    return SendMessageW(LVM_SETCOLUMNW, index, reinterpret_cast<LPARAM>(&lvc));
}

double sw::ListView::GetColumnWidth(int index)
{
    if (index < 0 || index >= _GetColCount()) {
        return -1;
    } else {
        return SendMessageW(LVM_GETCOLUMNWIDTH, index, 0) * Dip::ScaleX;
    }
}

bool sw::ListView::SetColumnWidth(int index, double width)
{
    return SendMessageW(LVM_SETCOLUMNWIDTH, index, std::lround(width / Dip::ScaleX));
}

bool sw::ListView::RemoveColumnAt(int index)
{
    return SendMessageW(LVM_DELETECOLUMN, index, 0);
}

sw::List<int> sw::ListView::GetSelectedIndices()
{
    List<int> result;

    for (int i = -1; (i = (int)SendMessageW(LVM_GETNEXTITEM, i, LVNI_SELECTED)) != -1;) {
        result.Add(i);
    }
    return result;
}

bool sw::ListView::GetItemCheckState(int index)
{
    bool checked = false;

    OnGetItemCheckState(index, checked);
    return checked;
}

void sw::ListView::SetItemCheckState(int index, bool value)
{
    OnSetItemCheckState(index, value);
}

int sw::ListView::GetItemIndexFromPoint(const Point &point)
{
    LVHITTESTINFO hitTestInfo{};
    hitTestInfo.pt = point;
    return (int)SendMessageW(LVM_HITTEST, 0, reinterpret_cast<LPARAM>(&hitTestInfo));
}

sw::ImageList sw::ListView::GetImageList(ListViewImageList imageList)
{
    return ImageList::Wrap((HIMAGELIST)SendMessageW(LVM_GETIMAGELIST, (WPARAM)imageList, 0));
}

HIMAGELIST sw::ListView::SetImageList(ListViewImageList imageList, HIMAGELIST value)
{
    return (HIMAGELIST)SendMessageW(LVM_SETIMAGELIST, (WPARAM)imageList, (LPARAM)value);
}

bool sw::ListView::EditItem(int index)
{
    return SendMessageW(LVM_EDITLABELW, index, 0) != 0;
}

void sw::ListView::CancelEdit()
{
    SendMessageW(LVM_CANCELEDITLABEL, 0, 0);
}

sw::IList *sw::ListView::GetDefaultItemsSource()
{
    return &_items;
}

void sw::ListView::OnCurrentItemsSourceChanged(IList *oldItemsSource, IList *newItemsSource)
{
    Refresh();
}

void sw::ListView::OnCurrentItemsSourceCollectionChanged(const NotifyCollectionChangedEventArgs &args)
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

int sw::ListView::GetSelectedIndex()
{
    return (int)SendMessageW(LVM_GETNEXTITEM, -1, LVNI_SELECTED);
}

void sw::ListView::SetSelectedIndex(int index)
{
    LVITEMW lvi;

    lvi.stateMask = LVIS_SELECTED;
    lvi.state     = 0;
    SendMessageW(LVM_SETITEMSTATE, -1, reinterpret_cast<LPARAM>(&lvi));

    lvi.stateMask = LVIS_SELECTED | LVIS_FOCUSED;
    lvi.state     = LVIS_SELECTED | LVIS_FOCUSED;
    SendMessageW(LVM_SETITEMSTATE, index, reinterpret_cast<LPARAM>(&lvi));
}

void sw::ListView::SetBackColor(Color color, bool redraw)
{
    Control::SetBackColor(color, false);
    SendMessageW(LVM_SETBKCOLOR, 0, (LPARAM)(COLORREF)color);
    SendMessageW(LVM_SETTEXTBKCOLOR, 0, (LPARAM)(COLORREF)color);
}

void sw::ListView::SetTextColor(Color color, bool redraw)
{
    Control::SetTextColor(color, false);
    SendMessageW(LVM_SETTEXTCOLOR, 0, (LPARAM)(COLORREF)color);
}

bool sw::ListView::OnNotify(NMHDR *pNMHDR, LRESULT &result)
{
    switch (pNMHDR->code) {
        case HDN_ITEMCLICKW: {
            OnHeaderItemClicked(reinterpret_cast<NMHEADERW *>(pNMHDR));
            break;
        }
        case HDN_ITEMDBLCLICKW: {
            OnHeaderItemDoubleClicked(reinterpret_cast<NMHEADERW *>(pNMHDR));
            break;
        }
    };
    return false;
}

bool sw::ListView::OnNotified(NMHDR *pNMHDR, LRESULT &result)
{
    switch (pNMHDR->code) {
        case LVN_ITEMCHANGED: {
            OnItemChanged(reinterpret_cast<NMLISTVIEW *>(pNMHDR));
            break;
        }
        case NM_CLICK: {
            OnItemClicked(reinterpret_cast<NMITEMACTIVATE *>(pNMHDR));
            break;
        }
        case NM_DBLCLK: {
            OnItemDoubleClicked(reinterpret_cast<NMITEMACTIVATE *>(pNMHDR));
            break;
        }
        case LVN_GETDISPINFOW: {
            OnGetDispInfo(reinterpret_cast<NMLVDISPINFOW *>(pNMHDR));
            return true;
        }
        case LVN_ENDLABELEDITW: {
            NMLVDISPINFOW *pNMInfo =
                reinterpret_cast<NMLVDISPINFOW *>(pNMHDR);

            if (pNMInfo->item.pszText == nullptr) {
                result = FALSE;
            } else {
                ListViewEndEditEventArgs args(pNMInfo->item.iItem, pNMInfo->item.pszText);
                OnEndEdit(args);
                result = !args.cancel;
                if (result) OnApplyEdit(args.index, args.newText);
            }
            return true;
        }
    }
    return Control::OnNotified(pNMHDR, result);
}

bool sw::ListView::OnPrePaint(NMCUSTOMDRAW *pNMCD, LRESULT &result)
{
    result = CDRF_NOTIFYITEMDRAW;
    return true;
}

bool sw::ListView::OnItemPrePaint(NMCUSTOMDRAW *pNMCD, bool subItem, LRESULT &result)
{
    if (!subItem) {
        result = CDRF_NOTIFYSUBITEMDRAW;
        return true;
    }

    if (!CheckBoxes) {
        return false;
    }

    auto *pCD =
        reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMCD);

    int row = static_cast<int>(pCD->nmcd.dwItemSpec);
    int col = pCD->iSubItem;

    if (col == 0) {
        RECT rtCheck{};
        GetCheckBoxRect(row, rtCheck);

        DrawFrameControl(
            pCD->nmcd.hdc,
            &rtCheck,
            DFC_BUTTON,
            DFCS_BUTTONCHECK | (GetItemCheckState(row) ? DFCS_CHECKED : 0));

        result = CDRF_DODEFAULT;
    }
    return true;
}

void sw::ListView::OnItemChanged(NMLISTVIEW *pNMLV)
{
    if (pNMLV->uChanged & LVIF_STATE) {
        auto changedState =
            pNMLV->uOldState ^ pNMLV->uNewState;

        if (changedState & LVIS_SELECTED) {
            OnSelectionChanged();
        }
    }
}

void sw::ListView::OnCheckStateChanged(int index)
{
    ListViewCheckStateChangedEventArgs args(index);
    RaiseRoutedEvent(args);
}

void sw::ListView::OnHeaderItemClicked(NMHEADERW *pNMH)
{
    ListViewHeaderClickedEventArgs args(pNMH->iItem);
    RaiseRoutedEvent(args);
}

void sw::ListView::OnHeaderItemDoubleClicked(NMHEADERW *pNMH)
{
    ListViewHeaderDoubleClickedEventArgs args(pNMH->iItem);
    RaiseRoutedEvent(args);
}

void sw::ListView::OnItemClicked(NMITEMACTIVATE *pNMIA)
{
    ListViewItemClickedEventArgs args(pNMIA->iItem, pNMIA->iSubItem);
    RaiseRoutedEvent(args);

    if (!args.handled && CheckBoxes) {
        POINT pt;
        GetCursorPos(&pt);
        ScreenToClient(Handle, &pt);

        RECT rtCheck{};
        GetCheckBoxRect(pNMIA->iItem, rtCheck);

        if (PtInRect(&rtCheck, pt)) {
            OnCheckBoxClicked(pNMIA->iItem);
        }
    }
}

void sw::ListView::OnItemDoubleClicked(NMITEMACTIVATE *pNMIA)
{
    ListViewItemDoubleClickedEventArgs args(pNMIA->iItem, pNMIA->iSubItem);
    RaiseRoutedEvent(args);
}

void sw::ListView::OnCheckBoxClicked(int index)
{
    SetItemCheckState(index, !GetItemCheckState(index));
}

void sw::ListView::OnEndEdit(ListViewEndEditEventArgs &args)
{
    RaiseRoutedEvent(args);
}

void sw::ListView::OnApplyEdit(int index, const std::wstring &newText)
{
    IList *source = GetCurrentItemsSource();

    if (source == nullptr || index < 0 || index >= source->Count()) {
        return;
    }

    Variant item = source->GetVariantAt(index);

    // Windows消息只支持编辑主项，因此只修改索引0处的文本

    if (item.IsType<ListViewItem>()) {
        // ListViewItem
        auto &lvItem = item.UnsafeCast<ListViewItem>();
        if (lvItem.subItems.Count() > 0) {
            lvItem.subItems[0] = newText;
        }
    } else if (item.IsType<std::wstring>()) {
        // std::wstring
        item.UnsafeCast<std::wstring>() = newText;
    } else if (item.IsType<List<std::wstring>>()) {
        // List<std::wstring>
        auto &subItems = item.UnsafeCast<List<std::wstring>>();
        if (subItems.Count() > 0) {
            subItems[0] = newText;
        }
    }
}

void sw::ListView::OnGetDispInfo(NMLVDISPINFOW *pNMInfo)
{
    IList *items = GetCurrentItemsSource();

    int index = static_cast<int>(pNMInfo->item.iItem);
    int count = items ? items->Count() : 0;

    if (index < 0 || index >= count) {
        return;
    }

    Variant item = items->GetVariantAt(index);
    GetDisplayInfo(index, item, pNMInfo);
}

void sw::ListView::GetDisplayInfo(int index, const Variant &item, NMLVDISPINFOW *pNMInfo)
{
    std::type_index itemType = item.GetType();

    if (itemType == typeid(ListViewItem)) {
        auto &lvItem = item.UnsafeCast<ListViewItem>();
        if (pNMInfo->item.mask & LVIF_TEXT) {
            int subIndex = pNMInfo->item.iSubItem;
            if (subIndex >= 0 && subIndex < lvItem.subItems.Count()) {
                pNMInfo->item.pszText = const_cast<LPWSTR>(lvItem.subItems[subIndex].c_str());
            }
        }
        if (pNMInfo->item.mask & LVIF_IMAGE) {
            pNMInfo->item.iImage = lvItem.imageIndex;
        }
    } else if (itemType == typeid(std::wstring)) {
        if (pNMInfo->item.mask & LVIF_TEXT && pNMInfo->item.iSubItem == 0) {
            pNMInfo->item.pszText = const_cast<LPWSTR>(item.UnsafeCast<std::wstring>().c_str());
        }
    } else if (itemType == typeid(List<std::wstring>)) {
        auto &subItems = item.UnsafeCast<List<std::wstring>>();
        if (pNMInfo->item.mask & LVIF_TEXT) {
            int subIndex = pNMInfo->item.iSubItem;
            if (subIndex >= 0 && subIndex < subItems.Count()) {
                pNMInfo->item.pszText = const_cast<LPWSTR>(subItems[subIndex].c_str());
            }
        }
    }
}

void sw::ListView::GetCheckBoxRect(int index, RECT &rect)
{
    ListView_GetItemRect(Handle, index, &rect, LVIR_ICON);

    int rowHeight   = rect.bottom - rect.top;
    int cxMenuCheck = GetSystemMetrics(SM_CXMENUCHECK);
    int cyMenuCheck = GetSystemMetrics(SM_CYMENUCHECK);

    rect.top += (rowHeight - cyMenuCheck) / 2;
    rect.left -= cxMenuCheck;
    rect.right  = rect.left + cxMenuCheck;
    rect.bottom = rect.top + cyMenuCheck;
}

void sw::ListView::OnGetItemCheckState(int index, bool &checked)
{
    if (GetCurrentItemsSource() != GetDefaultItemsSource()) {
        return;
    }

    if (index < 0 || index >= _items.Count()) {
        checked = false;
    } else {
        checked = _items.GetAt(index).checked;
    }
}

void sw::ListView::OnSetItemCheckState(int index, bool checked)
{
    if (GetCurrentItemsSource() != GetDefaultItemsSource()) {
        return;
    }
    if (index < 0 || index >= _items.Count()) {
        return;
    }

    auto &item = _items.GetAt(index);

    if (item.checked != checked) {
        item.checked = checked;
        Redraw();
        OnCheckStateChanged(index);
    }
}

void sw::ListView::_SetCount(int count)
{
    SendMessageW(LVM_SETITEMCOUNT, count, 0);
}

void sw::ListView::_UpdateCount()
{
    int selectedIndex = GetSelectedIndex();

    IList *currentItemsSource = GetCurrentItemsSource();
    _SetCount(currentItemsSource ? currentItemsSource->Count() : 0);

    RaisePropertyChanged(&ItemsControl::ItemsCount);
    SetSelectedIndex(selectedIndex); // 尝试恢复选中项，若原选中项索引超出范围则会被重置为-1
}

int sw::ListView::_GetRowCount()
{
    return (int)SendMessageW(LVM_GETITEMCOUNT, 0, 0);
}

int sw::ListView::_GetColCount()
{
    HWND hHeader = (HWND)SendMessageW(LVM_GETHEADER, 0, 0);
    return (int)::SendMessageW(hHeader, HDM_GETITEMCOUNT, 0, 0);
}

DWORD sw::ListView::_GetExtendedListViewStyle()
{
    return (DWORD)SendMessageW(LVM_GETEXTENDEDLISTVIEWSTYLE, 0, 0);
}

DWORD sw::ListView::_SetExtendedListViewStyle(DWORD style)
{
    return (DWORD)SendMessageW(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, (LPARAM)style);
}
