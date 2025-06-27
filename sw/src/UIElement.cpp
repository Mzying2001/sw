#include "UIElement.h"
#include "Utils.h"
#include <algorithm>
#include <deque>

sw::UIElement::UIElement()
    : Margin(
          // get
          [this]() -> Thickness {
              return this->_margin;
          },
          // set
          [this](const Thickness &value) {
              this->_margin = value;
              this->InvalidateMeasure();
          }),

      HorizontalAlignment(
          // get
          [this]() -> sw::HorizontalAlignment {
              return this->_horizontalAlignment;
          },
          // set
          [this](const sw::HorizontalAlignment &value) {
              if (this->_SetHorzAlignment(value)) {
                  this->InvalidateMeasure();
              }
          }),

      VerticalAlignment(
          // get
          [this]() -> sw::VerticalAlignment {
              return this->_verticalAlignment;
          },
          // set
          [this](const sw::VerticalAlignment &value) {
              if (this->_SetVertAlignment(value)) {
                  this->InvalidateMeasure();
              }
          }),

      ChildCount(
          // get
          [this]() -> int {
              return (int)this->_children.size();
          }),

      CollapseWhenHide(
          // get
          [this]() -> bool {
              return this->_collapseWhenHide;
          },
          // set
          [this](const bool &value) {
              if (this->_collapseWhenHide != value) {
                  this->_collapseWhenHide = value;
                  if (!this->Visible) this->InvalidateMeasure();
              }
          }),

      Parent(
          // get
          [this]() -> UIElement * {
              return this->_parent;
          }),

      Tag(
          // get
          [this]() -> uint64_t {
              return this->_tag;
          },
          // set
          [this](const uint64_t &value) {
              this->_tag = value;
          }),

      LayoutTag(
          // get
          [this]() -> uint64_t {
              return this->_layoutTag;
          },
          // set
          [this](const uint64_t &value) {
              this->_layoutTag = value;
              this->InvalidateMeasure();
          }),

      ContextMenu(
          // get
          [this]() -> sw::ContextMenu * {
              return this->_contextMenu;
          },
          // set
          [this](sw::ContextMenu *value) {
              this->_contextMenu = value;
          }),

      Float(
          // get
          [this]() -> bool {
              return this->_float;
          },
          // set
          [this](const bool &value) {
              if (this->_float != value) {
                  this->_float = value;
                  this->UpdateSiblingsZOrder();
                  this->InvalidateMeasure();
              }
          }),

      TabStop(
          // get
          [this]() -> bool {
              return this->_tabStop;
          },
          // set
          [this](const bool &value) {
              this->_tabStop = value;
          }),

      BackColor(
          // get
          [this]() -> Color {
              return this->_backColor;
          },
          // set
          [this](const Color &value) {
              this->_transparent = false;
              this->SetBackColor(value, true);
          }),

      TextColor(
          // get
          [this]() -> Color {
              return this->_textColor;
          },
          // set
          [this](const Color &value) {
              this->_inheritTextColor = false;
              this->SetTextColor(value, true);
          }),

      Transparent(
          // get
          [this]() -> bool {
              return this->_transparent;
          },
          // set
          [this](const bool &value) {
              this->_transparent = value;
              this->Redraw();
          }),

      InheritTextColor(
          // get
          [this]() -> bool {
              return this->_inheritTextColor;
          },
          // set
          [this](const bool &value) {
              this->_inheritTextColor = value;
              this->Redraw();
          }),

      LayoutUpdateCondition(
          // get
          [this]() -> sw::LayoutUpdateCondition {
              return this->_layoutUpdateCondition;
          },
          // set
          [this](const sw::LayoutUpdateCondition &value) {
              this->_layoutUpdateCondition = value;
          }),

      IsMeasureValid(
          // get
          [this]() -> bool {
              return !this->IsLayoutUpdateConditionSet(sw::LayoutUpdateCondition::MeasureInvalidated);
          })
{
}

sw::UIElement::~UIElement()
{
    // 将自己从父窗口的children中移除
    this->SetParent(nullptr);
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
    if (element == nullptr) {
        return false;
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

void sw::UIElement::ShowContextMenu(const Point &point)
{
    if (this->_contextMenu != nullptr) {
        POINT p = point;
        TrackPopupMenu(this->_contextMenu->GetHandle(), TPM_LEFTALIGN | TPM_TOPALIGN, p.x, p.y, 0, this->Handle, nullptr);
    }
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
}

bool sw::UIElement::IsRootElement()
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
    } while (element != nullptr && element != this && !(element->_tabStop && element->IsVisible()));
    return element;
}

sw::UIElement *sw::UIElement::GetPreviousTabStopElement()
{
    UIElement *element = this;
    do {
        element = element->GetPreviousElement();
    } while (element != nullptr && element != this && !(element->_tabStop && element->IsVisible()));
    return element;
}

sw::Color sw::UIElement::GetRealBackColor()
{
    UIElement *p = this;
    while (p->_transparent && p->_parent != nullptr) {
        p = p->_parent;
    }
    return p->_backColor;
}

sw::Color sw::UIElement::GetRealTextColor()
{
    UIElement *p = this;
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

uint64_t sw::UIElement::GetTag()
{
    return this->_tag;
}

void sw::UIElement::SetTag(uint64_t tag)
{
    this->_tag = tag;
}

uint64_t sw::UIElement::GetLayoutTag()
{
    return this->_layoutTag;
}

int sw::UIElement::GetChildLayoutCount()
{
    this->_childrenNotCollapsed.clear();
    for (UIElement *item : this->_children) {
        if (item->Visible || !item->_collapseWhenHide)
            this->_childrenNotCollapsed.push_back(item);
    }
    return (int)this->_childrenNotCollapsed.size();
}

sw::ILayout &sw::UIElement::GetChildLayoutAt(int index)
{
    return *this->_childrenNotCollapsed[index];
}

sw::Size sw::UIElement::GetDesireSize()
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

    this->_desireSize = this->MeasureOverride(measureSize);
    this->_desireSize.width += (windowRect.width - clientRect.width) + margin.left + margin.right;
    this->_desireSize.height += (windowRect.height - clientRect.height) + margin.top + margin.bottom;

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

    sw::Rect rect;
    rect.width  = desireSize.width - margin.left - margin.right;
    rect.height = desireSize.height - margin.top - margin.bottom;

    switch (this->_horizontalAlignment) {
        case HorizontalAlignment::Center: {
            rect.left = finalPosition.left + (finalPosition.width - rect.width - margin.left - margin.right) / 2 + margin.left;
            break;
        }
        case HorizontalAlignment::Stretch: {
            rect.left  = finalPosition.left + margin.left;
            rect.width = finalPosition.width - margin.left - margin.right;
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
        case VerticalAlignment::Center: {
            rect.top = finalPosition.top + (finalPosition.height - rect.height - margin.top - margin.bottom) / 2 + margin.top;
            break;
        }
        case VerticalAlignment::Stretch: {
            rect.top    = finalPosition.top + margin.top;
            rect.height = finalPosition.height - margin.top - margin.bottom;
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

    if (this->_parent && this->_parent->_hdwpChildren != NULL) {
        DeferWindowPos(this->_parent->_hdwpChildren, this->Handle, NULL,
                       Dip::DipToPxX(rect.left), Dip::DipToPxY(rect.top),
                       Dip::DipToPxX(rect.width), Dip::DipToPxY(rect.height),
                       SWP_NOACTIVATE | SWP_NOZORDER);
    } else {
        SetWindowPos(this->Handle, NULL,
                     Dip::DipToPxX(rect.left), Dip::DipToPxY(rect.top),
                     Dip::DipToPxX(rect.width), Dip::DipToPxY(rect.height),
                     SWP_NOACTIVATE | SWP_NOZORDER);
    }

    if (this->_children.size() >= 3) {
        this->_hdwpChildren = BeginDeferWindowPos((int)this->_children.size());
    }

    this->ArrangeOverride(this->ClientRect->GetSize());

    if (this->_hdwpChildren != NULL) {
        EndDeferWindowPos(this->_hdwpChildren);
        this->_hdwpChildren = NULL;
    }

    this->_layoutUpdateCondition &= ~sw::LayoutUpdateCondition::Supressed;
}

sw::UIElement *sw::UIElement::ToUIElement()
{
    return this;
}

sw::Size sw::UIElement::MeasureOverride(const Size &availableSize)
{
    // 普通元素测量时，其本身用户区尺寸即为所需尺寸
    // 当元素的对齐方式为拉伸时，返回原始用户区尺寸
    sw::Size desireSize = this->ClientRect->GetSize();

    // 由于Measure中会自动处理边框和Margin而_origionalSize
    // 记录的是原始的窗口尺寸，因此当使用原始尺寸时需要减去边框
    if (this->_horizontalAlignment == HorizontalAlignment::Stretch ||
        this->_verticalAlignment == VerticalAlignment::Stretch) {
        sw::Rect windowRect = this->Rect;
        sw::Size clientSize = desireSize;
        if (this->_horizontalAlignment == HorizontalAlignment::Stretch) {
            desireSize.width = this->_origionalSize.width - (windowRect.width - clientSize.width);
        }
        if (this->_verticalAlignment == VerticalAlignment::Stretch) {
            desireSize.height = this->_origionalSize.height - (windowRect.height - clientSize.height);
        }
    }
    return desireSize;
}

void sw::UIElement::ArrangeOverride(const Size &finalSize)
{
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
        if (handler) {
            handler(*element, eventArgs);
        }
        if (eventArgs.handled) {
            break;
        } else {
            element = element->_parent;
        }
    } while (element != nullptr);
}

double &sw::UIElement::GetArrangeOffsetX()
{
    return this->_arrangeOffsetX;
}

double &sw::UIElement::GetArrangeOffsetY()
{
    return this->_arrangeOffsetY;
}

double sw::UIElement::GetChildRightmost(bool update)
{
    if (update) {
        this->_childRightmost = 0;
        for (UIElement *item : this->_childrenNotCollapsed) {
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
        for (UIElement *item : this->_childrenNotCollapsed) {
            if (item->_float) continue;
            this->_childBottommost = Utils::Max(this->_childBottommost, item->Top + item->Height + item->_margin.bottom - this->_arrangeOffsetY);
        }
    }
    return this->_childBottommost;
}

void sw::UIElement::UpdateChildrenZOrder()
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
}

void sw::UIElement::UpdateSiblingsZOrder()
{
    if (this->_parent != nullptr) {
        this->_parent->UpdateChildrenZOrder();
    }
}

void sw::UIElement::SetNextTabStopFocus()
{
    UIElement *next = this->GetNextTabStopElement();
    if (next && next != this) next->OnTabStop();
}

void sw::UIElement::SetPreviousTabStopFocus()
{
    UIElement *previous = this->GetPreviousTabStopElement();
    if (previous && previous != this) previous->OnTabStop();
}

void sw::UIElement::SetBackColor(Color color, bool redraw)
{
    this->_backColor = color;
    if (redraw) this->Redraw();
}

void sw::UIElement::SetTextColor(Color color, bool redraw)
{
    this->_textColor = color;
    if (redraw) this->Redraw();
}

void sw::UIElement::OnAddedChild(UIElement &element)
{
    if (this->IsLayoutUpdateConditionSet(sw::LayoutUpdateCondition::ChildAdded)) {
        this->InvalidateMeasure();
    }
}

void sw::UIElement::OnRemovedChild(UIElement &element)
{
    if (this->IsLayoutUpdateConditionSet(sw::LayoutUpdateCondition::ChildRemoved)) {
        this->InvalidateMeasure();
    }
}

void sw::UIElement::OnTabStop()
{
    this->Focused = true;
}

bool sw::UIElement::SetParent(WndBase *parent)
{
    UIElement *oldParentElement = this->_parent;
    UIElement *newParentElement = parent ? parent->ToUIElement() : nullptr;

    if (newParentElement == nullptr) {
        /*
         * 要设置的父元素为nullptr
         */
        if (oldParentElement == nullptr) {
            // 在析构函数中会调用SetParent函数以保证自己从父元素的Children中移除
            // 当当前元素为顶级窗口或者不在界面中的控件（已移除/未加入）时会出现该情况
            this->_parent = nullptr;
            return true;
        } else {
            // UIElement虚构函数时调用SerParent函数保证从父元素的Children中移除
            // 当当前元素已经添加到界面中但中途对象被销毁时会出现该情况，调用父元素的RemoveChild函数移除引用
            bool temp = oldParentElement->RemoveChild(this);
            // 当程序将要退出时::SetParent函数可能会失败，此时RemoveChild函数返回false
            // 此时直接将其从父元素的Children中移除
            if (!temp) {
                auto it = std::find(oldParentElement->_children.begin(), oldParentElement->_children.end(), this);
                if (it != oldParentElement->_children.end()) oldParentElement->_children.erase(it);
            }
            this->_parent = nullptr;
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
    this->_parent = newParent ? newParent->ToUIElement() : nullptr;
    this->_SetMeasureInvalidated();
}

bool sw::UIElement::OnClose()
{
    this->SetParent(nullptr);
    return this->WndBase::OnClose();
}

bool sw::UIElement::OnMove(Point newClientPosition)
{
    PositionChangedEventArgs args(newClientPosition);
    this->RaiseRoutedEvent(args);

    if (this->IsLayoutUpdateConditionSet(sw::LayoutUpdateCondition::PositionChanged)) {
        this->InvalidateMeasure();
    }
    return args.handledMsg;
}

bool sw::UIElement::OnSize(Size newClientSize)
{
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
    this->RaiseRoutedEvent(UIElement_TextChanged);

    if (this->IsLayoutUpdateConditionSet(sw::LayoutUpdateCondition::TextChanged)) {
        this->InvalidateMeasure();
    }
}

void sw::UIElement::FontChanged(HFONT hfont)
{
    if (this->IsLayoutUpdateConditionSet(sw::LayoutUpdateCondition::FontChanged)) {
        this->InvalidateMeasure();
    }
}

void sw::UIElement::VisibleChanged(bool newVisible)
{
    if (newVisible || this->_collapseWhenHide) {
        this->InvalidateMeasure();
    }
}

bool sw::UIElement::OnSetFocus(HWND hPrevFocus)
{
    TypedRoutedEventArgs<UIElement_GotFocus> args;
    this->RaiseRoutedEvent(args);
    return args.handledMsg;
}

bool sw::UIElement::OnKillFocus(HWND hNextFocus)
{
    TypedRoutedEventArgs<UIElement_LostFocus> args;
    this->RaiseRoutedEvent(args);
    return args.handledMsg;
}

bool sw::UIElement::OnChar(wchar_t ch, KeyFlags flags)
{
    GotCharEventArgs args(ch, flags);
    this->RaiseRoutedEvent(args);
    return args.handledMsg;
}

bool sw::UIElement::OnKeyDown(VirtualKey key, KeyFlags flags)
{
    KeyDownEventArgs args(key, flags);
    this->RaiseRoutedEvent(args);

    // 实现按下Tab键转移焦点
    if (!args.handledMsg && key == VirtualKey::Tab) {
        bool shiftDown = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
        shiftDown ? this->SetPreviousTabStopFocus() : this->SetNextTabStopFocus();
    }

    return args.handledMsg;
}

bool sw::UIElement::OnKeyUp(VirtualKey key, KeyFlags flags)
{
    KeyUpEventArgs args(key, flags);
    this->RaiseRoutedEvent(args);
    return args.handledMsg;
}

bool sw::UIElement::OnMouseMove(Point mousePosition, MouseKey keyState)
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

bool sw::UIElement::OnMouseWheel(int wheelDelta, Point mousePosition, MouseKey keyState)
{
    MouseWheelEventArgs args(wheelDelta, mousePosition, keyState);
    this->RaiseRoutedEvent(args);
    return args.handledMsg;
}

bool sw::UIElement::OnMouseLeftButtonDown(Point mousePosition, MouseKey keyState)
{
    MouseButtonDownEventArgs args(MouseKey::MouseLeft, mousePosition, keyState);
    this->RaiseRoutedEvent(args);
    return args.handledMsg;
}

bool sw::UIElement::OnMouseLeftButtonUp(Point mousePosition, MouseKey keyState)
{
    MouseButtonUpEventArgs args(MouseKey::MouseLeft, mousePosition, keyState);
    this->RaiseRoutedEvent(args);
    return args.handledMsg;
}

bool sw::UIElement::OnMouseRightButtonDown(Point mousePosition, MouseKey keyState)
{
    MouseButtonDownEventArgs args(MouseKey::MouseRight, mousePosition, keyState);
    this->RaiseRoutedEvent(args);
    return args.handledMsg;
}

bool sw::UIElement::OnMouseRightButtonUp(Point mousePosition, MouseKey keyState)
{
    MouseButtonUpEventArgs args(MouseKey::MouseRight, mousePosition, keyState);
    this->RaiseRoutedEvent(args);
    return args.handledMsg;
}

bool sw::UIElement::OnMouseMiddleButtonDown(Point mousePosition, MouseKey keyState)
{
    MouseButtonDownEventArgs args(MouseKey::MouseMiddle, mousePosition, keyState);
    this->RaiseRoutedEvent(args);
    return args.handledMsg;
}

bool sw::UIElement::OnMouseMiddleButtonUp(Point mousePosition, MouseKey keyState)
{
    MouseButtonUpEventArgs args(MouseKey::MouseMiddle, mousePosition, keyState);
    this->RaiseRoutedEvent(args);
    return args.handledMsg;
}

bool sw::UIElement::OnContextMenu(bool isKeyboardMsg, Point mousePosition)
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
    static HBRUSH hBrush = NULL;
    COLORREF textColor   = this->GetRealTextColor();
    COLORREF backColor   = this->GetRealBackColor();

    ::SetTextColor(hdc, textColor);
    ::SetBkColor(hdc, backColor);

    if (hBrush != NULL) {
        DeleteObject(hBrush);
    }

    hBrush    = CreateSolidBrush(backColor);
    hRetBrush = hBrush;
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

bool sw::UIElement::_SetHorzAlignment(sw::HorizontalAlignment value)
{
    if (value == this->_horizontalAlignment) {
        return false;
    }

    if (value == sw::HorizontalAlignment::Stretch) {
        this->_horizontalAlignment = value;
        this->_origionalSize.width = this->Width;
    } else {
        this->_horizontalAlignment = value;
        this->Width                = this->_origionalSize.width;
    }

    return true;
}

bool sw::UIElement::_SetVertAlignment(sw::VerticalAlignment value)
{
    if (value == this->_verticalAlignment) {
        return false;
    }

    if (value == sw::VerticalAlignment::Stretch) {
        this->_verticalAlignment    = value;
        this->_origionalSize.height = this->Height;
    } else {
        this->_verticalAlignment = value;
        this->Height             = this->_origionalSize.height;
    }

    return true;
}

void sw::UIElement::_SetMeasureInvalidated()
{
    this->_layoutUpdateCondition |= sw::LayoutUpdateCondition::MeasureInvalidated;
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
