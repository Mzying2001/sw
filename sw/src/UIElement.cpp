#include "UIElement.h"
#include <algorithm>

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
    // TODO
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
