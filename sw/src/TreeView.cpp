#include "TreeView.h"

sw::TreeViewNode::TreeViewNode(HWND hwnd, HTREEITEM hitem)
    : _hwnd(hwnd), _hitem(hitem)
{
}

HWND sw::TreeViewNode::GetOwnerHandle() const
{
    return _hwnd;
}

HTREEITEM sw::TreeViewNode::GetHandle() const
{
    return _hitem;
}

std::wstring sw::TreeViewNode::ToString() const
{
    return GetText();
}

bool sw::TreeViewNode::Equals(const TreeViewNode &other) const
{
    return _hwnd == other._hwnd && _hitem == other._hitem;
}

bool sw::TreeViewNode::IsNull() const
{
    return _hwnd == NULL || _hitem == NULL;
}

sw::TreeViewNode::operator bool() const
{
    return !IsNull();
}

std::wstring sw::TreeViewNode::GetText() const
{
    std::wstring text;
    text.resize(256);

    TVITEMW tvi{};
    tvi.mask  = TVIF_TEXT;
    tvi.hItem = _hitem;

    while (true) {
        tvi.pszText    = &text[0];
        tvi.cchTextMax = static_cast<int>(text.size());

        if (::SendMessageW(_hwnd, TVM_GETITEMW, 0, reinterpret_cast<LPARAM>(&tvi)) == FALSE) {
            text.clear();
            break;
        }

        size_t len = wcslen(tvi.pszText);

        if (len < text.size() - 1) {
            text.resize(len);
            break;
        } else {
            text.resize(text.size() * 2);
        }
    }
    return text;
}

bool sw::TreeViewNode::SetText(const std::wstring &text)
{
    TVITEMW tvi{};
    tvi.mask    = TVIF_TEXT;
    tvi.hItem   = _hitem;
    tvi.pszText = const_cast<LPWSTR>(text.c_str());

    return ::SendMessageW(_hwnd, TVM_SETITEMW, 0, reinterpret_cast<LPARAM>(&tvi)) != FALSE;
}

sw::TreeViewNode sw::TreeViewNode::GetParent() const
{
    return TreeViewNode{_hwnd, TreeView_GetParent(_hwnd, _hitem)};
}

sw::TreeViewNode sw::TreeViewNode::GetNextNode() const
{
    return TreeViewNode{_hwnd, TreeView_GetNextSibling(_hwnd, _hitem)};
}

sw::TreeViewNode sw::TreeViewNode::GetPreviousNode() const
{
    return TreeViewNode{_hwnd, TreeView_GetPrevSibling(_hwnd, _hitem)};
}

sw::TreeViewNode sw::TreeViewNode::GetFirstChildNode() const
{
    return TreeViewNode{_hwnd, TreeView_GetChild(_hwnd, _hitem)};
}

sw::TreeViewNode sw::TreeViewNode::InsertAfter(const std::wstring &text)
{
    TVINSERTSTRUCTW tvis{};
    tvis.hParent      = TreeView_GetParent(_hwnd, _hitem);
    tvis.hInsertAfter = _hitem;
    tvis.item.mask    = TVIF_TEXT;
    tvis.item.pszText = const_cast<LPWSTR>(text.c_str());

    auto hitem = (HTREEITEM)SendMessageW(_hwnd, TVM_INSERTITEMW, 0, reinterpret_cast<LPARAM>(&tvis));
    return TreeViewNode{_hwnd, hitem};
}

sw::TreeViewNode sw::TreeViewNode::AddChild(const std::wstring &text)
{
    TVINSERTSTRUCTW tvis{};
    tvis.hParent      = _hitem;
    tvis.hInsertAfter = TVI_LAST;
    tvis.item.mask    = TVIF_TEXT;
    tvis.item.pszText = const_cast<LPWSTR>(text.c_str());

    auto hitem = (HTREEITEM)SendMessageW(_hwnd, TVM_INSERTITEMW, 0, reinterpret_cast<LPARAM>(&tvis));
    return TreeViewNode{_hwnd, hitem};
}

bool sw::TreeViewNode::IsSelected() const
{
    return (TreeView_GetItemState(_hwnd, _hitem, TVIS_SELECTED) & TVIS_SELECTED) != 0;
}

bool sw::TreeViewNode::Select()
{
    return TreeView_SelectItem(_hwnd, _hitem);
}

bool sw::TreeViewNode::Delete()
{
    return TreeView_DeleteItem(_hwnd, _hitem);
}

bool sw::TreeViewNode::IsExpanded() const
{
    return (TreeView_GetItemState(_hwnd, _hitem, TVIS_EXPANDED) & TVIS_EXPANDED) != 0;
}

bool sw::TreeViewNode::SetExpand(bool expand)
{
    UINT action = expand ? TVE_EXPAND : TVE_COLLAPSE;
    return TreeView_Expand(_hwnd, _hitem, action);
}

bool sw::TreeViewNode::Expand()
{
    return SetExpand(true);
}

bool sw::TreeViewNode::Collapse()
{
    return SetExpand(false);
}

void *sw::TreeViewNode::GetUserData() const
{
    TVITEM tvi{};
    tvi.mask  = TVIF_PARAM;
    tvi.hItem = _hitem;

    if (TreeView_GetItem(_hwnd, &tvi) == FALSE) {
        return nullptr;
    } else {
        return reinterpret_cast<void *>(tvi.lParam);
    }
}

bool sw::TreeViewNode::SetUserData(void *data)
{
    TVITEM tvi{};
    tvi.mask   = TVIF_PARAM;
    tvi.hItem  = _hitem;
    tvi.lParam = reinterpret_cast<LPARAM>(data);

    return TreeView_SetItem(_hwnd, &tvi) != FALSE;
}

bool sw::TreeViewNode::IsChecked() const
{
    return TreeView_GetCheckState(_hwnd, _hitem) == 1;
}

void sw::TreeViewNode::SetCheck(bool check)
{
    TreeView_SetCheckState(_hwnd, _hitem, check);
}

bool sw::TreeViewNode::SetImages(int imageIndex, int selectedImageIndex)
{
    TVITEM tvi{};
    tvi.mask           = TVIF_IMAGE | TVIF_SELECTEDIMAGE;
    tvi.hItem          = _hitem;
    tvi.iImage         = imageIndex;
    tvi.iSelectedImage = selectedImageIndex;

    return TreeView_SetItem(_hwnd, &tvi) != FALSE;
}

int sw::TreeViewNode::GetChildCount() const
{
    int count  = 0;
    auto child = GetFirstChildNode();

    while (!child.IsNull()) {
        ++count;
        child = child.GetNextNode();
    }
    return count;
}

int sw::TreeViewNode::DeleteAllChildren()
{
    int count  = 0;
    auto child = GetFirstChildNode();

    while (!child.IsNull()) //
    {
        auto next = child.GetNextNode();

        if (!child.Delete()) {
            break;
        } else {
            ++count;
            child = next;
        }
    }
    return count;
}

sw::TreeView::TreeView()
    : Root(
          Property<TreeViewNode>::Init(this)
              .Getter<&TreeView::_GetRoot>()),

      AllItemsCount(
          Property<int>::Init(this)
              .Getter([](TreeView *self) -> int {
                  HWND hwnd = self->Handle;
                  return TreeView_GetCount(hwnd);
              })),

      CheckBoxes(
          Property<bool>::Init(this)
              .Getter([](TreeView *self) -> bool {
                  return self->GetStyle(TVS_CHECKBOXES);
              })
              .Setter([](TreeView *self, bool value) {
                  self->SetStyle(TVS_CHECKBOXES, value);
              })),

      LineColor(
          Property<Color>::Init(this)
              .Getter([](TreeView *self) -> Color {
                  HWND hwnd = self->Handle;
                  return static_cast<Color>(TreeView_GetLineColor(hwnd));
              })
              .Setter([](TreeView *self, const Color &value) {
                  HWND hwnd = self->Handle;
                  TreeView_SetLineColor(hwnd, static_cast<COLORREF>(value));
              })),

      IndentWidth(
          Property<double>::Init(this)
              .Getter([](TreeView *self) -> double {
                  HWND hwnd = self->Handle;
                  return Dip::PxToDipX(TreeView_GetIndent(hwnd));
              })
              .Setter([](TreeView *self, double value) {
                  HWND hwnd = self->Handle;
                  TreeView_SetIndent(hwnd, Dip::DipToPxX(value));
              }))
{
    InitControl(WC_TREEVIEWW, NULL, WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | WS_BORDER | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS, 0);

    Rect    = {0, 0, 200, 200};
    TabStop = true;
}

int sw::TreeView::GetItemsCount()
{
    int cnt = 0;

    TreeViewNode node = _GetRoot();
    while (!node.IsNull()) {
        cnt++;
        node = node.GetNextNode();
    }
    return cnt;
}

int sw::TreeView::GetSelectedIndex()
{
    int index = 0;

    TreeViewNode node = _GetRoot();
    while (!node.IsNull()) {
        if (node.IsSelected()) {
            return index;
        } else {
            ++index;
            node = node.GetNextNode();
        }
    }
    return -1;
}

void sw::TreeView::SetSelectedIndex(int index)
{
    TreeViewNode node = GetItemAt(index);
    if (!node.IsNull()) node.Select();
}

sw::TreeViewNode sw::TreeView::GetSelectedItem()
{
    HWND hwnd = Handle;
    return TreeViewNode{hwnd, TreeView_GetSelection(hwnd)};
}

void sw::TreeView::SetBackColor(Color color, bool redraw)
{
    Control::SetBackColor(color, false);

    HWND hwnd = Handle;
    TreeView_SetBkColor(hwnd, static_cast<COLORREF>(color));
}

void sw::TreeView::SetTextColor(Color color, bool redraw)
{
    Control::SetTextColor(color, false);

    HWND hwnd = Handle;
    TreeView_SetTextColor(hwnd, static_cast<COLORREF>(color));
}

bool sw::TreeView::OnNotified(NMHDR *pNMHDR, LRESULT &result)
{
    switch (pNMHDR->code) {
        case TVN_SELCHANGEDW: {
            OnSelectionChanged();
            break;
        }
        case NM_CLICK:
        case NM_RCLICK: {
            return OnClicked(pNMHDR, result);
        }
        case NM_DBLCLK:
        case NM_RDBLCLK: {
            return OnDoubleClicked(pNMHDR, result);
        }
        case TVN_GETDISPINFOW: {
            OnGetDispInfo(reinterpret_cast<NMTVDISPINFOW *>(pNMHDR));
            return true;
        }
        case TVN_ITEMEXPANDINGW: {
            result = OnItemExpanding(reinterpret_cast<NMTREEVIEWW *>(pNMHDR)) ? TRUE : FALSE;
            return true;
        }
        case TVN_ITEMEXPANDEDW: {
            OnItemExpanded(reinterpret_cast<NMTREEVIEWW *>(pNMHDR));
            return true;
        }
        case TVN_ITEMCHANGEDW: {
            OnItemChanged(reinterpret_cast<NMTVITEMCHANGE *>(pNMHDR));
            return true;
        }
    }
    return TBase::OnNotified(pNMHDR, result);
}

bool sw::TreeView::OnClicked(NMHDR *pNMHDR, LRESULT &result)
{
    return false;
}

bool sw::TreeView::OnDoubleClicked(NMHDR *pNMHDR, LRESULT &result)
{
    return false;
}

void sw::TreeView::OnGetDispInfo(NMTVDISPINFOW *pNMInfo)
{
}

bool sw::TreeView::OnItemExpanding(NMTREEVIEWW *pNMTV)
{
    TreeViewNode node{Handle, pNMTV->itemNew.hItem};
    TreeViewItemExpandingEventArgs args{pNMTV->action == TVE_EXPAND, node};
    RaiseRoutedEvent(args);
    return args.cancel;
}

void sw::TreeView::OnItemExpanded(NMTREEVIEWW *pNMTV)
{
    TreeViewNode node{Handle, pNMTV->itemNew.hItem};
    TreeViewItemExpandedEventArgs args{pNMTV->action == TVE_EXPAND, node};
    RaiseRoutedEvent(args);
}

void sw::TreeView::OnItemChanged(NMTVITEMCHANGE *pNMInfo)
{
    if ((pNMInfo->uChanged & TVIF_STATE) &&
        ((pNMInfo->uStateNew ^ pNMInfo->uStateOld) & TVIS_STATEIMAGEMASK)) //
    {
        int oldCheck = ((pNMInfo->uStateOld & TVIS_STATEIMAGEMASK) >> 12) - 1;
        int newCheck = ((pNMInfo->uStateNew & TVIS_STATEIMAGEMASK) >> 12) - 1;

        if (newCheck != oldCheck) {
            TreeViewNode node{Handle, pNMInfo->hItem};
            TreeViewCheckStateChangedEventArgs args{newCheck, node};
            RaiseRoutedEvent(args);
        }
    }
}

void sw::TreeView::Clear()
{
    HWND hwnd = Handle;
    TreeView_DeleteAllItems(hwnd);
}

sw::TreeViewNode sw::TreeView::GetItemAt(int index)
{
    if (index < 0) {
        return TreeViewNode{};
    }

    int curIndex = 0;

    TreeViewNode node = _GetRoot();
    while (!node.IsNull()) {
        if (curIndex == index) {
            return node;
        } else {
            ++curIndex;
            node = node.GetNextNode();
        }
    }
    return TreeViewNode{};
}

bool sw::TreeView::AddItem(const TreeViewNode &item)
{
    return false;
}

bool sw::TreeView::InsertItem(int index, const TreeViewNode &item)
{
    return false;
}

bool sw::TreeView::UpdateItem(int index, const TreeViewNode &newValue)
{
    return false;
}

bool sw::TreeView::RemoveItemAt(int index)
{
    TreeViewNode node = GetItemAt(index);
    return node.IsNull() ? false : node.Delete();
}

sw::TreeViewNode sw::TreeView::AddItem(const std::wstring &text)
{
    return _InsertItem(TVI_ROOT, TVI_LAST, text);
}

sw::TreeViewNode sw::TreeView::InsertItem(int index, const std::wstring &text)
{
    if (index <= 0) {
        return _InsertItem(TVI_ROOT, TVI_FIRST, text);
    }
    TreeViewNode prevNode = GetItemAt(index - 1);
    return prevNode.IsNull() ? TreeViewNode{} : _InsertItem(TVI_ROOT, prevNode.GetHandle(), text);
}

sw::ImageList sw::TreeView::GetImageList(TreeViewImageList imageList)
{
    HWND hwnd = Handle;
    return ImageList::Wrap(TreeView_GetImageList(hwnd, static_cast<int>(imageList)));
}

HIMAGELIST sw::TreeView::SetImageList(TreeViewImageList imageList, HIMAGELIST value)
{
    HWND hwnd = Handle;
    return TreeView_SetImageList(hwnd, value, static_cast<int>(imageList));
}

sw::TreeViewNode sw::TreeView::_GetRoot()
{
    HWND hwnd = Handle;
    return TreeViewNode{hwnd, TreeView_GetRoot(hwnd)};
}

sw::TreeViewNode sw::TreeView::_InsertItem(HTREEITEM hParent, HTREEITEM hInsertAfter, const std::wstring &text)
{
    HWND hwnd = Handle;

    TVINSERTSTRUCTW tvis{};
    tvis.hParent      = hParent;
    tvis.hInsertAfter = hInsertAfter;
    tvis.item.mask    = TVIF_TEXT;
    tvis.item.pszText = const_cast<LPWSTR>(text.c_str());

    auto hitem = (HTREEITEM)SendMessageW(TVM_INSERTITEMW, 0, reinterpret_cast<LPARAM>(&tvis));
    return TreeViewNode{hwnd, hitem};
}
