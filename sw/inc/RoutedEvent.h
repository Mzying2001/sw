#pragma once

#include "Delegate.h"
#include <cstdint>

namespace sw
{
    /**
     * @brief 路由事件类型枚举
     */
    enum RoutedEventType : uint32_t {
        // 从该值开始到RoutedEventType_UserEnd结束表示用户可以自定义路由事件的值范围
        RoutedEventType_User = 0,

        // 用户自定义路由事件的值的最大值
        RoutedEventType_UserEnd = 0x80000000,

        // 尺寸改变，参数类型为sw::SizeChangedEventArgs
        UIElement_SizeChanged,

        // 位置改变，参数类型为sw::PositionChangedEventArgs
        UIElement_PositionChanged,

        // Text属性发生变化，参数类型为sw::RoutedEventArgs
        UIElement_TextChanged,

        // 获取到焦点，参数类型为sw::RoutedEventArgs
        UIElement_GotFocus,

        // 失去焦点，参数类型为sw::RoutedEventArgs
        UIElement_LostFocus,

        // 输入字符，参数类型为sw::GotCharEventArgs
        UIElement_GotChar,

        // 键盘按键按下，参数类型为sw::KeyDownEventArgs
        UIElement_KeyDown,

        // 键盘按键抬起，参数类型为sw::KeyUpEventArgs
        UIElement_KeyUp,

        // 鼠标移动，参数类型为sw::MouseMoveEventArgs
        UIElement_MouseMove,

        // 鼠标离开，参数类型为sw::RoutedEventArgs
        UIElement_MouseLeave,

        // 鼠标滚轮滚动，参数类型为sw::MouseWheelEventArgs
        UIElement_MouseWheel,

        // 鼠标按键按下，参数类型为sw::MouseButtonDownEventArgs
        UIElement_MouseButtonDown,

        // 鼠标按键抬起，参数类型为sw::MouseButtonUpEventArgs
        UIElement_MouseButtonUp,

        // 要显示用户自定义的上下文菜单前触发该事件，参数类型为sw::ShowContextMenuEventArgs
        UIElement_ShowContextMenu,

        // 接收到文件拖放，参数类型为sw::DropFilesEventArgs
        UIElement_DropFiles,

        // 窗口正在关闭，参数类型为sw::WindowClosingEventArgs
        Window_Closing,

        // 窗口已关闭，参数类型为sw::RoutedEventArgs
        Window_Closed,

        // 窗口成为前台窗口，参数类型为sw::RoutedEventArgs
        Window_Actived,

        // 窗口成为后台窗口，参数类型为sw::RoutedEventArgs
        Window_Inactived,

        // 按钮被单击，参数类型为sw::RoutedEventArgs
        ButtonBase_Clicked,

        // 按钮被双击，参数类型为sw::RoutedEventArgs
        ButtonBase_DoubleClicked,

        // 分割按钮的下拉箭头被单击，参数类型为sw::SplitButtonDropDownEventArgs
        SplitButton_DropDown,

        // 列表视图/列表框/组合框的选中项改变，参数类型为sw::RoutedEventArgs
        ItemsControl_SelectionChanged,

        // 列表视图某个复选框的选中状态改变，参数类型为sw::ListViewCheckStateChangedEventArgs
        ListView_CheckStateChanged,

        // 鼠标左键单击列表视图的列标题，参数类型为sw::ListViewHeaderClickedEventArgs
        ListView_HeaderClicked,

        // 鼠标左键双击列表视图的列标题，参数类型为sw::ListViewHeaderDoubleClickedEventArgs
        ListView_HeaderDoubleClicked,

        // 鼠标左键单击列表视图某个项，参数类型为sw::ListViewItemClickedEventArgs
        ListView_ItemClicked,

        // 鼠标左键单击列表视图某个项，参数类型为sw::ListViewItemDoubleClickedEventArgs
        ListView_ItemDoubleClicked,

        // 编辑状态结束，参数类型为sw::ListViewEndEditEventArgs
        ListView_EndEdit,

        // 滑块的值被改变，参数类型为sw::RoutedEventArgs
        Slider_ValueChanged,

        // 滑块被释放，参数类型为sw::RoutedEventArgs
        Slider_EndTrack,

        // 窗口/面板滚动条滚动，参数类型为sw::ScrollingEventArgs
        Layer_Scrolling,

        // SelectedIndex属性被改变，参数类型为sw::RoutedEventArgs
        TabControl_SelectedIndexChanged,

        // DateTimePicker控件的时间改变，参数类型为sw::DateTimePickerTimeChangedEventArgs
        DateTimePicker_TimeChanged,

        // 月历控件的时间改变，参数类型为sw::MonthCalendarTimeChangedEventArgs
        MonthCalendar_TimeChanged,

        // IP地址框地址被改变，参数类型为sw::RoutedEventArgs
        IPAddressControl_AddressChanged,

        // SysLink控件链接被单击，参数类型为sw::SysLinkClickedEventArgs
        SysLink_Clicked,

        // 热键框的值被改变，参数类型为sw::HotKeyValueChangedEventArgs
        HotKeyControl_ValueChanged,

        // 树视图节点正在展开或折叠，参数类型为sw::TreeViewItemExpandingEventArgs
        TreeView_ItemExpanding,

        // 树视图节点已展开或折叠，参数类型为sw::TreeViewItemExpandedEventArgs
        TreeView_ItemExpanded,

        // 树视图节点的复选框选中状态被改变，参数类型为sw::TreeViewCheckStateChangedEventArgs
        TreeView_CheckStateChanged,
    };

    /*================================================================================*/

    class UIElement; // UIElement.h

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
         * @brief 事件源，指向触发当前事件的UIElement
         */
        UIElement *source = nullptr;

        /**
         * @brief 原始事件源，指向最初触发事件的UIElement
         */
        UIElement *originalSource = nullptr;

        /**
         * @brief RoutedEventArgs构造函数
         */
        RoutedEventArgs(RoutedEventType eventType);
    };

    /**
     * @brief 路由事件类型
     * @note  第一个参数为注册事件监听器的元素，第二个参数为具体的事件参数
     */
    using RoutedEventHandler = Action<UIElement &, RoutedEventArgs &>;
}
