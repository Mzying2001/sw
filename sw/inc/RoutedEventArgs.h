#pragma once

#include "Keys.h"
#include "Point.h"
#include "RoutedEvent.h"
#include "ScrollEnums.h"
#include "Size.h"
#include <type_traits>

namespace sw
{
    struct RoutedEventArgs; // RoutedEvent.h

    /**
     * @brief       表示特定类型路由事件的事件参数类型，继承自该类的事件参数可以用于RegisterRoutedEvent模板函数
     * @tparam TYPE 一个RoutedEventType枚举值，表示路由事件类型
     */
    template <RoutedEventType TYPE>
    struct RoutedEventArgsOfType : RoutedEventArgs {

        /**
         * @brief 路由事件的类型，RegisterRoutedEvent模板函数使用此字段注册事件
         */
        static constexpr RoutedEventType EventType = TYPE;

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
    struct _IsTypedRoutedEventArgs<T, decltype(void(std::declval<T>().EventType))> : std::true_type {
    };

    /**
     * @brief 尺寸改变事件参数类型
     */
    struct SizeChangedEventArgs : RoutedEventArgsOfType<UIElement_SizeChanged> {

        Size newClientSize; // 用户区的新尺寸

        SizeChangedEventArgs(Size newClientSize)
            : newClientSize(newClientSize)
        {
        }
    };

    /**
     * @brief 位置改变事件参数类型
     */
    struct PositionChangedEventArgs : RoutedEventArgsOfType<UIElement_PositionChanged> {

        Point newClientPosition; // 移动后用户区左上角的位置

        PositionChangedEventArgs(Point newClientPosition)
            : newClientPosition(newClientPosition)
        {
        }
    };

    /**
     * @brief 输入字符事件类型参数
     */
    struct GotCharEventArgs : RoutedEventArgsOfType<UIElement_GotChar> {

        wchar_t ch;     // 输入的字符
        KeyFlags flags; // 附加信息

        GotCharEventArgs(wchar_t ch, KeyFlags flags)
            : ch(ch), flags(flags)
        {
        }
    };

    /**
     * @brief 键盘按键按下事件参数类型
     */
    struct KeyDownEventArgs : RoutedEventArgsOfType<UIElement_KeyDown> {

        VirtualKey key; // 虚拟按键
        KeyFlags flags; // 附加信息

        KeyDownEventArgs(VirtualKey key, KeyFlags flags)
            : key(key), flags(flags)
        {
        }
    };

    /**
     * @brief 键盘按键抬起事件参数类型
     */
    struct KeyUpEventArgs : RoutedEventArgsOfType<UIElement_KeyUp> {

        VirtualKey key; // 虚拟按键
        KeyFlags flags; // 附加信息

        KeyUpEventArgs(VirtualKey key, KeyFlags flags)
            : key(key), flags(flags)
        {
        }
    };

    /**
     * @brief 鼠标移动事件参数类型
     */
    struct MouseMoveEventArgs : RoutedEventArgsOfType<UIElement_MouseMove> {

        Point mousePosition; // 鼠标位置
        MouseKey keyState;   // 按键状态

        MouseMoveEventArgs(Point mousePosition, MouseKey keyState)
            : mousePosition(mousePosition), keyState(keyState)
        {
        }
    };

    /**
     * @brief 鼠标滚轮滚动事件参数类型
     */
    struct MouseWheelEventArgs : RoutedEventArgsOfType<UIElement_MouseWheel> {

        int wheelDelta;      // 滚轮滚动的距离，为120的倍数
        Point mousePosition; // 鼠标位置
        MouseKey keyState;   // 按键状态

        MouseWheelEventArgs(int wheelDelta, Point mousePosition, MouseKey keyState)
            : wheelDelta(wheelDelta), mousePosition(mousePosition), keyState(keyState)
        {
        }
    };

    /**
     * @brief 鼠标按键按下事件参数类型
     */
    struct MouseButtonDownEventArgs : RoutedEventArgsOfType<UIElement_MouseButtonDown> {

        MouseKey key;        // 按下的按键（左键、中间、右键）
        Point mousePosition; // 鼠标位置
        MouseKey keyState;   // 按键状态

        MouseButtonDownEventArgs(MouseKey key, Point mousePosition, MouseKey keyState)
            : key(key), mousePosition(mousePosition), keyState(keyState)
        {
        }
    };

    /**
     * @brief 鼠标按键抬起事件参数类型
     */
    struct MouseButtonUpEventArgs : RoutedEventArgsOfType<UIElement_MouseButtonUp> {

        MouseKey key;        // 抬起的按键（左键、中间、右键）
        Point mousePosition; // 鼠标位置
        MouseKey keyState;   // 按键状态

        MouseButtonUpEventArgs(MouseKey key, Point mousePosition, MouseKey keyState)
            : key(key), mousePosition(mousePosition), keyState(keyState)
        {
        }
    };

    /**
     * @brief 显示用户自定义上下文菜单的事件参数类型
     */
    struct ShowContextMenuEventArgs : RoutedEventArgsOfType<UIElement_ShowContextMenu> {

        bool cancel = false; // 是否取消显示上下文菜单
        bool isKeyboardMsg;  // 消息是否由按下快捷键（Shift+F10、VK_APPS）产生
        Point mousePosition; // 鼠标在屏幕中的位置

        ShowContextMenuEventArgs(bool isKeyboardMsg, Point mousePosition)
            : isKeyboardMsg(isKeyboardMsg), mousePosition(mousePosition)
        {
        }
    };

    /**
     * @brief 窗口正在关闭事件参数类型
     */
    struct WindowClosingEventArgs : RoutedEventArgsOfType<Window_Closing> {
        bool cancel = false; // 是否取消本次关闭
    };

    /**
     * @brief 窗口/面板滚动条滚动事件参数类型
     */
    struct ScrollingEventArgs : RoutedEventArgsOfType<Layer_Scrolling> {

        bool cancel = false;         // 是否取消滚动条默认行为
        ScrollOrientation scrollbar; // 滚动条类型
        ScrollEvent event;           // 滚动条事件
        double pos;                  // 当event为ThumbPosition或ThubmTrack时表示当前滚动条位置，其他情况固定为0

        ScrollingEventArgs(ScrollOrientation scrollbar, ScrollEvent event, double pos)
            : scrollbar(scrollbar), event(event), pos(pos)
        {
        }
    };
}
