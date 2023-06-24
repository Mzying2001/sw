#include "UIElement.h"

sw::UIElement::UIElement()
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
