#pragma once

#include <cstdint>
#include <functional>

namespace sw
{
    enum RoutedEventType : uint32_t {
        UserEventBegin = 0,
        UserEventEnd   = 0x80000000,

        /**
         * @brief 窗口正在关闭
         * @param 指向一个bool值，表示是否取消本次关闭
         */
        WindowClosing,

        /**
         * @brief 窗口已关闭
         * @param 无参数
         */
        WindowClosed,
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
