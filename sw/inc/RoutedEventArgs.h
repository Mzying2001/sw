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
        SizeChangedEventArgs(Size newClientSize);
    };

    /**
     * @brief 位置改变事件参数类型
     */
    struct PositionChangedEventArgs : RoutedEventArgsOfType<UIElement_PositionChanged> {
        Point newClientPosition; // 移动后用户区左上角的位置
        PositionChangedEventArgs(Point newClientPosition);
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
     * @brief 鼠标移动事件参数类型
     */
    struct MouseMoveEventArgs : RoutedEventArgsOfType<UIElement_MouseMove> {
        Point mousePosition; // 鼠标位置
        MouseKey keyState;   // 按键状态
        MouseMoveEventArgs(Point mousePosition, MouseKey keyState);
    };

    /**
     * @brief 鼠标滚轮滚动事件参数类型
     */
    struct MouseWheelEventArgs : RoutedEventArgsOfType<UIElement_MouseWheel> {
        int wheelDelta;      // 滚轮滚动的距离，为120的倍数
        Point mousePosition; // 鼠标位置
        MouseKey keyState;   // 按键状态
        MouseWheelEventArgs(int wheelDelta, Point mousePosition, MouseKey keyState);
    };

    /**
     * @brief 鼠标按键按下事件参数类型
     */
    struct MouseButtonDownEventArgs : RoutedEventArgsOfType<UIElement_MouseButtonDown> {
        MouseKey key;        // 按下的按键（左键、中间、右键）
        Point mousePosition; // 鼠标位置
        MouseKey keyState;   // 按键状态
        MouseButtonDownEventArgs(MouseKey key, Point mousePosition, MouseKey keyState);
    };

    /**
     * @brief 鼠标按键抬起事件参数类型
     */
    struct MouseButtonUpEventArgs : RoutedEventArgsOfType<UIElement_MouseButtonUp> {
        MouseKey key;        // 抬起的按键（左键、中间、右键）
        Point mousePosition; // 鼠标位置
        MouseKey keyState;   // 按键状态
        MouseButtonUpEventArgs(MouseKey key, Point mousePosition, MouseKey keyState);
    };

    /**
     * @brief 窗口正在关闭事件参数类型
     */
    struct WindowClosingEventArgs : RoutedEventArgsOfType<Window_Closing> {
        bool cancel = false; // 是否取消本次关闭
    };
}
