#pragma once

#include "Keys.h"
#include "Point.h"
#include "RoutedEvent.h"
#include "Size.h"

namespace sw
{
    struct RoutedEventArgs; // RoutedEvent.h

    template <RoutedEventType EventType>
    struct RoutedEventArgsOfType : RoutedEventArgs {
        static const RoutedEventType TYPE = EventType;
        inline RoutedEventArgsOfType()
            : RoutedEventArgs(EventType)
        {
        }
    };

    /**
     * @brief 尺寸改变事件参数类型
     */
    struct SizeChangedEventArgs : RoutedEventArgsOfType<UIElement_SizeChanged> {
        Size newClientSize; // 用户区的新尺寸
        SizeChangedEventArgs(const Size &newClientSize);
    };

    /**
     * @brief 位置改变事件参数类型
     */
    struct PositionChangedEventArgs : RoutedEventArgsOfType<UIElement_PositionChanged> {
        Point newClientPosition; // 移动后用户区左上角的位置
        PositionChangedEventArgs(const Point &newClientPosition);
    };

    /**
     * @brief Text属性改变事件参数类型
     */
    struct TextChangedEventArgs : RoutedEventArgsOfType<UIElement_TextChanged> {
        const wchar_t *newText; // 改变后的Text
        TextChangedEventArgs(const wchar_t *newText);
    };

    /**
     * @brief 输入字符事件类型参数
     */
    struct GotCharEventArgs : RoutedEventArgsOfType<UIElement_GotChar> {
        wchar_t ch;     // 输入的字符
        KeyFlags flags; // 附加信息
        GotCharEventArgs(wchar_t ch, KeyFlags flags);
    };

    /**
     * @brief 键盘按键按下事件参数类型
     */
    struct KeyDownEventArgs : RoutedEventArgsOfType<UIElement_KeyDown> {
        VirtualKey key; // 虚拟按键
        KeyFlags flags; // 附加信息
        KeyDownEventArgs(VirtualKey key, KeyFlags falgs);
    };

    /**
     * @brief 键盘按键抬起事件参数类型
     */
    struct KeyUpEventArgs : RoutedEventArgsOfType<UIElement_KeyUp> {
        VirtualKey key; // 虚拟按键
        KeyFlags flags; // 附加信息
        KeyUpEventArgs(VirtualKey key, KeyFlags falgs);
    };

    /**
     * @brief 窗口正在关闭事件参数类型
     */
    struct WindowClosingEventArgs : RoutedEventArgsOfType<Window_Closing> {
        bool cancel = false; // 是否取消本次关闭
    };
}
