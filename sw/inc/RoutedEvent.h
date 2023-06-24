#pragma once

#include <cstdint>
#include <functional>

namespace sw
{
    enum RoutedEventType : uint32_t {
        UserEventBegin = 0,
        UserEventEnd   = 0x80000000,

        /**
         * @brief �������ڹر�
         * @param ָ��һ��boolֵ����ʾ�Ƿ�ȡ�����ιر�
         */
        WindowClosing,

        /**
         * @brief �����ѹر�
         * @param �޲���
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