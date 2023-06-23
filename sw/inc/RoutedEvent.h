#pragma once

#include <cstdint>
#include <functional>

namespace sw
{
    enum RoutedEventType : uint32_t {
        UserEventBegin = 0,
        UserEventEnd   = 0x80000000,
    };

    struct RoutedEventArgs {
        RoutedEventType eventType;
        void *param;
        bool handled;
        RoutedEventArgs(RoutedEventType eventType, void *param = nullptr, bool handled = false);
    };

    class UIElement; // UIElement.h
    typedef std::function<void(UIElement &, RoutedEventArgs &)> RoutedEvent;
}
