#pragma once

#include "Keys.h"
#include "Point.h"
#include "RoutedEvent.h"
#include "Size.h"

namespace sw
{
    struct RoutedEventArgs; // RoutedEvent.h

    /**
     * @brief    表示特定类型路由事件的事件参数类型，继承自该类的事件参数可以用于RegisterRoutedEvent模板函数
     * @tparam T 一个RoutedEventType枚举值，表示路由事件类型
     */
    template <RoutedEventType T>
    struct RoutedEventArgsOfType : RoutedEventArgs {
        static constexpr RoutedEventType EventType = T;
        RoutedEventArgsOfType()
            : RoutedEventArgs(EventType)
        {
        }
    };

    /**
     * @brief 结构体模板，用于检测类型T是否含有名为EventType的静态字段
     */
    template <typename T, typename = void>
    struct _IsTypedRoutedEventArgs : std::false_type {
    };

    /**
     * @brief 模板特化：当T包含EventType时，将_IsTypedRoutedEventArgs<T>设为std::true_type
     */
    template <typename T>
    struct _IsTypedRoutedEventArgs<T, std::void_t<decltype(T::EventType)>> : std::true_type {
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
