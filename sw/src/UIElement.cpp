#include "UIElement.h"
#include <algorithm>
#include <cmath>

sw::UIElement::UIElement()
    : Margin(
          // get
          [&]() -> const Thickness & {
              return this->_margin;
          },
          // set
          [&](const Thickness &value) {
              this->_margin = value;
              this->NotifyLayoutUpdated();
          }),

      HorizontalAlignment(
          [&]() -> const sw::HorizontalAlignment & {
              return this->_horizontalAlignment;
          },
          [&](const sw::HorizontalAlignment &value) {
              if (value == this->_horizontalAlignment) {
                  return;
              }
              if (value == sw::HorizontalAlignment::Stretch) {
                  this->_horizontalAlignment = value;
                  this->_origionalSize.width = this->Width;
              } else {
                  this->_horizontalAlignment = value;
                  this->Width                = this->_origionalSize.width;
              }
              this->NotifyLayoutUpdated();
          }),

      VerticalAlignment(
          [&]() -> const sw::VerticalAlignment & {
              return this->_verticalAlignment;
          },
          [&](const sw::VerticalAlignment &value) {
              if (value == this->_verticalAlignment) {
                  return;
              }
              if (value == sw::VerticalAlignment::Stretch) {
                  this->_verticalAlignment    = value;
                  this->_origionalSize.height = this->Height;
              } else {
                  this->_verticalAlignment = value;
                  this->Height             = this->_origionalSize.height;
              }
              this->NotifyLayoutUpdated();
          }),

      ChildCount(
          // get
          [&]() -> const int & {
              static int count;
              count = (int)this->_children.size();
              return count;
          }),

      CollapseWhenHide(
          // get
          [&]() -> const bool & {
              return this->_collapseWhenHide;
          },
          // set
          [&](const bool &value) {
              if (this->_collapseWhenHide != value) {
                  this->_collapseWhenHide = value;
                  if (!this->Visible)
                      this->NotifyLayoutUpdated();
              }
          }),

      Parent(
          // get
          [&]() -> UIElement *const & {
              return this->_parent;
          })
{
}

sw::UIElement::~UIElement()
{
    this->SetParent(nullptr);
}

void sw::UIElement::RegisterRoutedEvent(RoutedEventType eventType, const RoutedEvent &event)
{
    if (event) {
        this->_eventMap[eventType] = event;
    } else {
        this->UnregisterRoutedEvent(eventType);
    }
}

void sw::UIElement::UnregisterRoutedEvent(RoutedEventType eventType)
{
    if (this->IsRoutedEventRegistered(eventType))
        this->_eventMap.erase(eventType);
}

bool sw::UIElement::IsRoutedEventRegistered(RoutedEventType eventType)
{
    return this->_eventMap.count(eventType);
}

bool sw::UIElement::AddChild(UIElement *element)
{
    if (element == nullptr) {
        return false;
    }

    if (std::find(this->_children.begin(), this->_children.end(), element) != this->_children.end()) {
        return false;
    }

    if (!element->WndBase::SetParent(this)) {
        return false;
    }

    element->SetStyle(WS_CHILD, true);
    this->_children.push_back(element);
    this->NotifyLayoutUpdated();
    return true;
}

bool sw::UIElement::AddChild(UIElement &element)
{
    return this->AddChild(&element);
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
    this->NotifyLayoutUpdated();
    return true;
}

bool sw::UIElement::RemoveChild(UIElement &element)
{
    return this->RemoveChild(&element);
}

void sw::UIElement::Clear()
{
    for (UIElement *item : this->_children) {
        item->WndBase::SetParent(nullptr);
    }
    this->_children.clear();
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

sw::UIElement &sw::UIElement::operator[](int index) const
{
    return *this->_children[index];
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

void sw::UIElement::SetDesireSize(const Size &size)
{
    this->_desireSize = size;
}

void sw::UIElement::Measure(const Size &availableSize)
{
    sw::Rect rect     = this->Rect;
    Thickness &margin = this->_margin;

    if (this->_horizontalAlignment == HorizontalAlignment::Stretch) {
        rect.width = this->_origionalSize.width;
    }
    if (this->_verticalAlignment == VerticalAlignment::Stretch) {
        rect.height = this->_origionalSize.height;
    }

    this->SetDesireSize(Size(
        rect.width + margin.left + margin.right,
        rect.height + margin.top + margin.bottom));
}

void sw::UIElement::Arrange(const sw::Rect &finalPosition)
{
    this->_arranging  = true;
    Size &desireSize  = this->_desireSize;
    Thickness &margin = this->_margin;

    sw::Rect rect = this->Rect;
    rect.width    = desireSize.width - margin.left - margin.right;
    rect.height   = desireSize.height - margin.top - margin.bottom;

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

    // AbsoluteLayout特殊处理：用nan表示不需要调整位置
    if (std::isnan(finalPosition.left) || std::isnan(finalPosition.top)) {
        rect.left = this->Left;
        rect.top  = this->Top;
    }

    rect.width       = max(rect.width, 0);
    rect.height      = max(rect.height, 0);
    this->Rect       = rect;
    this->_arranging = false;

    this->Redraw();
}

void sw::UIElement::RaiseRoutedEvent(RoutedEventType eventType, void *param)
{
    UIElement *element = this;
    RoutedEventArgs args(eventType, param);
    do {
        if (element->IsRoutedEventRegistered(eventType)) {
            element->_eventMap[eventType](*this, args);
        }
        if (args.handled) {
            break;
        } else {
            element = element->_parent;
        }
    } while (element != nullptr);
}

sw::UIElement &sw::UIElement::GetRootElement()
{
    UIElement *root;
    UIElement *element = this;
    do {
        root    = element;
        element = element->_parent;
    } while (element != nullptr);
    return *root;
}

bool sw::UIElement::IsRootElement()
{
    return this->_parent == nullptr;
}

void sw::UIElement::NotifyLayoutUpdated()
{
    if (!this->_arranging) {
        UIElement &root = this->GetRootElement();
        SendMessageW(root.Handle, WM_UpdateLayout, NULL, NULL);
    }
}

bool sw::UIElement::SetParent(WndBase *parent)
{
    UIElement *oldParentElement = this->_parent;
    UIElement *newParentElement = dynamic_cast<UIElement *>(parent);

    if (newParentElement == nullptr) {
        if (oldParentElement == nullptr) {
            return this->WndBase::SetParent(parent);
        } else {
            return oldParentElement->RemoveChild(this) &&
                   this->WndBase::SetParent(parent);
        }
    } else {
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
    this->_parent = dynamic_cast<UIElement *>(newParent);
}

bool sw::UIElement::OnClose()
{
    this->SetParent(nullptr);
    return this->WndBase::OnClose();
}

bool sw::UIElement::OnMove(double newClientLeft, double newClientTop)
{
    Point newClientPos(newClientLeft, newClientTop);
    this->RaiseRoutedEvent(UIElement_PositionChanged, &newClientPos);
    return false;
}

bool sw::UIElement::OnSize(double newClientWidth, double newClientHeight)
{
    if (this->_horizontalAlignment != sw::HorizontalAlignment::Stretch) {
        this->_origionalSize.width = this->Width;
    }
    if (this->_verticalAlignment != sw::VerticalAlignment::Stretch) {
        this->_origionalSize.height = this->Height;
    }

    Size newClientSize(newClientWidth, newClientHeight);
    this->RaiseRoutedEvent(UIElement_SizeChanged, &newClientSize);

    this->NotifyLayoutUpdated();
    return true;
}

void sw::UIElement::OnTextChanged(const std::wstring &newText)
{
    this->RaiseRoutedEvent(UIElement_TextChanged, const_cast<std::wstring *>(&newText));
}

void sw::UIElement::VisibleChanged(bool newVisible)
{
    if (newVisible || this->_collapseWhenHide) {
        this->NotifyLayoutUpdated();
    }
}

bool sw::UIElement::OnSetFocus(HWND hPreFocus)
{
    this->RaiseRoutedEvent(UIElement_GotFocus);
    return true;
}

bool sw::UIElement::OnKillFocus(HWND hNextFocus)
{
    this->RaiseRoutedEvent(UIElement_LostFocus);
    return true;
}
