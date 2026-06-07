#include "MenuItem.h"
#include <atomic>

sw::MenuItemDesc::MenuItemDesc(const wchar_t *text)
    : text(text)
{
}

sw::MenuItemDesc::MenuItemDesc(const std::wstring &text)
    : text(text)
{
}

sw::MenuItemDesc::MenuItemDesc(const std::wstring &text,
                               std::initializer_list<MenuItemDesc> subItems)
    : text(text), subItems(subItems)
{
}

sw::MenuItemDesc::MenuItemDesc(uint64_t tag, const std::wstring &text)
    : tag(tag), text(text)
{
}

sw::MenuItemDesc::MenuItemDesc(uint64_t tag, const std::wstring &text,
                               std::initializer_list<MenuItemDesc> subItems)
    : tag(tag), text(text), subItems(subItems)
{
}

sw::MenuItem::MenuItem(const MenuItemDesc &desc)
    : _id(_GenerateMenuItemID()),

      Id(
          Property<int>::Init(this)
              .Getter([](MenuItem *self) -> int {
                  return self->_id;
              })),

      Handle(
          Property<HMENU>::Init(this)
              .Getter([](MenuItem *self) -> HMENU {
                  return self->_hMenu;
              })),

      Text(
          Property<std::wstring>::Init(this)
              .Getter([](MenuItem *self) -> std::wstring {
                  return self->_desc.text;
              })
              .Setter([](MenuItem *self, const std::wstring &value) {
                  if (self->_desc.text != value) {
                      bool isSeparator = self->IsSeparator;
                      self->_desc.text = value;
                      if (self->IsSeparator == isSeparator) {
                          self->_UpdateState();
                          self->RaisePropertyChanged(&MenuItem::Text);
                      } else {
                          self->_ResetMenuItem();
                          self->RaisePropertyChanged(&MenuItem::Text);
                          self->RaisePropertyChanged(&MenuItem::IsSeparator);
                      }
                  }
              })),

      Enabled(
          Property<bool>::Init(this)
              .Getter([](MenuItem *self) -> bool {
                  return self->_desc.enabled;
              })
              .Setter([](MenuItem *self, bool value) {
                  if (self->_desc.enabled != value) {
                      self->_desc.enabled = value;
                      self->_UpdateState();
                      self->RaisePropertyChanged(&MenuItem::Enabled);
                  }
              })),

      IsChecked(
          Property<bool>::Init(this)
              .Getter([](MenuItem *self) -> bool {
                  return self->_desc.checked;
              })
              .Setter([](MenuItem *self, bool value) {
                  if (self->_desc.checked != value) {
                      self->_desc.checked = value;
                      self->_UpdateState();
                      self->RaisePropertyChanged(&MenuItem::IsChecked);
                  }
              })),

      Bitmap(
          Property<HBITMAP>::Init(this)
              .Getter([](MenuItem *self) -> HBITMAP {
                  return self->_desc.hBitmap;
              })
              .Setter([](MenuItem *self, HBITMAP value) {
                  if (self->_desc.hBitmap != value) {
                      self->_desc.hBitmap = value;
                      self->_UpdateState();
                      self->RaisePropertyChanged(&MenuItem::Bitmap);
                  }
              })),

      IsSeparator(
          ReadOnlyProperty<bool>::Init(this)
              .Getter([](MenuItem *self) -> bool {
                  return self->_subItems.empty() &&
                         self->_desc.text == L"-";
              })),

      Tag(
          Property<uint64_t>::Init(this)
              .Getter([](MenuItem *self) -> uint64_t {
                  return self->_desc.tag;
              })
              .Setter([](MenuItem *self, uint64_t value) {
                  self->SetTag(value);
              }))
{
    SetTag(desc.tag);
    _desc.text    = desc.text;
    _desc.enabled = desc.enabled;
    _desc.checked = desc.checked;
    _desc.hBitmap = desc.hBitmap;
}

sw::MenuItem::~MenuItem()
{
    if (_isRoot && _hMenu != NULL) {
        DestroyMenu(_hMenu);
        _hMenu = NULL;
    }
}

sw::MenuItem *sw::MenuItem::Create(const MenuItemDesc &desc)
{
    struct _CreateMenuItemStackItem {
        MenuItem *parent;
        const MenuItemDesc *desc;
    };

    std::vector<_CreateMenuItemStackItem> stack;
    stack.push_back({/*parent*/ nullptr, /*desc*/ &desc});

    MenuItem *root = nullptr;

    while (!stack.empty()) {
        auto parent = stack.back().parent;
        auto desc   = stack.back().desc;
        stack.pop_back();

        auto item = new MenuItem(*desc);
        if (root == nullptr) root = item;

        if (parent == nullptr) {
            item->_parent = nullptr;
        } else {
            _SetParent(parent, item);
        }

        for (int i = desc->subItems.Count() - 1; i >= 0; --i) {
            stack.push_back({/*parent*/ item, /*desc*/ &desc->subItems[i]});
        }
    }
    return root;
}

sw::MenuItem *sw::MenuItem::CreateRoot(bool isPopup)
{
    MenuItem *root = new MenuItem({});
    root->_isRoot  = true;

    if (!isPopup) {
        root->_hMenu = CreateMenu();
    } else {
        root->_hMenu = CreatePopupMenu();
    }
    return root;
}

void sw::MenuItem::OnTagChanged()
{
    TBase::OnTagChanged();

    Variant tagVariant   = GetTag();
    uint64_t newTagValue = 0;

    if (tagVariant.IsType<uint64_t>()) {
        newTagValue = tagVariant.UnsafeCast<uint64_t>();
    }
    if (_desc.tag != newTagValue) {
        _desc.tag = newTagValue;
        RaisePropertyChanged(&MenuItem::Tag);
    }
}

sw::MenuItem *sw::MenuItem::GetParent() const
{
    return _parent;
}

int sw::MenuItem::GetChildCount() const
{
    return static_cast<int>(_subItems.size());
}

sw::MenuItem &sw::MenuItem::GetChildAt(int index) const
{
    return *_subItems.at(index);
}

sw::MenuItem *sw::MenuItem::AddChild(const MenuItemDesc &desc)
{
    auto child = Create(desc);
    _SetParent(this, child);

    if (_hMenu == NULL) {
        _ResetMenuItem();
    } else {
        child->_ResetMenuItem();
    }

    if (child->CurrentDataContext != nullptr) {
        child->OnCurrentDataContextChanged(nullptr);
    }
    return child;
}

sw::MenuItem *sw::MenuItem::InsertChild(int index, const MenuItemDesc &desc)
{
    if (index < 0 || index > static_cast<int>(_subItems.size())) {
        throw std::out_of_range("Index out of range in InsertChild");
    }

    auto child = Create(desc);
    _SetParent(this, child, index);

    if (_hMenu == NULL) {
        _ResetMenuItem();
    } else {
        InsertMenuW(_hMenu, index, MF_STRING | MF_BYPOSITION, static_cast<UINT_PTR>(child->_id), L"");
        child->_ResetMenuItem();
    }

    if (child->CurrentDataContext != nullptr) {
        child->OnCurrentDataContextChanged(nullptr);
    }
    return child;
}

bool sw::MenuItem::RemoveChildAt(int index)
{
    if (index < 0 || index >= static_cast<int>(_subItems.size())) {
        return false;
    }

    DeleteMenu(_hMenu, index, MF_BYPOSITION);
    _subItems.erase(_subItems.begin() + index);

    if (_subItems.empty() && !_isRoot) {
        _ResetMenuItem();
    }
    return true;
}

bool sw::MenuItem::RemoveChild(MenuItem *child)
{
    int index = IndexOf(child);
    return RemoveChildAt(index);
}

void sw::MenuItem::ClearChildren()
{
    if (_hMenu == NULL && _subItems.empty()) {
        return;
    }
    if (_hMenu != NULL) {
        for (int i = GetMenuItemCount(_hMenu) - 1; i >= 0; --i) {
            DeleteMenu(_hMenu, i, MF_BYPOSITION);
        }
    }
    _subItems.clear();
    _ResetMenuItem();
}

int sw::MenuItem::IndexOf(MenuItem *child) const
{
    for (auto &p : _subItems) {
        if (p.get() == child) {
            return static_cast<int>(&p - &_subItems[0]);
        }
    }
    return -1;
}

void sw::MenuItem::ResetChildren(std::initializer_list<MenuItemDesc> descs)
{
    if (descs.size() == 0) {
        ClearChildren();
        return;
    }

    _subItems.clear();

    for (auto &desc : descs) {
        auto child = Create(desc);
        _SetParent(this, child);
    }

    bool resetMenu   = true;
    auto dataContext = CurrentDataContext.Get();

    if (_hMenu == NULL) {
        resetMenu = false;
        _ResetMenuItem();
    } else {
        for (int i = GetMenuItemCount(_hMenu) - 1; i >= 0; --i) {
            DeleteMenu(_hMenu, i, MF_BYPOSITION);
        }
    }

    for (auto &child : _subItems) {
        if (resetMenu) {
            child->_ResetMenuItem();
        }
        if (dataContext != nullptr) {
            child->OnCurrentDataContextChanged(nullptr);
        }
    }
}

sw::MenuItem *sw::MenuItem::FindChildById(int id)
{
    std::vector<MenuItem *> stack;
    stack.push_back(this);

    while (!stack.empty()) {
        MenuItem *current = stack.back();
        stack.pop_back();

        if (current->_id == id) {
            return current;
        }
        for (auto &child : current->_subItems) {
            stack.push_back(child.get());
        }
    }
    return nullptr;
}

sw::MenuItem *sw::MenuItem::FindChildByTag(uint64_t tag)
{
    std::vector<MenuItem *> stack;
    stack.push_back(this);

    while (!stack.empty()) {
        MenuItem *current = stack.back();
        stack.pop_back();

        if (current->_desc.tag == tag) {
            return current;
        }
        for (auto &child : current->_subItems) {
            stack.push_back(child.get());
        }
    }
    return nullptr;
}

void sw::MenuItem::_ResetMenuItem()
{
    struct _UpdateMenuItemStackItem {
        sw::MenuItem *parent;
        int index;
        sw::MenuItem *item;
    };

    HMENU hOldMenu = NULL;

    if (!_isRoot) {
        hOldMenu = _hMenu;
        _hMenu   = NULL;
    }

    std::vector<_UpdateMenuItemStackItem> stack;

    for (int i = static_cast<int>(_subItems.size()) - 1; i >= 0; --i) {
        stack.push_back({/*parent*/ this, /*index*/ i, /*item*/ _subItems[i].get()});
    }

    if (_hMenu == NULL && !stack.empty()) {
        _hMenu = CreatePopupMenu();
    }

    while (!stack.empty()) {
        auto parent = stack.back().parent;
        auto index  = stack.back().index;
        auto item   = stack.back().item;
        stack.pop_back();

        HMENU hParentMenu = parent->_hMenu;

        if (item->_subItems.empty()) {
            item->_hMenu = NULL;
            if (item->IsSeparator) {
                AppendMenuW(hParentMenu, MF_SEPARATOR, 0, NULL);
            } else {
                AppendMenuW(hParentMenu, MF_STRING, static_cast<UINT_PTR>(item->_id), L"");
                _UpdateMenuItem(hParentMenu, index, item->_desc);
            }
        } else {
            item->_hMenu = CreatePopupMenu();
            AppendMenuW(hParentMenu, MF_POPUP, reinterpret_cast<UINT_PTR>(item->_hMenu), L"");
            _UpdateMenuItem(hParentMenu, index, item->_desc);
        }

        for (int i = static_cast<int>(item->_subItems.size()) - 1; i >= 0; --i) {
            stack.push_back({/*parent*/ item, /*index*/ i, /*item*/ item->_subItems[i].get()});
        }
    }

    if (_parent == nullptr) {
        if (hOldMenu != NULL)
            DestroyMenu(hOldMenu);
        return;
    }

    int index = _parent->IndexOf(this);
    RemoveMenu(_parent->_hMenu, index, MF_BYPOSITION);

    if (hOldMenu != NULL) {
        DestroyMenu(hOldMenu);
    }

    if (IsSeparator) {
        InsertMenuW(_parent->_hMenu, index, MF_SEPARATOR | MF_BYPOSITION, 0, NULL);
    } else {
        if (_hMenu != NULL) {
            InsertMenuW(_parent->_hMenu, index, MF_POPUP | MF_BYPOSITION, reinterpret_cast<UINT_PTR>(_hMenu), L"");
        } else {
            InsertMenuW(_parent->_hMenu, index, MF_STRING | MF_BYPOSITION, static_cast<UINT_PTR>(_id), L"");
        }
        _UpdateMenuItem(_parent->_hMenu, index, _desc);
    }
}

void sw::MenuItem::_UpdateState()
{
    if (_parent != nullptr) {
        int index = _parent->IndexOf(this);
        _UpdateMenuItem(_parent->_hMenu, index, _desc);
    }
}

void sw::MenuItem::_SetParent(MenuItem *parent, MenuItem *child, int index)
{
    child->_parent = parent;

    if (index == -1) {
        parent->_subItems.emplace_back(child);
    } else {
        parent->_subItems.insert(parent->_subItems.begin() + index, std::unique_ptr<MenuItem>(child));
    }
}

int sw::MenuItem::_GenerateMenuItemID()
{
    static std::atomic<int> idCounter{1};
    return idCounter.fetch_add(1);
}

void sw::MenuItem::_ApplyMenuDesc(const MenuItemDesc &desc, MENUITEMINFOW *pMii)
{
    pMii->cbSize     = sizeof(MENUITEMINFOW);
    pMii->fMask      = MIIM_STRING | MIIM_STATE | MIIM_BITMAP;
    pMii->hbmpItem   = desc.hBitmap;
    pMii->dwTypeData = const_cast<LPWSTR>(desc.text.c_str());

    if (desc.checked) {
        pMii->fState |= MFS_CHECKED;
    }
    if (!desc.enabled) {
        pMii->fState |= MFS_DISABLED;
    }
}

void sw::MenuItem::_UpdateMenuItem(HMENU hParentMenu, int index, const MenuItemDesc &desc)
{
    MENUITEMINFOW mii{};
    _ApplyMenuDesc(desc, &mii);
    SetMenuItemInfoW(hParentMenu, index, TRUE, &mii);
}
