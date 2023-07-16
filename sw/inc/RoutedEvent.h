#pragma once

#include <cstdint>
#include <functional>

namespace sw
{
    /**
     * @brief 路由事件类型枚举
     */
    enum RoutedEventType : uint32_t {
        UserEventBegin = 0,
        UserEventEnd   = 0x80000000,

        /**
         * @brief 尺寸改变，param指向一个sw::Size结构体，表示用户区的新尺寸
         */
        UIElement_SizeChanged,

        /**
         * @brief 位置改变，param指向一个sw::Point结构体，表示用户区的新位置
         */
        UIElement_PositionChanged,

        /**
         * @brief Text属性发生变化，param指向一个std::wstring，表示新的文本
         */
        UIElement_TextChanged,

        /**
         * @brief 获取到焦点，该事件无参数
         */
        UIElement_GotFocus,

        /**
         * @brief 失去焦点，该事件无参数
         */
        UIElement_LostFocus,

        /**
         * @brief 窗口正在关闭，param指向一个bool值，表示是否取消本次关闭
         */
        Window_Closing,

        /**
         * @brief 窗口已关闭，该事件无参数
         */
        Window_Closed,

        /**
         * @brief 按钮被单击，该事件无参数
         */
        ButtonBase_Clicked,

        /**
         * @brief 按钮被双击，该事件无参数
         */
        ButtonBase_DoubleClicked,
    };

    /*================================================================================*/

    /**
     * @brief 路由事件的参数
     */
    struct RoutedEventArgs {
        /**
         * @brief 事件类型
         */
        RoutedEventType eventType;

        /**
         * @brief 事件是否已被处理，若将此字段设为true，则事件不会继续往上传递
         */
        bool handled = false;

        /**
         * @brief RoutedEventArgs构造函数
         */
        RoutedEventArgs(RoutedEventType eventType);
    };

    class UIElement; // UIElement.h
    /**
     * @brief 路由事件类型
     */
    typedef std::function<void(UIElement &, RoutedEventArgs &)> RoutedEvent;
}
