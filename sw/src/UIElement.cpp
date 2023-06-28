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
              this->_horizontalAlignment = value;
              this->NotifyLayoutUpdated();
          }),

      VerticalAlignment(
          [&]() -> const sw::VerticalAlignment & {
              return this->_verticalAlignment;
          },
          [&](const sw::VerticalAlignment &value) {
              this->_verticalAlignment = value;
              this->NotifyLayoutUpdated();
          }),

      ChildCount(
          // get
          [&]() -> const int & {
              static int count;
              count = (int)this->_children.size();
              return count;
          })
{
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

void sw::UIElement::NotifyLayoutUpdated()
{
    if (!this->_arranging) {
        // TODO
    }
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
    return true;
}

sw::UIElement *sw::UIElement::operator[](int index) const
{
    return this->_children[index];
}

int sw::UIElement::GetChildLayoutCount()
{
    return (int)this->_children.size();
}

sw::ILayout &sw::UIElement::GetChildLayoutAt(int index)
{
    return *this->_children[index];
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

    this->SetDesireSize(Size(
        rect.width + margin.left + margin.right,
        rect.height + margin.top + margin.bottom));
}

void sw::UIElement::Arrange(const sw::Rect &finalPosition)
{
    this->_arranging  = true;
    sw::Rect rect     = this->Rect;
    Thickness &margin = this->_margin;

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
            rect.left = finalPosition.left + finalPosition.width - rect.width - margin.left - margin.right;
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
            rect.top = finalPosition.top + finalPosition.height - rect.height - margin.top - margin.bottom;
            break;
        }
    }

    rect.width       = max(rect.width, 0);
    rect.height      = max(rect.height, 0);
    this->Rect       = rect;
    this->_arranging = false;
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
            element = dynamic_cast<UIElement *>(element->Parent.Get());
        }
    } while (element != nullptr);
}

bool sw::UIElement::SetParent(WndBase *parent)
{
    UIElement *oldParentElement = dynamic_cast<UIElement *>(this->Parent.Get());
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
