#include "UIElement.h"
#include "DataBinding.h"
#include "Utils.h"
#include <algorithm>
#include <cmath>
#include <deque>

namespace
{
    /**
     * @brief 属性ID
     */
    const sw::FieldId _PropId_Margin              = sw::Reflection::GetFieldId(&sw::UIElement::Margin);
    const sw::FieldId _PropId_HorizontalAlignment = sw::Reflection::GetFieldId(&sw::UIElement::HorizontalAlignment);
    const sw::FieldId _PropId_VerticalAlignment   = sw::Reflection::GetFieldId(&sw::UIElement::VerticalAlignment);
    const sw::FieldId _PropId_ChildCount          = sw::Reflection::GetFieldId(&sw::UIElement::ChildCount);
    const sw::FieldId _PropId_CollapseWhenHide    = sw::Reflection::GetFieldId(&sw::UIElement::CollapseWhenHide);
    const sw::FieldId _PropId_UIElementParent     = sw::Reflection::GetFieldId(&sw::UIElement::Parent);
    const sw::FieldId _PropId_Tag                 = sw::Reflection::GetFieldId(&sw::UIElement::Tag);
    const sw::FieldId _PropId_LayoutTag           = sw::Reflection::GetFieldId(&sw::UIElement::LayoutTag);
    const sw::FieldId _PropId_ContextMenu         = sw::Reflection::GetFieldId(&sw::UIElement::ContextMenu);
    const sw::FieldId _PropId_Float               = sw::Reflection::GetFieldId(&sw::UIElement::Float);
    const sw::FieldId _PropId_TabStop             = sw::Reflection::GetFieldId(&sw::UIElement::TabStop);
    const sw::FieldId _PropId_BackColor           = sw::Reflection::GetFieldId(&sw::UIElement::BackColor);
    const sw::FieldId _PropId_TextColor           = sw::Reflection::GetFieldId(&sw::UIElement::TextColor);
    const sw::FieldId _PropId_Transparent         = sw::Reflection::GetFieldId(&sw::UIElement::Transparent);
    const sw::FieldId _PropId_InheritTextColor    = sw::Reflection::GetFieldId(&sw::UIElement::InheritTextColor);
    const sw::FieldId _PropId_MinWidth            = sw::Reflection::GetFieldId(&sw::UIElement::MinWidth);
    const sw::FieldId _PropId_MinHeight           = sw::Reflection::GetFieldId(&sw::UIElement::MinHeight);
    const sw::FieldId _PropId_MaxWidth            = sw::Reflection::GetFieldId(&sw::UIElement::MaxWidth);
    const sw::FieldId _PropId_MaxHeight           = sw::Reflection::GetFieldId(&sw::UIElement::MaxHeight);
    const sw::FieldId _PropId_LogicalRect         = sw::Reflection::GetFieldId(&sw::UIElement::LogicalRect);
    const sw::FieldId _PropId_IsHitTestVisible    = sw::Reflection::GetFieldId(&sw::UIElement::IsHitTestVisible);
    const sw::FieldId _PropId_DataContext         = sw::Reflection::GetFieldId(&sw::UIElement::DataContext);
    const sw::FieldId _PropId_CurrentDataContext  = sw::Reflection::GetFieldId(&sw::UIElement::CurrentDataContext);
}

sw::UIElement::UIElement()
    : Margin(
          Property<Thickness>::Init(this)
              .Getter([](UIElement *self) -> Thickness {
                  return self->_margin;
              })
              .Setter([](UIElement *self, const Thickness &value) {
                  if (self->_margin != value) {
                      self->_margin = value;
                      self->RaisePropertyChanged(_PropId_Margin);
                      self->InvalidateMeasure();
                  }
              })),

      HorizontalAlignment(
          Property<sw::HorizontalAlignment>::Init(this)
              .Getter([](UIElement *self) -> sw::HorizontalAlignment {
                  return self->_horizontalAlignment;
              })
              .Setter([](UIElement *self, sw::HorizontalAlignment value) {
                  if (self->_SetHorzAlignment(value)) {
                      self->InvalidateMeasure();
                  }
              })),

      VerticalAlignment(
          Property<sw::VerticalAlignment>::Init(this)
              .Getter([](UIElement *self) -> sw::VerticalAlignment {
                  return self->_verticalAlignment;
              })
              .Setter([](UIElement *self, sw::VerticalAlignment value) {
                  if (self->_SetVertAlignment(value)) {
                      self->InvalidateMeasure();
                  }
              })),

      ChildCount(
          Property<int>::Init(this)
              .Getter([](UIElement *self) -> int {
                  return (int)self->_children.size();
              })),

      CollapseWhenHide(
          Property<bool>::Init(this)
              .Getter([](UIElement *self) -> bool {
                  return self->_collapseWhenHide;
              })
              .Setter([](UIElement *self, bool value) {
                  if (self->_collapseWhenHide != value) {
                      self->_collapseWhenHide = value;
                      self->RaisePropertyChanged(_PropId_CollapseWhenHide);
                      if (self->_parent && !self->Visible) {
                          self->_parent->_UpdateLayoutVisibleChildren();
                          self->_parent->InvalidateMeasure();
                      }
                  }
              })),

      Parent(
          Property<UIElement *>::Init(this)
              .Getter([](UIElement *self) -> UIElement * {
                  return self->_parent;
              })),

      Tag(
          Property<uint64_t>::Init(this)
              .Getter([](UIElement *self) -> uint64_t {
                  return self->GetTag();
              })
              .Setter([](UIElement *self, uint64_t value) {
                  self->SetTag(value);
              })),

      LayoutTag(
          Property<uint64_t>::Init(this)
              .Getter([](UIElement *self) -> uint64_t {
                  return self->_layoutTag;
              })
              .Setter([](UIElement *self, uint64_t value) {
                  if (self->_layoutTag != value) {
                      self->_layoutTag = value;
                      self->RaisePropertyChanged(_PropId_LayoutTag);
                      self->InvalidateMeasure();
                  }
              })),

      ContextMenu(
          Property<sw::ContextMenu *>::Init(this)
              .Getter([](UIElement *self) -> sw::ContextMenu * {
                  return self->_contextMenu;
              })
              .Setter([](UIElement *self, sw::ContextMenu *value) {
                  if (self->_contextMenu != value) {
                      self->_contextMenu = value;
                      self->RaisePropertyChanged(_PropId_ContextMenu);
                  }
              })),

      Float(
          Property<bool>::Init(this)
              .Getter([](UIElement *self) -> bool {
                  return self->_float;
              })
              .Setter([](UIElement *self, bool value) {
                  if (self->_float != value) {
                      self->_float = value;
                      self->RaisePropertyChanged(_PropId_Float);
                      self->UpdateSiblingsZOrder();
                  }
              })),

      TabStop(
          Property<bool>::Init(this)
              .Getter([](UIElement *self) -> bool {
                  return self->_tabStop;
              })
              .Setter([](UIElement *self, bool value) {
                  if (self->_tabStop != value) {
                      self->_tabStop = value;
                      self->RaisePropertyChanged(_PropId_TabStop);
                  }
              })),

      BackColor(
          Property<Color>::Init(this)
              .Getter([](UIElement *self) -> Color {
                  return self->_backColor;
              })
              .Setter([](UIElement *self, const Color &value) {
                  self->Transparent = false;
                  self->SetBackColor(value, true);
              })),

      TextColor(
          Property<Color>::Init(this)
              .Getter([](UIElement *self) -> Color {
                  return self->_textColor;
              })
              .Setter([](UIElement *self, const Color &value) {
                  self->InheritTextColor = false;
                  self->SetTextColor(value, true);
              })),

      Transparent(
          Property<bool>::Init(this)
              .Getter([](UIElement *self) -> bool {
                  return self->_transparent;
              })
              .Setter([](UIElement *self, bool value) {
                  if (self->_transparent != value) {
                      self->_transparent = value;
                      self->RaisePropertyChanged(_PropId_Transparent);
                      self->Redraw();
                  }
              })),

      InheritTextColor(
          Property<bool>::Init(this)
              .Getter([](UIElement *self) -> bool {
                  return self->_inheritTextColor;
              })
              .Setter([](UIElement *self, bool value) {
                  if (self->_inheritTextColor != value) {
                      self->_inheritTextColor = value;
                      self->RaisePropertyChanged(_PropId_InheritTextColor);
                      self->Redraw();
                  }
              })),

      LayoutUpdateCondition(
          Property<sw::LayoutUpdateCondition>::Init(this)
              .Getter([](UIElement *self) -> sw::LayoutUpdateCondition {
                  return self->_layoutUpdateCondition;
              })
              .Setter([](UIElement *self, sw::LayoutUpdateCondition value) {
                  self->_layoutUpdateCondition = value;
              })),

      IsMeasureValid(
          Property<bool>::Init(this)
              .Getter([](UIElement *self) -> bool {
                  return !self->IsLayoutUpdateConditionSet(sw::LayoutUpdateCondition::MeasureInvalidated);
              })),

      MinWidth(
          Property<double>::Init(this)
              .Getter([](UIElement *self) -> double {
                  return self->_minSize.width;
              })
              .Setter([](UIElement *self, double value) {
                  if (self->_minSize.width != value) {
                      self->_minSize.width = value;
                      self->RaisePropertyChanged(_PropId_MinWidth);
                      self->OnMinMaxSizeChanged();
                  }
              })),

      MinHeight(
          Property<double>::Init(this)
              .Getter([](UIElement *self) -> double {
                  return self->_minSize.height;
              })
              .Setter([](UIElement *self, double value) {
                  if (self->_minSize.height != value) {
                      self->_minSize.height = value;
                      self->RaisePropertyChanged(_PropId_MinHeight);
                      self->OnMinMaxSizeChanged();
                  }
              })),

      MaxWidth(
          Property<double>::Init(this)
              .Getter([](UIElement *self) -> double {
                  return self->_maxSize.width;
              })
              .Setter([](UIElement *self, double value) {
                  if (self->_maxSize.width != value) {
                      self->_maxSize.width = value;
                      self->RaisePropertyChanged(_PropId_MaxWidth);
                      self->OnMinMaxSizeChanged();
                  }
              })),

      MaxHeight(
          Property<double>::Init(this)
              .Getter([](UIElement *self) -> double {
                  return self->_maxSize.height;
              })
              .Setter([](UIElement *self, double value) {
                  if (self->_maxSize.height != value) {
                      self->_maxSize.height = value;
                      self->RaisePropertyChanged(_PropId_MaxHeight);
                      self->OnMinMaxSizeChanged();
                  }
              })),

      LogicalRect(
          Property<sw::Rect>::Init(this)
              .Getter([](UIElement *self) -> sw::Rect {
                  sw::Size size = self->_origionalSize;
                  sw::Point pos = self->Rect->GetPos();
                  if (self->_parent != nullptr) {
                      pos.x -= self->_parent->_arrangeOffsetX;
                      pos.y -= self->_parent->_arrangeOffsetY;
                  }
                  return sw::Rect{
                      pos.x, pos.y, size.width, size.height};
              })),

      IsHitTestVisible(
          Property<bool>::Init(this)
              .Getter([](UIElement *self) -> bool {
                  return self->_isHitTestVisible;
              })
              .Setter([](UIElement *self, bool value) {
                  if (self->_isHitTestVisible != value) {
                      self->_isHitTestVisible = value;
                      self->RaisePropertyChanged(_PropId_IsHitTestVisible);
                  }
              })),

      IsFocusedViaTab(
          Property<bool>::Init(this)
              .Getter([](UIElement *self) -> bool {
                  return self->_focusedViaTab;
              })),

      DataContext(
          Property<DynamicObject *>::Init(this)
              .Getter([](UIElement *self) -> DynamicObject * {
                  return self->_dataContext;
              })
              .Setter([](UIElement *self, DynamicObject *value) {
                  if (self->_dataContext != value) {
                      auto oldDataContext = self->_GetCurrentDataContext();
                      self->_dataContext  = value;
                      self->RaisePropertyChanged(_PropId_DataContext);
                      if (oldDataContext != value) {
                          self->_OnCurrentDataContextChanged(oldDataContext);
                      }
                  }
              })),

      CurrentDataContext(
          Property<DynamicObject *>::Init(this)
              .Getter([](UIElement *self) -> DynamicObject * {
                  return self->_GetCurrentDataContext();
              }))
{
}

sw::UIElement::~UIElement()
{
    // 将自己从父窗口的children中移除
    this->SetParent(nullptr);

    // 释放资源
    if (this->_hCtlColorBrush != NULL) {
        DeleteObject(this->_hCtlColorBrush);
    }
}

void sw::UIElement::RegisterRoutedEvent(RoutedEventType eventType, const RoutedEventHandler &handler)
{
    this->_eventMap[eventType] = handler;
}

void sw::UIElement::AddHandler(RoutedEventType eventType, const RoutedEventHandler &handler)
{
    if (handler) this->_eventMap[eventType] += handler;
}

bool sw::UIElement::RemoveHandler(RoutedEventType eventType, const RoutedEventHandler &handler)
{
    return handler == nullptr ? false : this->_eventMap[eventType].Remove(handler);
}

void sw::UIElement::UnregisterRoutedEvent(RoutedEventType eventType)
{
    this->_eventMap[eventType] = nullptr;
}

bool sw::UIElement::IsRoutedEventRegistered(RoutedEventType eventType)
{
    return this->_eventMap[eventType] != nullptr;
}

sw::UIElement &sw::UIElement::operator[](int index) const
{
    return *this->_children[index];
}

sw::UIElement &sw::UIElement::GetChildAt(int index) const
{
    return *this->_children.at(index);
}

bool sw::UIElement::AddChild(UIElement *element)
{
    if (element == nullptr || element == this) {
        return false;
    }

    if (!this->CheckAccess(*element)) {
        return false; // 父子元素必须在同一线程创建
    }

    if (std::find(this->_children.begin(), this->_children.end(), element) != this->_children.end()) {
        return false;
    }

    if (element->_parent != nullptr && !element->_parent->RemoveChild(element)) {
        return false;
    }

    if (!element->WndBase::SetParent(this)) {
        return false;
    }

    // 处理z轴顺序，确保悬浮的元素在最前
    if (!element->_float) {
        HDWP hdwp = BeginDeferWindowPos((int)this->_children.size());
        for (UIElement *child : this->_children) {
            if (child->_float)
                DeferWindowPos(hdwp, child->Handle, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        }
        EndDeferWindowPos(hdwp);
    }

    this->_children.push_back(element);
    this->_UpdateLayoutVisibleChildren();

    this->OnAddedChild(*element);
    return true;
}

bool sw::UIElement::AddChild(UIElement &element)
{
    return this->AddChild(&element);
}

bool sw::UIElement::AddChild(UIElement *element, uint64_t layoutTag)
{
    if (element == nullptr) {
        return false;
    }
    element->_layoutTag = layoutTag;
    return this->AddChild(element);
}

bool sw::UIElement::AddChild(UIElement &element, uint64_t layoutTag)
{
    element._layoutTag = layoutTag;
    return this->AddChild(&element);
}

bool sw::UIElement::RemoveChildAt(int index)
{
    if (index < 0 || index >= (int)this->_children.size()) {
        return false;
    }

    std::vector<UIElement *>::iterator it =
        this->_children.begin() + index;

    if (!(*it)->WndBase::SetParent(nullptr)) {
        return false;
    }

    UIElement *element = *it;
    this->_children.erase(it);
    this->_RemoveFromLayoutVisibleChildren(element);

    this->OnRemovedChild(*element);
    return true;
}

bool sw::UIElement::RemoveChild(UIElement *element)
{
    if (element == nullptr) {
        return false;
    }

    std::vector<UIElement *>::iterator it =
        std::find(this->_children.begin(), this->_children.end(), element);

    if (it == this->_children.end()) {
        return false;
    }

    if (!element->WndBase::SetParent(nullptr)) {
        return false;
    }

    this->_children.erase(it);
    this->_RemoveFromLayoutVisibleChildren(element);

    this->OnRemovedChild(*element);
    return true;
}

bool sw::UIElement::RemoveChild(UIElement &element)
{
    return this->RemoveChild(&element);
}

void sw::UIElement::ClearChildren()
{
    if (this->_children.empty()) {
        return;
    }

    this->_layoutUpdateCondition |= sw::LayoutUpdateCondition::Supressed;

    while (!this->_children.empty()) {
        UIElement *item = this->_children.back();
        item->WndBase::SetParent(nullptr);
        this->_children.pop_back();
        this->OnRemovedChild(*item);
    }

    this->_layoutUpdateCondition &= ~sw::LayoutUpdateCondition::Supressed;
    this->_UpdateLayoutVisibleChildren();

    if (this->IsLayoutUpdateConditionSet(sw::LayoutUpdateCondition::ChildRemoved)) {
        this->InvalidateMeasure();
    }
}

int sw::UIElement::IndexOf(UIElement *element)
{
    std::vector<UIElement *>::iterator beg = this->_children.begin();
    std::vector<UIElement *>::iterator end = this->_children.end();
    std::vector<UIElement *>::iterator it  = std::find(beg, end, element);
    return it == end ? -1 : int(it - beg);
}

int sw::UIElement::IndexOf(UIElement &element)
{
    return this->IndexOf(&element);
}

bool sw::UIElement::ShowContextMenu(const Point &point, sw::HorizontalAlignment horz, sw::VerticalAlignment vert)
{
    UINT uFlags = 0;
    HMENU hMenu = NULL;

    if (this->_contextMenu) {
        hMenu = this->_contextMenu->GetHandle();
    }
    if (hMenu == NULL) {
        return false;
    }

    switch (horz) {
        case sw::HorizontalAlignment::Left: {
            uFlags |= TPM_LEFTALIGN;
            break;
        }
        case sw::HorizontalAlignment::Right: {
            uFlags |= TPM_RIGHTALIGN;
            break;
        }
        case sw::HorizontalAlignment::Center:
        case sw::HorizontalAlignment::Stretch: {
            uFlags |= TPM_CENTERALIGN;
            break;
        }
    }

    switch (vert) {
        case sw::VerticalAlignment::Top: {
            uFlags |= TPM_TOPALIGN;
            break;
        }
        case sw::VerticalAlignment::Bottom: {
            uFlags |= TPM_BOTTOMALIGN;
            break;
        }
        case sw::VerticalAlignment::Center:
        case sw::VerticalAlignment::Stretch: {
            uFlags |= TPM_VCENTERALIGN;
            break;
        }
    }

    POINT pos = point;
    HWND hwnd = this->Handle;
    SetForegroundWindow(hwnd); // 确保菜单能正确关闭
    return TrackPopupMenu(hMenu, uFlags, pos.x, pos.y, 0, hwnd, nullptr);
}

void sw::UIElement::MoveToTop()
{
    UIElement *parent = this->_parent;
    if (parent == nullptr) return;

    int index = parent->IndexOf(this);
    if (index == -1 || index == (int)parent->_children.size() - 1) return;

    HDWP hdwp = BeginDeferWindowPos((int)parent->_children.size());

    parent->_children.erase(parent->_children.begin() + index);
    parent->_children.push_back(this);
    DeferWindowPos(hdwp, this->Handle, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

    if (!this->_float) {
        for (UIElement *item : parent->_children) {
            if (item->_float)
                DeferWindowPos(hdwp, item->Handle, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        }
    }
    EndDeferWindowPos(hdwp);
    parent->_UpdateLayoutVisibleChildren();
    parent->InvalidateMeasure();
}

void sw::UIElement::MoveToBottom()
{
    UIElement *parent = this->_parent;
    if (parent == nullptr) return;

    int index = parent->IndexOf(this);
    if (index == -1 || index == 0) return;

    for (int i = index; i; --i)
        parent->_children[i] = parent->_children[i - 1];
    parent->_children[0] = this;

    if (this->_float) {
        HDWP hdwp = BeginDeferWindowPos((int)parent->_children.size());
        for (UIElement *item : parent->_children) {
            if (item->_float)
                DeferWindowPos(hdwp, item->Handle, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        }
        EndDeferWindowPos(hdwp);
    } else {
        SetWindowPos(this->Handle, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    }
    parent->_UpdateLayoutVisibleChildren();
    parent->InvalidateMeasure();
}

bool sw::UIElement::IsRootElement() const
{
    return this->_parent == nullptr;
}

sw::UIElement *sw::UIElement::GetRootElement()
{
    UIElement *root;
    UIElement *element = this;
    do {
        root    = element;
        element = element->_parent;
    } while (element != nullptr);
    return root;
}

sw::UIElement *sw::UIElement::GetNextElement()
{
    return _GetNextElement(this);
}

sw::UIElement *sw::UIElement::GetPreviousElement()
{
    return _GetPreviousElement(this);
}

sw::UIElement *sw::UIElement::GetNextTabStopElement()
{
    UIElement *element = this;
    do {
        element = element->GetNextElement();
    } while (element != nullptr && element != this &&
             !(element->_tabStop && element->IsVisible() && element->Enabled));
    return element;
}

sw::UIElement *sw::UIElement::GetPreviousTabStopElement()
{
    UIElement *element = this;
    do {
        element = element->GetPreviousElement();
    } while (element != nullptr && element != this &&
             !(element->_tabStop && element->IsVisible() && element->Enabled));
    return element;
}

sw::Color sw::UIElement::GetRealBackColor() const
{
    auto *p = this;
    while (p->_transparent && p->_parent != nullptr) {
        p = p->_parent;
    }
    return p->_backColor;
}

sw::Color sw::UIElement::GetRealTextColor() const
{
    auto *p = this;
    while (p->_inheritTextColor && p->_parent != nullptr) {
        p = p->_parent;
    }
    return p->_textColor;
}

void sw::UIElement::SetCursor(HCURSOR hCursor)
{
    this->_hCursor          = hCursor;
    this->_useDefaultCursor = false;
}

void sw::UIElement::SetCursor(StandardCursor cursor)
{
    this->SetCursor(CursorHelper::GetCursorHandle(cursor));
}

void sw::UIElement::ResetCursor()
{
    this->_hCursor          = NULL;
    this->_useDefaultCursor = true;
}

void sw::UIElement::SetAlignment(sw::HorizontalAlignment horz, sw::VerticalAlignment vert)
{
    bool changed = false;

    changed |= this->_SetHorzAlignment(horz);
    changed |= this->_SetVertAlignment(vert);

    if (changed) {
        this->InvalidateMeasure();
    }
}

void sw::UIElement::Resize(const Size &size)
{
    this->_origionalSize = size;
    this->ClampDesireSize(this->_origionalSize);

    if (!this->IsRootElement()) {
        this->InvalidateMeasure();
        return;
    }

    SetWindowPos(this->Handle, NULL,
                 0, 0, Dip::DipToPxX(size.width), Dip::DipToPxY(size.height),
                 SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);
}

bool sw::UIElement::IsLayoutUpdateConditionSet(sw::LayoutUpdateCondition condition)
{
    return (this->_layoutUpdateCondition & condition) == condition;
}

void sw::UIElement::InvalidateMeasure()
{
    if (this->IsLayoutUpdateConditionSet(sw::LayoutUpdateCondition::Supressed)) {
        return;
    }

    UIElement *root;
    UIElement *element = this;
    do {
        root = element;
        element->_SetMeasureInvalidated();
        element = element->_parent;
    } while (element != nullptr);

    root->SendMessageW(WM_UpdateLayout, 0, 0);
}

bool sw::UIElement::BringIntoView()
{
    UIElement *p = this->_parent;
    if (this->_float || p == nullptr) return false;

    sw::Rect rect = this->Rect;
    sw::Point pos = p->PointToScreen(rect.GetPos());

    rect.left = pos.x - this->_margin.left;
    rect.top  = pos.y - this->_margin.top;
    rect.width += this->_margin.left + this->_margin.right;
    rect.height += this->_margin.top + this->_margin.bottom;

    for (; p != nullptr; p = p->_parent) {
        rect.left -= p->_arrangeOffsetX;
        rect.top -= p->_arrangeOffsetY;
        if (p->RequestBringIntoView(rect)) return true;
        if (p->_float) return false;
    }
    return false;
}

bool sw::UIElement::AddBinding(BindingBase *binding)
{
    if (binding == nullptr) {
        return false;
    } else {
        this->_bindings[binding->GetTargetPropertyId()].reset(binding);
        return true;
    }
}

bool sw::UIElement::AddBinding(Binding *binding)
{
    if (binding == nullptr) {
        return false;
    }
    if (binding->GetSourceObject() == nullptr) {
        auto dataBinding = DataBinding::Create(this, binding);
        return this->AddBinding(static_cast<BindingBase *>(dataBinding));
    } else {
        binding->SetTargetObject(this);
        return this->AddBinding(static_cast<BindingBase *>(binding));
    }
}

bool sw::UIElement::AddBinding(DataBinding *binding)
{
    if (binding == nullptr) {
        return false;
    }
    binding->SetTargetElement(this);
    return this->AddBinding(static_cast<BindingBase *>(binding));
}

bool sw::UIElement::RemoveBinding(FieldId propertyId)
{
    if (this->_bindings.count(propertyId) == 0) {
        return false;
    } else {
        this->_bindings.erase(propertyId);
        return true;
    }
}

uint64_t sw::UIElement::GetTag() const
{
    return this->_tag;
}

void sw::UIElement::SetTag(uint64_t tag)
{
    if (this->_tag != tag) {
        this->_tag = tag;
        this->RaisePropertyChanged(_PropId_Tag);
    }
}

uint64_t sw::UIElement::GetLayoutTag() const
{
    return this->_layoutTag;
}

int sw::UIElement::GetChildLayoutCount() const
{
    return (int)this->_layoutVisibleChildren.size();
}

sw::ILayout &sw::UIElement::GetChildLayoutAt(int index)
{
    return *this->_layoutVisibleChildren[index];
}

sw::Size sw::UIElement::GetDesireSize() const
{
    return this->_desireSize;
}

void sw::UIElement::Measure(const Size &availableSize)
{
    if (!this->IsLayoutUpdateConditionSet(sw::LayoutUpdateCondition::MeasureInvalidated) &&
        this->_lastMeasureAvailableSize == availableSize) {
        return; // 若布局未失效且可用尺寸没有变化，则无需重新测量
    }

    Size measureSize    = availableSize;
    Thickness &margin   = this->_margin;
    sw::Rect windowRect = this->Rect;
    sw::Rect clientRect = this->ClientRect;

    // 考虑边框
    measureSize.width -= (windowRect.width - clientRect.width) + margin.left + margin.right;
    measureSize.height -= (windowRect.height - clientRect.height) + margin.top + margin.bottom;

    // 由子类实现MeasureOverride函数来计算内容所需的尺寸
    this->_desireSize = this->MeasureOverride(measureSize);

    // 若MeasureOverride返回Size{NAN,NAN}则表示使用默认实现，直接使用原始尺寸
    // 若子类实现了MeasureOverride，则子类应保证返回值合理，此时加上边框大小
    if (std::isnan(this->_desireSize.width) &&
        std::isnan(this->_desireSize.height)) {
        this->_desireSize = this->_origionalSize;
    } else {
        this->_desireSize.width += windowRect.width - clientRect.width;
        this->_desireSize.height += windowRect.height - clientRect.height;
    }

    // 限制尺寸在最小和最大尺寸之间
    this->ClampDesireSize(this->_desireSize);
    this->_desireSize.width += margin.left + margin.right;
    this->_desireSize.height += margin.top + margin.bottom;

    // 更新_lastMeasureAvailableSize
    this->_lastMeasureAvailableSize = availableSize;
}

void sw::UIElement::Arrange(const sw::Rect &finalPosition)
{
    // 为什么在Arrange阶段清除MeasureInvalidated标记：
    // 一些复杂的布局可能会在测量阶段多次调用Measure函数，
    // 比如Grid会调用两次Measure，若在测量阶段清除该标记，
    // 则在第二次调用时会认为布局没有失效，导致测量被跳过。
    this->_layoutUpdateCondition &= ~sw::LayoutUpdateCondition::MeasureInvalidated;
    this->_layoutUpdateCondition |= sw::LayoutUpdateCondition::Supressed;

    Size &desireSize  = this->_desireSize;
    Thickness &margin = this->_margin;

    sw::Rect rect{};
    rect.width  = desireSize.width - margin.left - margin.right;
    rect.height = desireSize.height - margin.top - margin.bottom;

    if (this->_horizontalAlignment == HorizontalAlignment::Stretch) {
        rect.width = finalPosition.width - margin.left - margin.right;
    }

    if (this->_verticalAlignment == VerticalAlignment::Stretch) {
        rect.height = finalPosition.height - margin.top - margin.bottom;
    }

    this->ClampDesireSize(rect);

    switch (this->_horizontalAlignment) {
        case HorizontalAlignment::Center:
        case HorizontalAlignment::Stretch: {
            rect.left = finalPosition.left + (finalPosition.width - rect.width - margin.left - margin.right) / 2 + margin.left;
            break;
        }
        case HorizontalAlignment::Left: {
            rect.left = finalPosition.left + margin.left;
            break;
        }
        case HorizontalAlignment::Right: {
            rect.left = finalPosition.left + finalPosition.width - rect.width - margin.right;
            break;
        }
    }

    switch (this->_verticalAlignment) {
        case VerticalAlignment::Center:
        case VerticalAlignment::Stretch: {
            rect.top = finalPosition.top + (finalPosition.height - rect.height - margin.top - margin.bottom) / 2 + margin.top;
            break;
        }
        case VerticalAlignment::Top: {
            rect.top = finalPosition.top + margin.top;
            break;
        }
        case VerticalAlignment::Bottom: {
            rect.top = finalPosition.top + finalPosition.height - rect.height - margin.bottom;
            break;
        }
    }

    if (this->_parent && !this->_float) { // 考虑偏移量
        rect.left += this->_parent->_arrangeOffsetX;
        rect.top += this->_parent->_arrangeOffsetY;
    }

    rect.width  = Utils::Max(0.0, rect.width);
    rect.height = Utils::Max(0.0, rect.height);

    bool hasChildren = !this->_children.empty();
    HDWP hdwpCurrent = this->_parent ? this->_parent->_hdwpChildren : NULL;

    if (!hasChildren && hdwpCurrent != NULL) {
        DeferWindowPos(hdwpCurrent, this->Handle, NULL,
                       Dip::DipToPxX(rect.left), Dip::DipToPxY(rect.top),
                       Dip::DipToPxX(rect.width), Dip::DipToPxY(rect.height),
                       SWP_NOACTIVATE | SWP_NOZORDER);
    } else {
        SetWindowPos(this->Handle, NULL,
                     Dip::DipToPxX(rect.left), Dip::DipToPxY(rect.top),
                     Dip::DipToPxX(rect.width), Dip::DipToPxY(rect.height),
                     SWP_NOACTIVATE | SWP_NOZORDER);
    }

    if (hasChildren) {
        if (this->_children.size() >= 3) {
            this->_hdwpChildren = BeginDeferWindowPos((int)this->_children.size());
        }

        this->ArrangeOverride(this->ClientRect->GetSize());

        if (this->_hdwpChildren != NULL) {
            EndDeferWindowPos(this->_hdwpChildren);
            this->_hdwpChildren = NULL;
        }
    }

    this->_layoutUpdateCondition &= ~sw::LayoutUpdateCondition::Supressed;
}

sw::UIElement *sw::UIElement::ToUIElement()
{
    return this;
}

void sw::UIElement::RaiseRoutedEvent(RoutedEventType eventType)
{
    RoutedEventArgs eventArgs(eventType);
    this->RaiseRoutedEvent(eventArgs);
}

void sw::UIElement::RaiseRoutedEvent(RoutedEventArgs &eventArgs)
{
    eventArgs.originalSource = this;

    if (eventArgs.source == nullptr) {
        eventArgs.source = this;
    }

    UIElement *element = this;
    do {
        auto &handler = element->_eventMap[eventArgs.eventType];
        element->OnRoutedEvent(eventArgs, handler);

        if (eventArgs.handled) {
            break;
        } else {
            element = element->_parent;
        }
    } while (element != nullptr);
}

double &sw::UIElement::GetInternalArrangeOffsetX()
{
    return this->_arrangeOffsetX;
}

double &sw::UIElement::GetInternalArrangeOffsetY()
{
    return this->_arrangeOffsetY;
}

double sw::UIElement::GetChildRightmost(bool update)
{
    if (update) {
        this->_childRightmost = 0;
        for (UIElement *item : this->_layoutVisibleChildren) {
            if (item->_float) continue;
            this->_childRightmost = Utils::Max(this->_childRightmost, item->Left + item->Width + item->_margin.right - this->_arrangeOffsetX);
        }
    }
    return this->_childRightmost;
}

double sw::UIElement::GetChildBottommost(bool update)
{
    if (update) {
        this->_childBottommost = 0;
        for (UIElement *item : this->_layoutVisibleChildren) {
            if (item->_float) continue;
            this->_childBottommost = Utils::Max(this->_childBottommost, item->Top + item->Height + item->_margin.bottom - this->_arrangeOffsetY);
        }
    }
    return this->_childBottommost;
}

void sw::UIElement::UpdateChildrenZOrder(bool invalidateMeasure)
{
    int childCount = (int)this->_children.size();
    if (childCount < 2) return;

    std::deque<HWND> floatingElements;
    HDWP hdwp = BeginDeferWindowPos(childCount);

    for (UIElement *child : this->_children) {
        HWND hwnd = child->Handle;
        if (child->_float) {
            floatingElements.push_back(hwnd);
        } else {
            DeferWindowPos(hdwp, hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        }
    }

    while (!floatingElements.empty()) {
        DeferWindowPos(hdwp, floatingElements.front(), HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        floatingElements.pop_front();
    }

    EndDeferWindowPos(hdwp);

    if (invalidateMeasure) {
        this->InvalidateMeasure();
    }
}

void sw::UIElement::UpdateSiblingsZOrder(bool invalidateMeasure)
{
    if (this->_parent != nullptr) {
        this->_parent->UpdateChildrenZOrder(invalidateMeasure);
    }
}

void sw::UIElement::ClampDesireSize(sw::Size &size) const
{
    if (this->_minSize.width > 0) {
        size.width = Utils::Max(size.width, this->_minSize.width);
    }
    if (this->_minSize.height > 0) {
        size.height = Utils::Max(size.height, this->_minSize.height);
    }
    if (this->_maxSize.width > 0) {
        size.width = Utils::Min(size.width, this->_maxSize.width);
    }
    if (this->_maxSize.height > 0) {
        size.height = Utils::Min(size.height, this->_maxSize.height);
    }
}

void sw::UIElement::ClampDesireSize(sw::Rect &rect) const
{
    auto size = rect.GetSize();
    this->ClampDesireSize(size);
    rect.width  = size.width;
    rect.height = size.height;
}

bool sw::UIElement::QueryAllChildren(const Func<UIElement *, bool> &queryFunc)
{
    if (queryFunc == nullptr) {
        return true;
    }

    std::vector<UIElement *> children;
    _GetAllChildren(this, children);

    for (UIElement *child : children) {
        if (!queryFunc(child)) return false;
    }
    return true;
}

sw::Size sw::UIElement::MeasureOverride(const Size &availableSize)
{
    // 返回NAN表示使用默认实现
    return Size{NAN, NAN};
}

void sw::UIElement::ArrangeOverride(const Size &finalSize)
{
}

void sw::UIElement::SetBackColor(Color color, bool redraw)
{
    this->_backColor = color;
    this->RaisePropertyChanged(_PropId_BackColor);

    if (redraw) {
        this->Redraw();
    }
}

void sw::UIElement::SetTextColor(Color color, bool redraw)
{
    this->_textColor = color;
    this->RaisePropertyChanged(_PropId_TextColor);

    if (redraw) {
        this->Redraw();
    }
}

bool sw::UIElement::RequestBringIntoView(const sw::Rect &screenRect)
{
    return false;
}

void sw::UIElement::OnAddedChild(UIElement &element)
{
    this->RaisePropertyChanged(_PropId_ChildCount);

    if (this->IsLayoutUpdateConditionSet(sw::LayoutUpdateCondition::ChildAdded)) {
        this->InvalidateMeasure();
    }
}

void sw::UIElement::OnRemovedChild(UIElement &element)
{
    this->RaisePropertyChanged(_PropId_ChildCount);

    if (this->IsLayoutUpdateConditionSet(sw::LayoutUpdateCondition::ChildRemoved)) {
        this->InvalidateMeasure();
    }
}

void sw::UIElement::OnTabMove(bool forward)
{
    UIElement *next = nullptr;

    // 获取下一个可Tab停止的元素
    if (forward) {
        next = this->GetNextTabStopElement();
    } else {
        next = this->GetPreviousTabStopElement();
    }

    // 跳转到下一个可Tab停止的元素
    if (next != nullptr && next != this) {
        next->OnTabStop();
    }
}

void sw::UIElement::OnTabStop()
{
    // 标记为通过Tab键获得焦点
    this->_focusedViaTab = true;

    // 设置焦点并滚动到可见区域
    this->Focused = true;
    this->BringIntoView();
}

void sw::UIElement::OnMinMaxSizeChanged()
{
    this->InvalidateMeasure();
}

void sw::UIElement::OnRoutedEvent(RoutedEventArgs &eventArgs, const RoutedEventHandler &handler)
{
    if (handler) {
        handler(*this, eventArgs);
    }
}

bool sw::UIElement::SetParent(WndBase *parent)
{
    UIElement *oldParentElement = this->_parent;
    UIElement *newParentElement = parent ? parent->ToUIElement() : nullptr;

    if (newParentElement != nullptr && !newParentElement->CheckAccess(*this)) {
        return false; // 父子元素必须在同一线程创建
    }

    if (newParentElement == this) {
        return false; // 不能将自己设置为自己的父元素
    }

    if (oldParentElement == newParentElement) {
        return true; // 父元素没有变化
    }

    if (newParentElement == nullptr) {
        /*
         * 要设置的父元素为nullptr
         */
        if (oldParentElement == nullptr) {
            // 前面已排除父元素没有变化的情况，不会执行到这里
            this->_parent = nullptr;
            return true;
        } else {
            // UIElement虚构函数时调用SerParent函数保证从父元素的Children中移除
            // 当当前元素已经添加到界面中但中途对象被销毁时会出现该情况，调用父元素的RemoveChild函数移除引用
            bool success = oldParentElement->RemoveChild(this);
            // 当程序将要退出时::SetParent函数可能会失败，此时RemoveChild函数返回false
            // 此时直接将其从父元素的Children中移除
            if (!success) {
                auto it = std::find(oldParentElement->_children.begin(), oldParentElement->_children.end(), this);
                if (it != oldParentElement->_children.end()) oldParentElement->_children.erase(it);
                // 前面调用RemoveChild失败，当前元素仍在父元素的_layoutVisibleChildren中，此处手动调用更新
                oldParentElement->_RemoveFromLayoutVisibleChildren(this);
                // 通知父元素改变以触发属性变更通知以及数据上下文变更
                this->ParentChanged(nullptr);
            }
            return true;
        }
    } else {
        /*
         * 添加为某个元素的子元素
         */
        if (oldParentElement == nullptr) {
            return newParentElement->AddChild(this);
        } else {
            return oldParentElement->RemoveChild(this) &&
                   newParentElement->AddChild(this);
        }
    }
}

void sw::UIElement::ParentChanged(WndBase *newParent)
{
    auto oldDataContext = this->_GetCurrentDataContext();

    this->_parent = newParent ? newParent->ToUIElement() : nullptr;
    this->_SetMeasureInvalidated();

    this->WndBase::ParentChanged(newParent); // raise property WndBase::Parent changed event
    this->RaisePropertyChanged(_PropId_UIElementParent);

    if (this->_GetCurrentDataContext() != oldDataContext) {
        this->_OnCurrentDataContextChanged(oldDataContext);
    }
}

bool sw::UIElement::OnClose()
{
    this->SetParent(nullptr);
    return this->WndBase::OnClose();
}

bool sw::UIElement::OnMove(const Point &newClientPosition)
{
    this->WndBase::OnMove(newClientPosition);
    this->RaisePropertyChanged(_PropId_LogicalRect);

    PositionChangedEventArgs args(newClientPosition);
    this->RaiseRoutedEvent(args);

    if (this->IsLayoutUpdateConditionSet(sw::LayoutUpdateCondition::PositionChanged)) {
        this->InvalidateMeasure();
    }
    return args.handledMsg;
}

bool sw::UIElement::OnSize(const Size &newClientSize)
{
    this->WndBase::OnSize(newClientSize);
    this->RaisePropertyChanged(_PropId_LogicalRect);

    if (this->_horizontalAlignment != sw::HorizontalAlignment::Stretch) {
        this->_origionalSize.width = this->Width;
    }
    if (this->_verticalAlignment != sw::VerticalAlignment::Stretch) {
        this->_origionalSize.height = this->Height;
    }

    SizeChangedEventArgs args(newClientSize);
    this->RaiseRoutedEvent(args);

    if (this->IsLayoutUpdateConditionSet(sw::LayoutUpdateCondition::SizeChanged)) {
        this->InvalidateMeasure();
    }
    return args.handledMsg;
}

void sw::UIElement::OnTextChanged()
{
    this->WndBase::OnTextChanged();
    this->RaiseRoutedEvent(UIElement_TextChanged);

    if (this->IsLayoutUpdateConditionSet(sw::LayoutUpdateCondition::TextChanged)) {
        this->InvalidateMeasure();
    }
}

void sw::UIElement::FontChanged(HFONT hfont)
{
    this->WndBase::FontChanged(hfont);

    if (this->IsLayoutUpdateConditionSet(sw::LayoutUpdateCondition::FontChanged)) {
        this->InvalidateMeasure();
    }
}

void sw::UIElement::VisibleChanged(bool newVisible)
{
    this->WndBase::VisibleChanged(newVisible);

    if (this->_parent && this->_collapseWhenHide) {
        this->_parent->_UpdateLayoutVisibleChildren();
    }
    if (newVisible || this->_collapseWhenHide) {
        this->InvalidateMeasure(); // visible变为true，或者需要折叠隐藏时，更新布局
    }
}

bool sw::UIElement::OnSetFocus(HWND hPrevFocus)
{
    this->WndBase::OnSetFocus(hPrevFocus);

    TypedRoutedEventArgs<UIElement_GotFocus> args;
    this->RaiseRoutedEvent(args);

    if (!args.handledMsg && this->_parent != nullptr) {
        int action = this->_focusedViaTab ? UIS_CLEAR : UIS_SET;
        this->_parent->SendMessageW(WM_UPDATEUISTATE, MAKEWPARAM(action, UISF_HIDEFOCUS), 0);
    }
    return args.handledMsg;
}

bool sw::UIElement::OnKillFocus(HWND hNextFocus)
{
    this->WndBase::OnKillFocus(hNextFocus);
    this->_focusedViaTab = false;

    TypedRoutedEventArgs<UIElement_LostFocus> args;
    this->RaiseRoutedEvent(args);
    return args.handledMsg;
}

bool sw::UIElement::OnChar(wchar_t ch, const KeyFlags &flags)
{
    GotCharEventArgs args(ch, flags);
    this->RaiseRoutedEvent(args);
    return args.handledMsg;
}

bool sw::UIElement::OnKeyDown(VirtualKey key, const KeyFlags &flags)
{
    KeyDownEventArgs args(key, flags);
    this->RaiseRoutedEvent(args);

    // 实现按下Tab键转移焦点
    if (!args.handledMsg && key == VirtualKey::Tab) {
        bool shiftDown = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
        this->OnTabMove(!shiftDown);
    }

    return args.handledMsg;
}

bool sw::UIElement::OnKeyUp(VirtualKey key, const KeyFlags &flags)
{
    KeyUpEventArgs args(key, flags);
    this->RaiseRoutedEvent(args);
    return args.handledMsg;
}

bool sw::UIElement::OnMouseMove(const Point &mousePosition, MouseKey keyState)
{
    MouseMoveEventArgs args(mousePosition, keyState);
    this->RaiseRoutedEvent(args);
    return args.handledMsg;
}

bool sw::UIElement::OnMouseLeave()
{
    TypedRoutedEventArgs<UIElement_MouseLeave> args;
    this->RaiseRoutedEvent(args);
    return args.handledMsg;
}

bool sw::UIElement::OnMouseWheel(int wheelDelta, const Point &mousePosition, MouseKey keyState)
{
    MouseWheelEventArgs args(wheelDelta, mousePosition, keyState);
    this->RaiseRoutedEvent(args);
    return args.handledMsg;
}

bool sw::UIElement::OnMouseLeftButtonDown(const Point &mousePosition, MouseKey keyState)
{
    MouseButtonDownEventArgs args(MouseKey::MouseLeft, mousePosition, keyState);
    this->RaiseRoutedEvent(args);
    return args.handledMsg;
}

bool sw::UIElement::OnMouseLeftButtonUp(const Point &mousePosition, MouseKey keyState)
{
    MouseButtonUpEventArgs args(MouseKey::MouseLeft, mousePosition, keyState);
    this->RaiseRoutedEvent(args);
    return args.handledMsg;
}

bool sw::UIElement::OnMouseRightButtonDown(const Point &mousePosition, MouseKey keyState)
{
    MouseButtonDownEventArgs args(MouseKey::MouseRight, mousePosition, keyState);
    this->RaiseRoutedEvent(args);
    return args.handledMsg;
}

bool sw::UIElement::OnMouseRightButtonUp(const Point &mousePosition, MouseKey keyState)
{
    MouseButtonUpEventArgs args(MouseKey::MouseRight, mousePosition, keyState);
    this->RaiseRoutedEvent(args);
    return args.handledMsg;
}

bool sw::UIElement::OnMouseMiddleButtonDown(const Point &mousePosition, MouseKey keyState)
{
    MouseButtonDownEventArgs args(MouseKey::MouseMiddle, mousePosition, keyState);
    this->RaiseRoutedEvent(args);
    return args.handledMsg;
}

bool sw::UIElement::OnMouseMiddleButtonUp(const Point &mousePosition, MouseKey keyState)
{
    MouseButtonUpEventArgs args(MouseKey::MouseMiddle, mousePosition, keyState);
    this->RaiseRoutedEvent(args);
    return args.handledMsg;
}

bool sw::UIElement::OnContextMenu(bool isKeyboardMsg, const Point &mousePosition)
{
    if (this->_contextMenu == nullptr) {
        return false;
    }

    ShowContextMenuEventArgs args(isKeyboardMsg, mousePosition);
    this->RaiseRoutedEvent(args);

    if (!args.cancel) {
        this->ShowContextMenu(isKeyboardMsg ? this->PointToScreen({0, 0}) : mousePosition);
    }

    return true;
}

void sw::UIElement::OnMenuCommand(int id)
{
    if (this->_contextMenu) {
        MenuItem *item = this->_contextMenu->GetMenuItem(id);
        if (item) item->CallCommand();
    }
}

bool sw::UIElement::OnColor(HDC hdc, HBRUSH &hRetBrush)
{
    COLORREF textColor = this->GetRealTextColor();
    COLORREF backColor = this->GetRealBackColor();

    ::SetTextColor(hdc, textColor);
    ::SetBkColor(hdc, backColor);

    if (this->_lastTextColor != textColor ||
        this->_lastBackColor != backColor) {
        if (this->_hCtlColorBrush != NULL)
            DeleteObject(this->_hCtlColorBrush);
        this->_hCtlColorBrush = NULL;
        this->_lastTextColor  = textColor;
        this->_lastBackColor  = backColor;
    }

    if (this->_hCtlColorBrush == NULL)
        this->_hCtlColorBrush = CreateSolidBrush(backColor);

    hRetBrush = this->_hCtlColorBrush;
    return true;
}

bool sw::UIElement::OnSetCursor(HWND hwnd, HitTestResult hitTest, int message, bool &result)
{
    if (this->_useDefaultCursor || hitTest != HitTestResult::HitClient) {
        return false;
    }
    ::SetCursor(this->_hCursor);
    result = true;
    return true;
}

bool sw::UIElement::OnDropFiles(HDROP hDrop)
{
    DropFilesEventArgs args(hDrop);
    this->RaiseRoutedEvent(args);
    return args.handledMsg;
}

void sw::UIElement::OnNcHitTest(const Point &testPoint, HitTestResult &result)
{
    if (!this->_isHitTestVisible) {
        result = HitTestResult::HitTransparent;
    }
}

bool sw::UIElement::_SetHorzAlignment(sw::HorizontalAlignment value)
{
    if (value == this->_horizontalAlignment) {
        return false;
    }
    if (value == sw::HorizontalAlignment::Stretch) {
        this->_origionalSize.width = this->Width;
    }
    this->_horizontalAlignment = value;
    this->RaisePropertyChanged(_PropId_HorizontalAlignment);
    return true;
}

bool sw::UIElement::_SetVertAlignment(sw::VerticalAlignment value)
{
    if (value == this->_verticalAlignment) {
        return false;
    }
    if (value == sw::VerticalAlignment::Stretch) {
        this->_origionalSize.height = this->Height;
    }
    this->_verticalAlignment = value;
    this->RaisePropertyChanged(_PropId_VerticalAlignment);
    return true;
}

void sw::UIElement::_SetMeasureInvalidated()
{
    this->_layoutUpdateCondition |= sw::LayoutUpdateCondition::MeasureInvalidated;
}

void sw::UIElement::_UpdateLayoutVisibleChildren()
{
    this->_layoutVisibleChildren.clear();

    for (UIElement *item : this->_children) {
        if (!item->_collapseWhenHide || item->Visible)
            this->_layoutVisibleChildren.push_back(item);
    }
}

void sw::UIElement::_RemoveFromLayoutVisibleChildren(UIElement *element)
{
    auto it = std::find(
        this->_layoutVisibleChildren.begin(),
        this->_layoutVisibleChildren.end(), element);

    if (it != this->_layoutVisibleChildren.end()) {
        this->_layoutVisibleChildren.erase(it);
    }
}

sw::DynamicObject *sw::UIElement::_GetCurrentDataContext()
{
    DynamicObject *result = nullptr;
    UIElement *element    = this;
    do {
        result  = element->_dataContext;
        element = element->_parent;
    } while (result == nullptr && element != nullptr);
    return result;
}

void sw::UIElement::_OnCurrentDataContextChanged(DynamicObject *oldval)
{
    std::vector<UIElement *> stack;
    stack.push_back(this);

    while (!stack.empty()) {
        auto current = stack.back();
        stack.pop_back();

        current->RaisePropertyChanged(_PropId_CurrentDataContext);

        if (current->DataContextChanged) {
            current->DataContextChanged(*current, oldval);
        }

        for (UIElement *child : current->_children) {
            if (child->_dataContext == nullptr) {
                stack.push_back(child);
            }
        }
    }
}

sw::UIElement *sw::UIElement::_GetNextElement(UIElement *element, bool searchChildren)
{
    if (searchChildren && !element->_children.empty()) {
        return element->_children.front();
    }

    UIElement *parent = element->_parent;
    if (parent == nullptr) {
        return element; // 回到根节点
    }

    int index = parent->IndexOf(element);
    if (index == (int)parent->_children.size() - 1) {
        return _GetNextElement(parent, false);
    }

    return parent->_children[index + 1];
}

sw::UIElement *sw::UIElement::_GetDeepestLastElement(UIElement *element)
{
    while (!element->_children.empty()) {
        element = element->_children.back();
    }
    return element;
}

sw::UIElement *sw::UIElement::_GetPreviousElement(UIElement *element)
{
    UIElement *parent = element->_parent;

    if (parent == nullptr) {
        return _GetDeepestLastElement(element);
    }

    int index = parent->IndexOf(element);
    return index <= 0 ? parent : _GetDeepestLastElement(parent->_children[index - 1]);
}

void sw::UIElement::_GetAllChildren(UIElement *element, std::vector<UIElement *> &children)
{
    for (UIElement *child : element->_children) {
        children.push_back(child);
        _GetAllChildren(child, children);
    }
}
