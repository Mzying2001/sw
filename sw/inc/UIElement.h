#pragma once

#include <map>
#include <string>
#include "WndBase.h"
#include "RoutedEvent.h"

namespace sw
{
    class UIElement : public WndBase
    {
    private:
        std::map<RoutedEventType, RoutedEvent> _eventMap{};

    public:
        std::wstring Description = L"";

    public:
        UIElement();

        /**
         * @brief           注册路由事件处理函数，当事件已注册时会覆盖已注册的函数
         * @param eventType 路由事件类型
         * @param event     处理函数，当值为nullptr时可取消注册
         */
        void RegisterRoutedEvent(RoutedEventType eventType, const RoutedEvent &event);

        /**
         * @brief           取消对应类型路由事件的注册
         * @param eventType 路由事件类型
         */
        void UnregisterRoutedEvent(RoutedEventType eventType);

        /**
         * @brief           判断路由事件是否已被注册
         * @param eventType 路由事件类型
         */
        bool IsRoutedEventRegistered(RoutedEventType eventType);
    };
}
