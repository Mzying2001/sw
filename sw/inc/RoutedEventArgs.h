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
     * @brief       表示特定类型路由事件的事件参数类型，继承自该类的类型可以直接作为RegisterRoutedEvent函数的模板参数
     * @tparam TYPE 一个RoutedEventType枚举值，表示路由事件类型
     */
    template <RoutedEventType TYPE>
    struct TypedRoutedEventArgs : RoutedEventArgs {
        /**
         * @brief 路由事件的类型，RegisterRoutedEvent模板函数使用此字段注册事件
         */
        static constexpr RoutedEventType EventType = TYPE;

        /**
         * @brief 构造函数，初始化事件类型为EventType
         */
        TypedRoutedEventArgs() : RoutedEventArgs(EventType) {}
    };

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
    struct _HasEventType<T, decltype(void(std::declval<T>().EventType))> : std::true_type {
    };

    /**
     * @brief 结构体模板，用于检测类型T是否包含事件类型信息
     */
    template <typename T>
    struct _IsTypedRoutedEventArgs : _HasEventType<T> {
    };

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
     * @brief 键盘按键按下事件参数类型
     */
    struct KeyDownEventArgs : TypedRoutedEventArgs<UIElement_KeyDown> {
        VirtualKey key; // 虚拟按键
        KeyFlags flags; // 附加信息
        KeyDownEventArgs(VirtualKey key, KeyFlags flags) : key(key), flags(flags) {}
    };

    /**
     * @brief 键盘按键抬起事件参数类型
     */
    struct KeyUpEventArgs : TypedRoutedEventArgs<UIElement_KeyUp> {
        VirtualKey key; // 虚拟按键
        KeyFlags flags; // 附加信息
        KeyUpEventArgs(VirtualKey key, KeyFlags flags) : key(key), flags(flags) {}
    };

    /**
     * @brief 鼠标移动事件参数类型
     */
    struct MouseMoveEventArgs : TypedRoutedEventArgs<UIElement_MouseMove> {
        Point mousePosition; // 鼠标位置
        MouseKey keyState;   // 按键状态
        MouseMoveEventArgs(Point mousePosition, MouseKey keyState)
            : mousePosition(mousePosition), keyState(keyState) {}
    };

    /**
     * @brief 鼠标滚轮滚动事件参数类型
     */
    struct MouseWheelEventArgs : TypedRoutedEventArgs<UIElement_MouseWheel> {
        int wheelDelta;      // 滚轮滚动的距离，为120的倍数
        Point mousePosition; // 鼠标位置
        MouseKey keyState;   // 按键状态
        MouseWheelEventArgs(int wheelDelta, Point mousePosition, MouseKey keyState)
            : wheelDelta(wheelDelta), mousePosition(mousePosition), keyState(keyState) {}
    };

    /**
     * @brief 鼠标按键按下事件参数类型
     */
    struct MouseButtonDownEventArgs : TypedRoutedEventArgs<UIElement_MouseButtonDown> {
        MouseKey key;        // 按下的按键（左键、中间、右键）
        Point mousePosition; // 鼠标位置
        MouseKey keyState;   // 按键状态
        MouseButtonDownEventArgs(MouseKey key, Point mousePosition, MouseKey keyState)
            : key(key), mousePosition(mousePosition), keyState(keyState) {}
    };

    /**
     * @brief 鼠标按键抬起事件参数类型
     */
    struct MouseButtonUpEventArgs : TypedRoutedEventArgs<UIElement_MouseButtonUp> {
        MouseKey key;        // 抬起的按键（左键、中间、右键）
        Point mousePosition; // 鼠标位置
        MouseKey keyState;   // 按键状态
        MouseButtonUpEventArgs(MouseKey key, Point mousePosition, MouseKey keyState)
            : key(key), mousePosition(mousePosition), keyState(keyState) {}
    };

    /**
     * @brief 显示用户自定义上下文菜单的事件参数类型
     */
    struct ShowContextMenuEventArgs : TypedRoutedEventArgs<UIElement_ShowContextMenu> {
        bool cancel = false; // 是否取消显示上下文菜单
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
    struct WindowClosingEventArgs : TypedRoutedEventArgs<Window_Closing> {
        bool cancel = false; // 是否取消本次关闭
    };

    /**
     * @brief 窗口/面板滚动条滚动事件参数类型
     */
    struct ScrollingEventArgs : TypedRoutedEventArgs<Layer_Scrolling> {
        bool cancel = false;         // 是否取消滚动条默认行为
        ScrollOrientation scrollbar; // 滚动条类型
        ScrollEvent event;           // 滚动条事件
        double pos;                  // 当event为ThumbPosition或ThubmTrack时表示当前滚动条位置，其他情况固定为0
        ScrollingEventArgs(ScrollOrientation scrollbar, ScrollEvent event, double pos)
            : scrollbar(scrollbar), event(event), pos(pos) {}
    };

    /**
     * @brief 列表视图某个复选框选中状态改变的事件参数类型
     */
    struct ListViewCheckStateChangedEventArgs : TypedRoutedEventArgs<ListView_CheckStateChanged> {
        int index; // 改变项的索引
        ListViewCheckStateChangedEventArgs(int index) : index(index) {}
    };

    /**
     * @brief 列表视图的列标题单击与双击事件参数类型
     */
    struct ListViewHeaderClickedEventArgs : RoutedEventArgs {
        int index; // 被点击列标题的索引
        ListViewHeaderClickedEventArgs(RoutedEventType eventType, int index)
            : RoutedEventArgs(eventType), index(index) {}
    };

    /**
     * @brief 列表视图项单击与双击事件参数类型
     */
    struct ListViewItemClickedEventArgs : RoutedEventArgs {
        int row; // 被点击的行
        int col; // 被点击的列
        ListViewItemClickedEventArgs(RoutedEventType eventType, int row, int col)
            : RoutedEventArgs(eventType), row(row), col(col) {}
    };

    /**
     * @brief 列表视图编辑状态结束事件参数类型
     */
    struct ListViewEndEditEventArgs : TypedRoutedEventArgs<ListView_EndEdit> {
        bool cancel = false; // 是否取消文本更改，默认为false
        int index;           // 被编辑项的索引
        wchar_t *newText;    // 新的文本
        ListViewEndEditEventArgs(int index, wchar_t *newText) : index(index), newText(newText) {}
    };

    /**
     * @brief DateTimePicker控件时间改变事件参数类型
     */
    struct DateTimePickerTimeChangedEventArgs : TypedRoutedEventArgs<DateTimePicker_TimeChanged> {
        SYSTEMTIME time; // 时间的新值
        DateTimePickerTimeChangedEventArgs(const SYSTEMTIME &time) : time(time) {}
    };

    /**
     * @brief 月历控件时间改变事件参数类型
     */
    struct MonthCalendarTimeChangedEventArgs : TypedRoutedEventArgs<MonthCalendar_TimeChanged> {
        SYSTEMTIME time; // 时间的新值
        MonthCalendarTimeChangedEventArgs(const SYSTEMTIME &time) : time(time) {}
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
}
