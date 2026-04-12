#pragma once

#include "Keys.h"
#include "Point.h"
#include "RoutedEvent.h"
#include "ScrollEnums.h"
#include "Size.h"
#include <type_traits>

namespace sw
{
    // clang-format off

    /**
     * @brief 表示特定类型路由事件的事件参数类型，继承自该类的类型可以直接作为AddHandler函数的模板参数
     * @tparam Type 路由事件类型，必须为RoutedEventType枚举中的值
     * @tparam TBase 事件参数类型的基类，默认为RoutedEventArgs
     */
    template <RoutedEventType Type, typename TBase = RoutedEventArgs>
    struct TypedRoutedEventArgs : TBase
    {
        // TBase必须派生自RoutedEventArgs
        static_assert(
            std::is_base_of<RoutedEventArgs, TBase>::value,
            "TBase must be derived from RoutedEventArgs.");

        /**
         * @brief 路由事件的类型，AddHandler模板函数使用此字段注册事件
         */
        static constexpr RoutedEventType EventType = Type;

        /**
         * @brief 构造函数，初始化事件类型为EventType
         */
        TypedRoutedEventArgs() { this->eventType = EventType; }
    };

    /*================================================================================*/

    /**
     * @brief 结构体模板，用于检测类型T是否含有名为EventType的静态字段
     */
    template <typename T, typename = void>
    struct _HasEventType : std::false_type {
    };

    /**
     * @brief 模板特化：当T包含EventType时，将_IsTypedRoutedEventArgs<T>设为std::true_type
     */
    template <typename T>
    struct _HasEventType<T, decltype(void(T::EventType))> : std::true_type {
    };

    /**
     * @brief 结构体模板，用于检测类型T是否包含事件类型信息
     */
    template <typename T>
    struct _IsTypedRoutedEventArgs : _HasEventType<T> {
    };

    /*================================================================================*/

    /**
     * @brief 尺寸改变事件参数类型
     */
    struct SizeChangedEventArgs : TypedRoutedEventArgs<UIElement_SizeChanged> {
        Size newClientSize; // 用户区的新尺寸
        SizeChangedEventArgs(Size newClientSize) : newClientSize(newClientSize) {}
    };

    /**
     * @brief 位置改变事件参数类型
     */
    struct PositionChangedEventArgs : TypedRoutedEventArgs<UIElement_PositionChanged> {
        Point newClientPosition; // 移动后用户区左上角的位置
        PositionChangedEventArgs(Point newClientPosition) : newClientPosition(newClientPosition) {}
    };

    /**
     * @brief 输入字符事件类型参数
     */
    struct GotCharEventArgs : TypedRoutedEventArgs<UIElement_GotChar> {
        wchar_t ch;     // 输入的字符
        KeyFlags flags; // 附加信息
        GotCharEventArgs(wchar_t ch, KeyFlags flags) : ch(ch), flags(flags) {}
    };

    /**
     * @brief 键盘事件参数类型模板
     * @tparam Type 路由事件类型，必须为RoutedEventType枚举中的值
     */
    template <RoutedEventType Type>
    struct KeyEventArgs : TypedRoutedEventArgs<Type> {
        VirtualKey key; // 虚拟按键
        KeyFlags flags; // 附加信息
        KeyEventArgs(VirtualKey key, KeyFlags flags) : key(key), flags(flags) {}
    };

    /**
     * @brief 键盘按键按下事件参数类型
     */
    struct KeyDownEventArgs : KeyEventArgs<UIElement_KeyDown> {
        using KeyEventArgs<UIElement_KeyDown>::KeyEventArgs;
    };

    /**
     * @brief 键盘按键抬起事件参数类型
     */
    struct KeyUpEventArgs : KeyEventArgs<UIElement_KeyUp> {
        using KeyEventArgs<UIElement_KeyUp>::KeyEventArgs;
    };

    /**
     * @brief 鼠标事件参数类型模板
     * @tparam Type 路由事件类型，必须为RoutedEventType枚举中的值
     */
    template <RoutedEventType Type>
    struct MouseEventArgs : TypedRoutedEventArgs<Type> {
        Point mousePosition; // 鼠标位置
        MouseKey keyState;   // 按键状态
        MouseEventArgs(Point mousePosition, MouseKey keyState) : mousePosition(mousePosition), keyState(keyState) {}
    };

    /**
     * @brief 鼠标移动事件参数类型
     */
    struct MouseMoveEventArgs : MouseEventArgs<UIElement_MouseMove> {
        using MouseEventArgs<UIElement_MouseMove>::MouseEventArgs;
    };

    /**
     * @brief 鼠标滚轮滚动事件参数类型
     */
    struct MouseWheelEventArgs : MouseEventArgs<UIElement_MouseWheel> {
        int wheelDelta; // 滚轮滚动的距离，为120的倍数
        MouseWheelEventArgs(int wheelDelta, Point mousePosition, MouseKey keyState)
            : MouseEventArgs<UIElement_MouseWheel>(mousePosition, keyState), wheelDelta(wheelDelta) {}
    };

    /**
     * @brief 鼠标按键事件参数类型模板
     * @tparam Type 路由事件类型，必须为RoutedEventType枚举中的值
     */
    template <RoutedEventType Type>
    struct MouseButtonEventArgs : MouseEventArgs<Type> {
        MouseKey key; // 按下/抬起的按键（左键、中间、右键）
        MouseButtonEventArgs(MouseKey key, Point mousePosition, MouseKey keyState)
            : MouseEventArgs<Type>(mousePosition, keyState), key(key) {}
    };

    /**
     * @brief 鼠标按键按下事件参数类型
     */
    struct MouseButtonDownEventArgs : MouseButtonEventArgs<UIElement_MouseButtonDown> {
        using MouseButtonEventArgs<UIElement_MouseButtonDown>::MouseButtonEventArgs;
    };

    /**
     * @brief 鼠标按键抬起事件参数类型
     */
    struct MouseButtonUpEventArgs : MouseButtonEventArgs<UIElement_MouseButtonUp> {
        using MouseButtonEventArgs<UIElement_MouseButtonUp>::MouseButtonEventArgs;
    };

    /**
     * @brief 可取消事件参数类型，包含一个cancel字段用于指示是否取消事件
     */
    struct CancelableEventArgs : RoutedEventArgs {
        bool cancel = false; // 是否取消事件，默认为false
    };

    /**
     * @brief 显示用户自定义上下文菜单的事件参数类型
     */
    struct ShowContextMenuEventArgs : TypedRoutedEventArgs<UIElement_ShowContextMenu, CancelableEventArgs> {
        bool isKeyboardMsg;  // 消息是否由按下快捷键（Shift+F10、VK_APPS）产生
        Point mousePosition; // 鼠标在屏幕中的位置
        ShowContextMenuEventArgs(bool isKeyboardMsg, Point mousePosition)
            : isKeyboardMsg(isKeyboardMsg), mousePosition(mousePosition) {}
    };

    /**
     * @brief 文件拖放事件参数类型
     */
    struct DropFilesEventArgs : TypedRoutedEventArgs<UIElement_DropFiles> {
        HDROP hDrop; // 描述拖入文件的句柄
        DropFilesEventArgs(HDROP hDrop) : hDrop(hDrop) {}
    };

    /**
     * @brief 窗口正在关闭事件参数类型
     */
    struct WindowClosingEventArgs : TypedRoutedEventArgs<Window_Closing, CancelableEventArgs> {
    };

    /**
     * @brief 窗口/面板滚动条滚动事件参数类型
     */
    struct ScrollingEventArgs : TypedRoutedEventArgs<Layer_Scrolling, CancelableEventArgs> {
        ScrollOrientation scrollbar; // 滚动条类型
        ScrollEvent event; // 滚动条事件
        double pos;        // 当event为ThumbPosition或ThubmTrack时表示当前滚动条位置，其他情况固定为0
        ScrollingEventArgs(ScrollOrientation scrollbar, ScrollEvent event, double pos)
            : scrollbar(scrollbar), event(event), pos(pos) {}
    };

    /**
     * @brief 列表视图某个项事件参数类型模板，适用于单击、双击等事件
     * @tparam Type 路由事件类型，必须为RoutedEventType枚举中的值
     * @tparam TBase 事件参数类型的基类，默认为RoutedEventArgs
     */
    template <RoutedEventType Type, typename TBase = RoutedEventArgs>
    struct ListViewItemEventArgs : TypedRoutedEventArgs<Type, TBase> {
        int index; // 发生事件的项的索引
        ListViewItemEventArgs(int index) : index(index) {}
    };

    /**
    * @brief 列表视图某个子项事件参数类型模板，适用于单击、双击等事件
    * @tparam Type 路由事件类型，必须为RoutedEventType枚举中的值
    * @tparam TBase 事件参数类型的基类，默认为RoutedEventArgs
    */
    template <RoutedEventType Type, typename TBase = RoutedEventArgs>
    struct ListViewSubItemEventArgs : ListViewItemEventArgs<Type, TBase> {
        int subIndex; // 发生事件的子项的索引
        ListViewSubItemEventArgs(int index, int subIndex)
            : ListViewItemEventArgs<Type, TBase>(index), subIndex(subIndex) {}
    };

    /**
     * @brief 列表视图某个复选框选中状态改变的事件参数类型
     */
    struct ListViewCheckStateChangedEventArgs : ListViewItemEventArgs<ListView_CheckStateChanged> {
        using ListViewItemEventArgs<ListView_CheckStateChanged>::ListViewItemEventArgs;
    };

    /**
     * @brief 列表视图的列标题单击事件参数类型
     */
    struct ListViewHeaderClickedEventArgs : ListViewItemEventArgs<ListView_HeaderClicked> {
        using ListViewItemEventArgs<ListView_HeaderClicked>::ListViewItemEventArgs;
    };

    /**
     * @brief 列表视图的列标题双击事件参数类型
     */
    struct ListViewHeaderDoubleClickedEventArgs : ListViewItemEventArgs<ListView_HeaderDoubleClicked> {
        using ListViewItemEventArgs<ListView_HeaderDoubleClicked>::ListViewItemEventArgs;
    };

    /**
     * @brief 列表视图项单击事件参数类型
     */
    struct ListViewItemClickedEventArgs : ListViewSubItemEventArgs<ListView_ItemClicked> {
        using ListViewSubItemEventArgs<ListView_ItemClicked>::ListViewSubItemEventArgs;
    };

    /**
     * @brief 列表视图项双击事件参数类型
     */
    struct ListViewItemDoubleClickedEventArgs : ListViewSubItemEventArgs<ListView_ItemDoubleClicked> {
        using ListViewSubItemEventArgs<ListView_ItemDoubleClicked>::ListViewSubItemEventArgs;
    };

    /**
     * @brief 列表视图编辑状态结束事件参数类型
     */
    struct ListViewEndEditEventArgs : ListViewItemEventArgs<ListView_EndEdit, CancelableEventArgs> {
        wchar_t *newText; // 新的文本
        ListViewEndEditEventArgs(int index, wchar_t *newText)
            : ListViewItemEventArgs<ListView_EndEdit, CancelableEventArgs>(index), newText(newText) {}
    };

    /**
     * @brief 时间改变事件参数类型模板，适用于DateTimePicker和MonthCalendar控件
     * @tparam Type 路由事件类型，必须为RoutedEventType枚举中的值
     */
    template <RoutedEventType Type>
    struct TimeChangedEventArgs : TypedRoutedEventArgs<Type> {
        SYSTEMTIME time; // 时间的新值
        TimeChangedEventArgs(const SYSTEMTIME &time) : time(time) {}
    };

    /**
     * @brief DateTimePicker控件时间改变事件参数类型
     */
    struct DateTimePickerTimeChangedEventArgs : TimeChangedEventArgs<DateTimePicker_TimeChanged> {
        using TimeChangedEventArgs<DateTimePicker_TimeChanged>::TimeChangedEventArgs;
    };

    /**
     * @brief 月历控件时间改变事件参数类型
     */
    struct MonthCalendarTimeChangedEventArgs : TimeChangedEventArgs<MonthCalendar_TimeChanged> {
        using TimeChangedEventArgs<MonthCalendar_TimeChanged>::TimeChangedEventArgs;
    };

    /**
     * @brief SysLink控件链接被单击事件参数类型
     */
    struct SysLinkClickedEventArgs : TypedRoutedEventArgs<SysLink_Clicked> {
        wchar_t *id;  // 被单击链接的id
        wchar_t *url; // 被单击链接的url（即href）
        SysLinkClickedEventArgs(wchar_t *id, wchar_t *url) : id(id), url(url) {}
    };

    /**
     * @brief 热键框值改变事件参数类型
     */
    struct HotKeyValueChangedEventArgs : TypedRoutedEventArgs<HotKeyControl_ValueChanged> {
        VirtualKey key;          // 按键
        HotKeyModifier modifier; // 辅助按键
        HotKeyValueChangedEventArgs(VirtualKey key, HotKeyModifier modifier) : key(key), modifier(modifier) {}
    };

    /**
     * @brief 分割按钮的下拉箭头单击事件参数类型
     */
    struct SplitButtonDropDownEventArgs : TypedRoutedEventArgs<SplitButton_DropDown, CancelableEventArgs> {
    };

    // clang-format on
}
