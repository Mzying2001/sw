#pragma once

#include "RoutedEvent.h"
#include "Thickness.h"
#include "WndBase.h"
#include <map>
#include <string>
#include <vector>

namespace sw
{
    class UIElement : public WndBase
    {
    private:
        Thickness _margin = Thickness();
        std::vector<UIElement *> _children{};
        std::map<RoutedEventType, RoutedEvent> _eventMap{};

    public:
        /**
         * @brief 描述
         */
        std::wstring Description = L"";

        /**
         * @brief 边缘空白大小
         */
        const Property<Thickness> Margin;

        /**
         * @brief 子控件数量
         */
        const ReadOnlyProperty<int> ChildCount;

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

        /**
         * @brief 通知父窗口布局改变
         */
        void NotifyLayoutUpdated();

        /**
         * @brief  添加子控件
         * @return 添加是否成功
         */
        bool AddChild(UIElement *element);

        /**
         * @brief  移除子控件
         * @return 移除是否成功
         */
        bool RemoveChild(UIElement *element);

        /**
         * @brief 通过索引获取子控件
         */
        UIElement *operator[](int index) const;

    protected:
        /**
         * @brief 触发路由事件
         */
        void RaiseRoutedEvent(RoutedEventType eventType, void *param = nullptr);

        /**
         * @brief  设置父窗口
         * @return 设置是否成功
         */
        virtual bool SetParent(WndBase *parent);
    };
}
