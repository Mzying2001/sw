#pragma once

#include "Keys.h"
#include "Point.h"
#include "RoutedEvent.h"
#include "ScrollEnums.h"
#include "Size.h"
#include <type_traits>

namespace sw
{
    /**
     * @brief 表示特定类型路由事件的事件参数类型，继承自该类的类型可以直接作为AddHandler函数的模板参数
     * @tparam Type 路由事件类型，必须为RoutedEventType枚举中的值
     * @tparam TBase 事件参数类型的基类，默认为RoutedEventArgs
     */
    template <RoutedEventType Type, typename TBase = RoutedEventArgs>
    class TypedRoutedEventArgs : public TBase
    {
        // TBase必须派生自RoutedEventArgs
        static_assert(
            std::is_base_of<RoutedEventArgs, TBase>::value,
            "TBase must be derived from RoutedEventArgs.");

    public:
        /// @brief 路由事件的类型，AddHandler模板函数使用此字段注册事件
        static constexpr RoutedEventType EventType = Type;

        /// @brief 构造函数，初始化事件类型为EventType
        TypedRoutedEventArgs()
        { this->eventType = EventType; }
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
    class SizeChangedEventArgs : public TypedRoutedEventArgs<UIElement_SizeChanged>
    {
    public:
        /// @brief 用户区的新尺寸
        Size newClientSize;

        /// @brief 构造函数
        /// @param newClientSize 用户区的新尺寸
        SizeChangedEventArgs(Size newClientSize) : newClientSize(newClientSize)
        {
        }
    };

    /**
     * @brief 位置改变事件参数类型
     */
    class PositionChangedEventArgs : public TypedRoutedEventArgs<UIElement_PositionChanged>
    {
    public:
        /// @brief 移动后用户区左上角的位置
        Point newClientPosition;

        /// @brief 构造函数
        /// @param newClientPosition 移动后用户区左上角的位置
        PositionChangedEventArgs(Point newClientPosition) : newClientPosition(newClientPosition)
        {
        }
    };

    /**
     * @brief 输入字符事件类型参数
     */
    class GotCharEventArgs : public TypedRoutedEventArgs<UIElement_GotChar>
    {
    public:
        /// @brief 输入的字符
        wchar_t ch;

        /// @brief 附加信息
        KeyFlags flags;

        /// @brief 构造函数
        /// @param ch 输入的字符
        /// @param flags 附加信息
        GotCharEventArgs(wchar_t ch, KeyFlags flags) : ch(ch), flags(flags)
        {
        }
    };

    /**
     * @brief 键盘事件参数类型模板
     * @tparam Type 路由事件类型，必须为RoutedEventType枚举中的值
     */
    template <RoutedEventType Type>
    class KeyEventArgs : public TypedRoutedEventArgs<Type>
    {
    public:
        /// @brief 虚拟按键
        VirtualKey key;

        /// @brief 附加信息
        KeyFlags flags;

        /// @brief 构造函数
        /// @param key 虚拟按键
        /// @param flags 附加信息
        KeyEventArgs(VirtualKey key, KeyFlags flags) : key(key), flags(flags)
        {
        }
    };

    /**
     * @brief 键盘按键按下事件参数类型
     */
    class KeyDownEventArgs : public KeyEventArgs<UIElement_KeyDown>
    {
    public:
        /// @brief 继承基类构造函数
        using KeyEventArgs<UIElement_KeyDown>::KeyEventArgs;
    };

    /**
     * @brief 键盘按键抬起事件参数类型
     */
    class KeyUpEventArgs : public KeyEventArgs<UIElement_KeyUp>
    {
    public:
        /// @brief 继承基类构造函数
        using KeyEventArgs<UIElement_KeyUp>::KeyEventArgs;
    };

    /**
     * @brief 鼠标事件参数类型模板
     * @tparam Type 路由事件类型，必须为RoutedEventType枚举中的值
     */
    template <RoutedEventType Type>
    class MouseEventArgs : public TypedRoutedEventArgs<Type>
    {
    public:
        /// @brief 鼠标位置
        Point mousePosition;

        /// @brief 按键状态
        MouseKey keyState;

        /// @brief 构造函数
        /// @param mousePosition 鼠标位置
        /// @param keyState 按键状态
        MouseEventArgs(Point mousePosition, MouseKey keyState)
            : mousePosition(mousePosition), keyState(keyState)
        {
        }
    };

    /**
     * @brief 鼠标移动事件参数类型
     */
    class MouseMoveEventArgs : public MouseEventArgs<UIElement_MouseMove>
    {
    public:
        /// @brief 继承基类构造函数
        using MouseEventArgs<UIElement_MouseMove>::MouseEventArgs;
    };

    /**
     * @brief 鼠标滚轮滚动事件参数类型
     */
    class MouseWheelEventArgs : public MouseEventArgs<UIElement_MouseWheel>
    {
    public:
        /// @brief 滚轮滚动的距离，为120的倍数
        int wheelDelta;

        /// @brief 构造函数
        /// @param wheelDelta 滚轮滚动的距离，为120的倍数
        /// @param mousePosition 鼠标位置
        /// @param keyState 按键状态
        MouseWheelEventArgs(int wheelDelta, Point mousePosition, MouseKey keyState)
            : MouseEventArgs<UIElement_MouseWheel>(mousePosition, keyState), wheelDelta(wheelDelta)
        {
        }
    };

    /**
     * @brief 鼠标按键事件参数类型模板
     * @tparam Type 路由事件类型，必须为RoutedEventType枚举中的值
     */
    template <RoutedEventType Type>
    class MouseButtonEventArgs : public MouseEventArgs<Type>
    {
    public:
        /// @brief 按下/抬起的按键（左键、中间、右键）
        MouseKey key;

        /// @brief 构造函数
        /// @param key 按下/抬起的按键（左键、中间、右键）
        /// @param mousePosition 鼠标位置
        /// @param keyState 按键状态
        MouseButtonEventArgs(MouseKey key, Point mousePosition, MouseKey keyState)
            : MouseEventArgs<Type>(mousePosition, keyState), key(key)
        {
        }
    };

    /**
     * @brief 鼠标按键按下事件参数类型
     */
    class MouseButtonDownEventArgs : public MouseButtonEventArgs<UIElement_MouseButtonDown>
    {
    public:
        /// @brief 继承基类构造函数
        using MouseButtonEventArgs<UIElement_MouseButtonDown>::MouseButtonEventArgs;
    };

    /**
     * @brief 鼠标按键抬起事件参数类型
     */
    class MouseButtonUpEventArgs : public MouseButtonEventArgs<UIElement_MouseButtonUp>
    {
    public:
        /// @brief 继承基类构造函数
        using MouseButtonEventArgs<UIElement_MouseButtonUp>::MouseButtonEventArgs;
    };

    /**
     * @brief 可取消事件参数类型，包含一个cancel字段用于指示是否取消事件
     */
    class CancelableEventArgs : public RoutedEventArgs
    {
    public:
        /// @brief 是否取消事件，默认为false
        bool cancel = false;
    };

    /**
     * @brief 显示用户自定义上下文菜单的事件参数类型
     */
    class ShowContextMenuEventArgs : public TypedRoutedEventArgs<UIElement_ShowContextMenu, CancelableEventArgs>
    {
    public:
        /// @brief 消息是否由按下快捷键（Shift+F10、VK_APPS）产生
        bool isKeyboardMsg;

        /// @brief 鼠标在屏幕中的位置
        Point mousePosition;

        /// @brief 构造函数
        /// @param isKeyboardMsg 消息是否由按下快捷键（Shift+F10、VK_APPS）产生
        /// @param mousePosition 鼠标在屏幕中的位置
        ShowContextMenuEventArgs(bool isKeyboardMsg, Point mousePosition)
            : isKeyboardMsg(isKeyboardMsg), mousePosition(mousePosition)
        {
        }
    };

    /**
     * @brief 文件拖放事件参数类型
     */
    class DropFilesEventArgs : public TypedRoutedEventArgs<UIElement_DropFiles>
    {
    public:
        /// @brief 描述拖入文件的句柄
        HDROP hDrop;

        /// @brief 构造函数
        /// @param hDrop 描述拖入文件的句柄
        DropFilesEventArgs(HDROP hDrop) : hDrop(hDrop)
        {
        }
    };

    /**
     * @brief 窗口正在关闭事件参数类型
     */
    class WindowClosingEventArgs : public TypedRoutedEventArgs<Window_Closing, CancelableEventArgs>
    {
    };

    /**
     * @brief 窗口/面板滚动条滚动事件参数类型
     */
    class ScrollingEventArgs : public TypedRoutedEventArgs<Layer_Scrolling, CancelableEventArgs>
    {
    public:
        /// @brief 滚动条类型
        ScrollOrientation scrollbar;

        /// @brief 滚动条事件
        ScrollEvent event;

        /// @brief 当event为ThumbPosition或ThubmTrack时表示当前滚动条位置，其他情况固定为0
        double pos;

        /// @brief 构造函数
        /// @param scrollbar 滚动条类型
        /// @param event 滚动条事件
        /// @param pos 当event为ThumbPosition或ThubmTrack时表示当前滚动条位置，其他情况固定为0
        ScrollingEventArgs(ScrollOrientation scrollbar, ScrollEvent event, double pos)
            : scrollbar(scrollbar), event(event), pos(pos)
        {
        }
    };

    /**
     * @brief 列表视图某个项事件参数类型模板，适用于单击、双击等事件
     * @tparam Type 路由事件类型，必须为RoutedEventType枚举中的值
     * @tparam TBase 事件参数类型的基类，默认为RoutedEventArgs
     */
    template <RoutedEventType Type, typename TBase = RoutedEventArgs>
    class ListViewItemEventArgs : public TypedRoutedEventArgs<Type, TBase>
    {
    public:
        /// @brief 发生事件的项的索引
        int index;

        /// @brief 构造函数
        /// @param index 发生事件的项的索引
        ListViewItemEventArgs(int index) : index(index)
        {
        }
    };

    /**
     * @brief 列表视图某个子项事件参数类型模板，适用于单击、双击等事件
     * @tparam Type 路由事件类型，必须为RoutedEventType枚举中的值
     * @tparam TBase 事件参数类型的基类，默认为RoutedEventArgs
     */
    template <RoutedEventType Type, typename TBase = RoutedEventArgs>
    class ListViewSubItemEventArgs : public ListViewItemEventArgs<Type, TBase>
    {
    public:
        /// @brief 发生事件的子项的索引
        int subIndex;

        /// @brief 构造函数
        /// @param index 发生事件的项的索引
        /// @param subIndex 发生事件的子项的索引
        ListViewSubItemEventArgs(int index, int subIndex)
            : ListViewItemEventArgs<Type, TBase>(index), subIndex(subIndex)
        {
        }
    };

    /**
     * @brief 列表视图某个复选框选中状态改变的事件参数类型
     */
    class ListViewCheckStateChangedEventArgs : public ListViewItemEventArgs<ListView_CheckStateChanged>
    {
    public:
        /// @brief 继承基类构造函数
        using ListViewItemEventArgs<ListView_CheckStateChanged>::ListViewItemEventArgs;
    };

    /**
     * @brief 列表视图的列标题单击事件参数类型
     */
    class ListViewHeaderClickedEventArgs : public ListViewItemEventArgs<ListView_HeaderClicked>
    {
    public:
        /// @brief 继承基类构造函数
        using ListViewItemEventArgs<ListView_HeaderClicked>::ListViewItemEventArgs;
    };

    /**
     * @brief 列表视图的列标题双击事件参数类型
     */
    class ListViewHeaderDoubleClickedEventArgs : public ListViewItemEventArgs<ListView_HeaderDoubleClicked>
    {
    public:
        /// @brief 继承基类构造函数
        using ListViewItemEventArgs<ListView_HeaderDoubleClicked>::ListViewItemEventArgs;
    };

    /**
     * @brief 列表视图项单击事件参数类型
     */
    class ListViewItemClickedEventArgs : public ListViewSubItemEventArgs<ListView_ItemClicked>
    {
    public:
        /// @brief 继承基类构造函数
        using ListViewSubItemEventArgs<ListView_ItemClicked>::ListViewSubItemEventArgs;
    };

    /**
     * @brief 列表视图项双击事件参数类型
     */
    class ListViewItemDoubleClickedEventArgs : public ListViewSubItemEventArgs<ListView_ItemDoubleClicked>
    {
    public:
        /// @brief 继承基类构造函数
        using ListViewSubItemEventArgs<ListView_ItemDoubleClicked>::ListViewSubItemEventArgs;
    };

    /**
     * @brief 列表视图编辑状态结束事件参数类型
     */
    class ListViewEndEditEventArgs : public ListViewItemEventArgs<ListView_EndEdit, CancelableEventArgs>
    {
    public:
        /// @brief 新的文本
        std::wstring newText;

        /// @brief 构造函数
        /// @param index 发生事件的项的索引
        /// @param newText 新的文本
        ListViewEndEditEventArgs(int index, const std::wstring &newText)
            : ListViewItemEventArgs<ListView_EndEdit, CancelableEventArgs>(index), newText(newText)
        {
        }
    };

    /**
     * @brief 时间改变事件参数类型模板，适用于DateTimePicker和MonthCalendar控件
     * @tparam Type 路由事件类型，必须为RoutedEventType枚举中的值
     */
    template <RoutedEventType Type>
    class TimeChangedEventArgs : public TypedRoutedEventArgs<Type>
    {
    public:
        /// @brief 时间的新值
        SYSTEMTIME time;

        /// @brief 构造函数
        /// @param time 时间的新值
        TimeChangedEventArgs(const SYSTEMTIME &time) : time(time)
        {
        }
    };

    /**
     * @brief DateTimePicker控件时间改变事件参数类型
     */
    class DateTimePickerTimeChangedEventArgs : public TimeChangedEventArgs<DateTimePicker_TimeChanged>
    {
    public:
        /// @brief 继承基类构造函数
        using TimeChangedEventArgs<DateTimePicker_TimeChanged>::TimeChangedEventArgs;
    };

    /**
     * @brief 月历控件时间改变事件参数类型
     */
    class MonthCalendarTimeChangedEventArgs : public TimeChangedEventArgs<MonthCalendar_TimeChanged>
    {
    public:
        /// @brief 继承基类构造函数
        using TimeChangedEventArgs<MonthCalendar_TimeChanged>::TimeChangedEventArgs;
    };

    /**
     * @brief SysLink控件链接被单击事件参数类型
     */
    class SysLinkClickedEventArgs : public TypedRoutedEventArgs<SysLink_Clicked>
    {
    public:
        /// @brief 被单击链接的id
        std::wstring id;

        /// @brief 被单击链接的url（即href）
        std::wstring url;

        /// @brief 构造函数
        /// @param id 被单击链接的id
        /// @param url 被单击链接的url（即href）
        SysLinkClickedEventArgs(const std::wstring &id, const std::wstring &url) : id(id), url(url)
        {
        }
    };

    /**
     * @brief 热键框值改变事件参数类型
     */
    class HotKeyValueChangedEventArgs : public TypedRoutedEventArgs<HotKeyControl_ValueChanged>
    {
    public:
        /// @brief 按键
        VirtualKey key;

        /// @brief 辅助按键
        HotKeyModifier modifier;

        /// @brief 构造函数
        /// @param key 按键
        /// @param modifier 辅助按键
        HotKeyValueChangedEventArgs(VirtualKey key, HotKeyModifier modifier) : key(key), modifier(modifier)
        {
        }
    };

    /**
     * @brief 分割按钮的下拉箭头单击事件参数类型
     */
    class SplitButtonDropDownEventArgs : public TypedRoutedEventArgs<SplitButton_DropDown, CancelableEventArgs>
    {
    };
}
