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
         * @brief 尺寸改变，参数类型为sw::SizeChangedEventArgs
         */
        UIElement_SizeChanged,

        /**
         * @brief 位置改变，参数类型为sw::PositionChangedEventArgs
         */
        UIElement_PositionChanged,

        /**
         * @brief Text属性发生变化，参数类型为sw::RoutedEventArgs
         */
        UIElement_TextChanged,

        /**
         * @brief 获取到焦点，参数类型为sw::RoutedEventArgs
         */
        UIElement_GotFocus,

        /**
         * @brief 失去焦点，参数类型为sw::RoutedEventArgs
         */
        UIElement_LostFocus,

        /**
         * @brief 输入字符，参数类型为sw::GotCharEventArgs
         */
        UIElement_GotChar,

        /**
         * @brief 键盘按键按下，参数类型为sw::KeyDownEventArgs
         */
        UIElement_KeyDown,

        /**
         * @brief 键盘按键抬起，参数类型为sw::KeyUpEventArgs
         */
        UIElement_KeyUp,

        /**
         * @brief 鼠标移动，参数类型为sw::MouseMoveEventArgs
         */
        UIElement_MouseMove,

        /**
         * @brief 鼠标离开，参数类型为sw::RoutedEventArgs
         */
        UIElement_MouseLeave,

        /**
         * @brief 鼠标滚轮滚动，参数类型为sw::MouseWheelEventArgs
         */
        UIElement_MouseWheel,

        /**
         * @brief 鼠标按键按下，参数类型为sw::MouseButtonDownEventArgs
         */
        UIElement_MouseButtonDown,

        /**
         * @brief 鼠标按键抬起，参数类型为sw::MouseButtonUpEventArgs
         */
        UIElement_MouseButtonUp,

        /**
         * @brief 要显示用户自定义的上下文菜单前触发该事件，参数类型为sw::ShowContextMenuEventArgs
         */
        UIElement_ShowContextMenu,

        /**
         * @brief 窗口正在关闭，参数类型为sw::WindowClosingEventArgs
         */
        Window_Closing,

        /**
         * @brief 窗口已关闭，参数类型为sw::RoutedEventArgs
         */
        Window_Closed,

        /**
         * @brief 窗口成为前台窗口，参数类型为sw::RoutedEventArgs
         */
        Window_Actived,

        /**
         * @brief 窗口成为后台窗口，参数类型为sw::RoutedEventArgs
         */
        Window_Inactived,

        /**
         * @brief 按钮被单击，参数类型为sw::RoutedEventArgs
         */
        ButtonBase_Clicked,

        /**
         * @brief 按钮被双击，参数类型为sw::RoutedEventArgs
         */
        ButtonBase_DoubleClicked,

        /**
         * @brief 列表视图/列表框/组合框的选中项改变，参数类型为sw::RoutedEventArgs
         */
        ItemsControl_SelectionChanged,

        /**
         * @brief 列表视图某个复选框的选中状态改变，参数类型为sw::ListViewCheckStateChangedEventArgs
         */
        ListView_CheckStateChanged,

        /**
         * @brief 鼠标左键单击列表视图的列标题，参数类型为sw::ListViewHeaderClickedEventArgs
         */
        ListView_HeaderClicked,

        /**
         * @brief 鼠标左键双击列表视图的列标题，参数类型为sw::ListViewHeaderClickedEventArgs
         */
        ListView_HeaderDoubleClicked,

        /**
         * @brief 鼠标左键单击列表视图某个项，参数类型为sw::ListViewItemClickedEventArgs
         */
        ListView_ItemClicked,

        /**
         * @brief 鼠标左键单击列表视图某个项，参数类型为sw::ListViewItemClickedEventArgs
         */
        ListView_ItemDoubleClicked,

        /**
         * @brief 滑块的值被改变，参数类型为sw::RoutedEventArgs
         */
        Slider_ValueChanged,

        /**
         * @brief 滑块被释放，参数类型为sw::RoutedEventArgs
         */
        Slider_EndTrack,

        /**
         * @brief 窗口/面板滚动条滚动，参数类型为sw::ScrollingEventArgs
         */
        Layer_Scrolling,

        /**
         * @brief SelectedIndex属性被改变，参数类型为sw::RoutedEventArgs
         */
        TabControl_SelectedIndexChanged,
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
         * @brief 表示是否已处理事件所对应的Windows消息，对于部分消息将字段设为true可取消对DefaultWndProc的调用，若当前事件无对应消息则该字段无意义
         */
        bool handledMsg = false;

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
