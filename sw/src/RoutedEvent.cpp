#include "RoutedEvent.h"

sw::RoutedEventArgs::RoutedEventArgs(RoutedEventType eventType, void *param, bool handled)
    : eventType(eventType), param(param), handled(handled)
{
}
