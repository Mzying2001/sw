#pragma once

#include "Alignment.h"
#include "Color.h"
#include "ContextMenu.h"
#include "EnumBit.h"
#include "EventHandlerWrapper.h"
#include "ILayout.h"
#include "ITag.h"
#include "RoutedEvent.h"
#include "RoutedEventArgs.h"
#include "Thickness.h"
#include "WndBase.h"
#include "WndMsg.h"
#include <cstdint>
#include <map>
#include <string>
#include <vector>

namespace sw
{
    /**
     * @brief 通知布局更新的条件
     */
    enum class LayoutUpdateCondition : uint32_t {
        /**
         * @brief 尺寸改变时更新布局
         */
        SizeChanged = 1 << 0,

        /**
         * @brief 位置改变时更新布局
         */
        PositionChanged = 1 << 1,

        /**
         * @brief 添加子元素时更新布局
         */
        ChildAdded = 1 << 2,

        /**
         * @brief 移除子元素时更新布局
         */
        ChildRemoved = 1 << 3,

        /**
         * @brief 文本改变时更新布局
         */
        TextChanged = 1 << 4,

        /**
         * @brief 字体改变时更新布局
         */
        FontChanged = 1 << 5,

        /**
         * @brief 框架内部使用，表示布局已失效
         * @note  该标记指示了Measure函数的结果已失效，需要重新调用Measure函数来更新尺寸
         */
        MeasureInvalidated = 1 << 29,

        /**
         * @brief 框架内部使用，表示不需要更新布局
         * @note  一旦设置了该标记，InvalidateMeasure函数将不会更新状态和触发布局更新
         * @note  该标记用于抑制布局更新，可能会频繁被设置/取消，一般不建议用户直接使用
         */
        Supressed = 1 << 30,
    };

    /**
     * @brief 标记LayoutUpdateCondition支持位操作
     */
    _SW_ENUM_ENABLE_BIT_OPERATIONS(LayoutUpdateCondition);

    /**
     * @brief 表示界面中的元素
     */
    class UIElement : public WndBase, public ILayout, public ITag
    {
    private:
        /**
         * @brief 布局更新条件
         */
        sw::LayoutUpdateCondition _layoutUpdateCondition =
            sw::LayoutUpdateCondition::SizeChanged |
            sw::LayoutUpdateCondition::ChildAdded |
            sw::LayoutUpdateCondition::ChildRemoved;

        /**
         * @brief 是否在不可见时不参与布局
         */
        bool _collapseWhenHide = true;

        /**
         * @brief 当前元素所需要占用的尺寸
         */
        Size _desireSize{};

        /**
         * @brief 当对齐方式为拉伸时用该字段存储原始大小
         */
        Size _origionalSize{};

        /**
         * @brief 最小尺寸，若值为负数或0则表示不限制最小尺寸
         */
        Size _minSize{-1, -1};

        /**
         * @brief 最大尺寸，若值为负数或0则表示不限制最大尺寸
         */
        Size _maxSize{-1, -1};

        /**
         * @brief 边距
         */
        Thickness _margin{};

        /**
         * @brief 水平对齐方式
         */
        HorizontalAlignment _horizontalAlignment = HorizontalAlignment::Center;

        /**
         * @brief 垂直对齐方式
         */
        VerticalAlignment _verticalAlignment = VerticalAlignment::Center;

        /**
         * @brief 指向父元素的指针，在调用SetParent后会更新该值
         */
        UIElement *_parent = nullptr;

        /**
         * @brief 所有子元素
         */
        std::vector<UIElement *> _children{};

        /**
         * @brief 参与布局的子元素，即所有非collapsed状态的子元素
         */
        std::vector<UIElement *> _layoutVisibleChildren{};

        /**
         * @brief 记录路由事件的map
         */
        std::map<RoutedEventType, RoutedEventHandler> _eventMap{};

        /**
         * @brief 储存用户自定义信息
         */
        uint64_t _tag = 0;

        /**
         * @brief 布局标记
         */
        uint64_t _layoutTag = 0;

        /**
         * @brief 上下文菜单
         */
        sw::ContextMenu *_contextMenu = nullptr;

        /**
         * @brief Arrange时子元素的水平偏移量
         */
        double _arrangeOffsetX = 0;

        /**
         * @brief Arrange时子元素的垂直偏移量
         */
        double _arrangeOffsetY = 0;

        /**
         * @brief 所有子元素在当前元素中最右边的位置
         */
        double _childRightmost = 0;

        /**
         * @brief 所有子元素在当前元素中最底边的位置
         */
        double _childBottommost = 0;

        /**
         * @brief 元素是否悬浮，若元素悬浮则该元素不会随滚动条滚动而改变位置
         */
        bool _float = false;

        /**
         * @brief 表示用户是否可以通过按下Tab键将焦点移动到当前元素
         */
        bool _tabStop = false;

        /**
         * @brief 背景颜色
         */
        Color _backColor{KnownColor::White};

        /**
         * @brief 文本颜色
         */
        Color _textColor{KnownColor::Black};

        /**
         * @brief 是否使用透明背景
         */
        bool _transparent = false;

        /**
         * @brief 是否继承父元素的文本颜色
         */
        bool _inheritTextColor = false;

        /**
         * @brief 是否使用默认的鼠标样式
         */
        bool _useDefaultCursor = true;

        /**
         * @brief 鼠标句柄
         */
        HCURSOR _hCursor = NULL;

        /**
         * @brief 上一次Measure函数调用时的可用大小
         */
        Size _lastMeasureAvailableSize{};

        /**
         * @brief 用于存储批量调整子元素位置时调用DeferWindowPos的句柄
         */
        HDWP _hdwpChildren = NULL;

    public:
        /**
         * @brief 边距
         */
        const Property<Thickness> Margin;

        /**
         * @brief 水平对齐方式
         */
        const Property<HorizontalAlignment> HorizontalAlignment;

        /**
         * @brief 垂直对齐方式
         */
        const Property<VerticalAlignment> VerticalAlignment;

        /**
         * @brief 子元素数量
         */
        const ReadOnlyProperty<int> ChildCount;

        /**
         * @brief 是否在不可见时不参与布局
         */
        const Property<bool> CollapseWhenHide;

        /**
         * @brief 指向父元素的指针，当前元素为顶级窗口时该值为nullptr
         */
        const ReadOnlyProperty<UIElement *> Parent;

        /**
         * @brief 储存用户自定义信息的标记
         */
        const Property<uint64_t> Tag;

        /**
         * @brief 布局标记，对于不同的布局有不同含义
         */
        const Property<uint64_t> LayoutTag;

        /**
         * @brief 右键按下时弹出的菜单
         */
        const Property<sw::ContextMenu *> ContextMenu;

        /**
         * @brief 元素是否悬浮，若元素悬浮则该元素不会随滚动条滚动而改变位置
         */
        const Property<bool> Float;

        /**
         * @brief 表示用户是否可以通过按下Tab键将焦点移动到当前元素
         */
        const Property<bool> TabStop;

        /**
         * @brief 背景颜色，修改该属性会同时将Transparent属性设为false，对于部分控件该属性可能不生效
         */
        const Property<Color> BackColor;

        /**
         * @brief 文本颜色，修改该属性会同时将InheritTextColor属性设为false，对于部分控件该属性可能不生效
         */
        const Property<Color> TextColor;

        /**
         * @brief 是否使用透明背景（此属性并非真正意义上的透明，将该属性设为true可继承父元素的背景颜色）
         */
        const Property<bool> Transparent;

        /**
         * @brief 是否继承父元素的文本颜色
         */
        const Property<bool> InheritTextColor;

        /**
         * @brief 触发布局更新的条件
         * @note  修改该属性不会立即触发布局更新
         */
        const Property<sw::LayoutUpdateCondition> LayoutUpdateCondition;

        /**
         * @brief 当前元素的布局状态是否有效
         */
        const ReadOnlyProperty<bool> IsMeasureValid;

        /**
         * @brief 最小宽度，当值为负数或0时表示不限制
         */
        const Property<double> MinWidth;

        /**
         * @brief 最小高度，当值为负数或0时表示不限制
         */
        const Property<double> MinHeight;

        /**
         * @brief 最大宽度，当值为负数或0时表示不限制
         */
        const Property<double> MaxWidth;

        /**
         * @brief 最大高度，当值为负数或0时表示不限制
         */
        const Property<double> MaxHeight;

    public:
        /**
         * @brief 初始化UIElement
         */
        UIElement();

        /**
         * @brief 析构函数，这里用纯虚函数使该类成为抽象类
         */
        virtual ~UIElement() = 0;

        /**
         * @brief           注册路由事件处理函数，当事件已注册时会覆盖已注册的函数
         * @param eventType 路由事件类型
         * @param handler   处理函数，当值为nullptr时可取消注册
         * @deprecated      建议使用AddHandler函数代替以避免覆盖已注册的事件处理函数
         */
        void RegisterRoutedEvent(RoutedEventType eventType, const RoutedEventHandler &handler);

        /**
         * @brief           注册成员函数作为路由事件处理函数，当事件已注册时会覆盖已注册的函数
         * @tparam T        成员函数所在的类
         * @param eventType 路由事件类型
         * @param obj       注册的成员函数所在的对象
         * @param handler   处理函数，当值为nullptr时可取消注册
         * @deprecated      建议使用AddHandler函数代替以避免覆盖已注册的事件处理函数
         */
        template <typename T>
        void RegisterRoutedEvent(RoutedEventType eventType, T &obj, void (T::*handler)(UIElement &, RoutedEventArgs &))
        {
            if (handler == nullptr) {
                this->UnregisterRoutedEvent(eventType);
            } else {
                this->RegisterRoutedEvent(eventType, RoutedEventHandler(obj, handler));
            }
        }

        /**
         * @brief             根据事件参数类型注册路由事件，当事件已注册时会覆盖已注册的函数
         * @tparam TEventArgs 路由事件的参数类型，必须继承自TypedRoutedEventArgs<...>
         * @param handler     事件的处理函数，当值为nullptr时可取消注册
         * @deprecated        建议使用AddHandler函数代替以避免覆盖已注册的事件处理函数
         */
        template <typename TEventArgs>
        typename std::enable_if<std::is_base_of<RoutedEventArgs, TEventArgs>::value && sw::_IsTypedRoutedEventArgs<TEventArgs>::value>::type
        RegisterRoutedEvent(const Action<UIElement &, TEventArgs &> &handler)
        {
            if (!handler) {
                this->UnregisterRoutedEvent(TEventArgs::EventType);
            } else {
                this->RegisterRoutedEvent(TEventArgs::EventType, RoutedEventHandlerWrapper<TEventArgs>(handler));
            }
        }

        /**
         * @brief             根据事件参数类型注册成员函数作为路由事件，当事件已注册时会覆盖已注册的函数
         * @tparam TEventArgs 路由事件的参数类型，必须继承自TypedRoutedEventArgs<...>
         * @tparam THandleObj 成员函数所在的类
         * @param obj         注册的成员函数所在的对象
         * @param handler     事件的处理函数，当值为nullptr时可取消注册
         * @deprecated        建议使用AddHandler函数代替以避免覆盖已注册的事件处理函数
         */
        template <typename TEventArgs, typename THandleObj>
        typename std::enable_if<std::is_base_of<RoutedEventArgs, TEventArgs>::value && sw::_IsTypedRoutedEventArgs<TEventArgs>::value>::type
        RegisterRoutedEvent(THandleObj &obj, void (THandleObj::*handler)(UIElement &, TEventArgs &))
        {
            if (handler == nullptr) {
                this->UnregisterRoutedEvent(TEventArgs::EventType);
            } else {
                this->RegisterRoutedEvent(TEventArgs::EventType, RoutedEventHandlerWrapper<TEventArgs>(Action<UIElement &, TEventArgs &>(obj, handler)));
            }
        }

        /**
         * @brief           添加路由事件处理函数
         * @param eventType 路由事件类型
         * @param handler   处理函数
         */
        void AddHandler(RoutedEventType eventType, const RoutedEventHandler &handler);

        /**
         * @brief           添加成员函数作为路由事件处理函数
         * @tparam T        成员函数所在的类
         * @param eventType 路由事件类型
         * @param obj       注册的成员函数所在的对象
         * @param handler   处理函数
         */
        template <typename T>
        void AddHandler(RoutedEventType eventType, T &obj, void (T::*handler)(UIElement &, RoutedEventArgs &))
        {
            if (handler) this->_eventMap[eventType].Add(obj, handler);
        }

        /**
         * @brief             根据事件参数类型添加路由事件处理函数
         * @tparam TEventArgs 路由事件的参数类型，必须继承自TypedRoutedEventArgs<...>
         * @param handler     事件的处理函数
         */
        template <typename TEventArgs>
        typename std::enable_if<std::is_base_of<RoutedEventArgs, TEventArgs>::value && sw::_IsTypedRoutedEventArgs<TEventArgs>::value>::type
        AddHandler(const Action<UIElement &, TEventArgs &> &handler)
        {
            if (handler) this->AddHandler(TEventArgs::EventType, RoutedEventHandlerWrapper<TEventArgs>(handler));
        }

        /**
         * @brief             根据事件参数类型添加成员函数作为路由事件处理函数
         * @tparam TEventArgs 路由事件的参数类型，必须继承自TypedRoutedEventArgs<...>
         * @tparam THandleObj 成员函数所在的类
         * @param obj         注册的成员函数所在的对象
         * @param handler     事件的处理函数
         */
        template <typename TEventArgs, typename THandleObj>
        typename std::enable_if<std::is_base_of<RoutedEventArgs, TEventArgs>::value && sw::_IsTypedRoutedEventArgs<TEventArgs>::value>::type
        AddHandler(THandleObj &obj, void (THandleObj::*handler)(UIElement &, TEventArgs &))
        {
            if (handler) this->AddHandler(TEventArgs::EventType, RoutedEventHandlerWrapper<TEventArgs>(Action<UIElement &, TEventArgs &>(obj, handler)));
        }

        /**
         * @brief             添加路由事件处理函数
         * @tparam TEventArgs 路由事件的参数类型，必须继承自RoutedEventArgs
         * @param eventType   路由事件类型
         * @param handler     处理函数
         */
        template <typename TEventArgs>
        typename std::enable_if<
            std::is_base_of<RoutedEventArgs, TEventArgs>::value &&
            !std::is_same<TEventArgs, RoutedEventArgs>::value &&
            !sw::_IsTypedRoutedEventArgs<TEventArgs>::value>::type
        AddHandler(RoutedEventType eventType, const Action<UIElement &, TEventArgs &> &handler)
        {
            if (handler) this->AddHandler(eventType, RoutedEventHandlerWrapper<TEventArgs>(handler));
        }

        /**
         * @brief             添加成员函数作为路由事件处理函数
         * @tparam TEventArgs 路由事件的参数类型，必须继承自RoutedEventArgs
         * @tparam THandleObj 成员函数所在的类
         * @param eventType   路由事件类型
         * @param obj         注册的成员函数所在的对象
         * @param handler     处理函数
         */
        template <typename TEventArgs, typename THandleObj>
        typename std::enable_if<
            std::is_base_of<RoutedEventArgs, TEventArgs>::value &&
            !std::is_same<TEventArgs, RoutedEventArgs>::value &&
            !sw::_IsTypedRoutedEventArgs<TEventArgs>::value>::type
        AddHandler(RoutedEventType eventType, THandleObj &obj, void (THandleObj::*handler)(UIElement &, TEventArgs &))
        {
            if (handler) this->AddHandler(eventType, RoutedEventHandlerWrapper<TEventArgs>(Action<UIElement &, TEventArgs &>(obj, handler)));
        }

        /**
         * @brief           移除已添加的路由事件处理函数
         * @param eventType 路由事件类型
         * @param handler   处理函数
         * @return          是否成功移除
         */
        bool RemoveHandler(RoutedEventType eventType, const RoutedEventHandler &handler);

        /**
         * @brief           移除已添加的类型为成员函数的路由事件处理函数
         * @tparam T        成员函数所在的类
         * @param eventType 路由事件类型
         * @param obj       注册的成员函数所在的对象
         * @param handler   处理函数
         * @return          是否成功移除
         */
        template <typename T>
        bool RemoveHandler(RoutedEventType eventType, T &obj, void (T::*handler)(UIElement &, RoutedEventArgs &))
        {
            return handler == nullptr ? false : this->_eventMap[eventType].Remove(obj, handler);
        }

        /**
         * @brief             移除已添加的路由事件处理函数
         * @tparam TEventArgs 路由事件的参数类型，必须继承自TypedRoutedEventArgs<...>
         * @param handler     事件的处理函数
         * @return            是否成功移除
         */
        template <typename TEventArgs>
        typename std::enable_if<std::is_base_of<RoutedEventArgs, TEventArgs>::value && sw::_IsTypedRoutedEventArgs<TEventArgs>::value, bool>::type
        RemoveHandler(const Action<UIElement &, TEventArgs &> &handler)
        {
            if (handler == nullptr) {
                return false;
            } else {
                return this->RemoveHandler(TEventArgs::EventType, RoutedEventHandlerWrapper<TEventArgs>(handler));
            }
        }

        /**
         * @brief             移除已添加的类型为成员函数的路由事件处理函数
         * @tparam TEventArgs 路由事件的参数类型，必须继承自TypedRoutedEventArgs<...>
         * @tparam THandleObj 成员函数所在的类
         * @param obj         注册的成员函数所在的对象
         * @param handler     事件的处理函数
         * @return            是否成功移除
         */
        template <typename TEventArgs, typename THandleObj>
        typename std::enable_if<std::is_base_of<RoutedEventArgs, TEventArgs>::value && sw::_IsTypedRoutedEventArgs<TEventArgs>::value, bool>::type
        RemoveHandler(THandleObj &obj, void (THandleObj::*handler)(UIElement &, TEventArgs &))
        {
            if (handler == nullptr) {
                return false;
            } else {
                return this->RemoveHandler(TEventArgs::EventType, RoutedEventHandlerWrapper<TEventArgs>(Action<UIElement &, TEventArgs &>(obj, handler)));
            }
        }

        /**
         * @brief             移除已添加的路由事件处理函数
         * @tparam TEventArgs 路由事件的参数类型，必须继承自RoutedEventArgs
         * @param eventType   路由事件类型
         * @param handler     处理函数
         * @return            是否成功移除
         */
        template <typename TEventArgs>
        typename std::enable_if<
            std::is_base_of<RoutedEventArgs, TEventArgs>::value &&
                !std::is_same<TEventArgs, RoutedEventArgs>::value &&
                !sw::_IsTypedRoutedEventArgs<TEventArgs>::value,
            bool>::type
        RemoveHandler(RoutedEventType eventType, const Action<UIElement &, TEventArgs &> &handler)
        {
            if (handler == nullptr) {
                return false;
            } else {
                return this->RemoveHandler(eventType, RoutedEventHandlerWrapper<TEventArgs>(handler));
            }
        }

        /**
         * @brief             移除已添加的类型为成员函数的路由事件处理函数
         * @tparam TEventArgs 路由事件的参数类型，必须继承自RoutedEventArgs
         * @tparam THandleObj 成员函数所在的类
         * @param eventType   路由事件类型
         * @param obj         注册的成员函数所在的对象
         * @param handler     处理函数
         * @return            是否成功移除
         */
        template <typename TEventArgs, typename THandleObj>
        typename std::enable_if<
            std::is_base_of<RoutedEventArgs, TEventArgs>::value &&
                !std::is_same<TEventArgs, RoutedEventArgs>::value &&
                !sw::_IsTypedRoutedEventArgs<TEventArgs>::value,
            bool>::type
        RemoveHandler(RoutedEventType eventType, THandleObj &obj, void (THandleObj::*handler)(UIElement &, TEventArgs &))
        {
            if (handler == nullptr) {
                return false;
            } else {
                return this->RemoveHandler(eventType, RoutedEventHandlerWrapper<TEventArgs>(Action<UIElement &, TEventArgs &>(obj, handler)));
            }
        }

        /**
         * @brief           取消对应类型路由事件的注册，该函数会移除对应事件所有的处理函数
         * @param eventType 路由事件类型
         */
        void UnregisterRoutedEvent(RoutedEventType eventType);

        /**
         * @brief           判断路由事件是否已存在事件处理函数
         * @param eventType 路由事件类型
         */
        bool IsRoutedEventRegistered(RoutedEventType eventType);

        /**
         * @brief 通过索引获取子元素
         */
        UIElement &operator[](int index) const;

        /**
         * @brief 获取子元素
         */
        UIElement &GetChildAt(int index) const;

        /**
         * @brief  添加子元素
         * @return 添加是否成功
         */
        bool AddChild(UIElement *element);

        /**
         * @brief  添加子元素
         * @return 添加是否成功
         */
        bool AddChild(UIElement &element);

        /**
         * @brief  添加子元素并设置布局标记
         * @return 添加是否成功
         */
        bool AddChild(UIElement *element, uint64_t layoutTag);

        /**
         * @brief  添加子元素并设置布局标记
         * @return 添加是否成功
         */
        bool AddChild(UIElement &element, uint64_t layoutTag);

        /**
         * @brief       移除指定索引处的子元素
         * @param index 要移除的索引
         * @return      移除是否成功
         */
        bool RemoveChildAt(int index);

        /**
         * @brief  移除子元素
         * @return 移除是否成功
         */
        bool RemoveChild(UIElement *element);

        /**
         * @brief  移除子元素
         * @return 移除是否成功
         */
        bool RemoveChild(UIElement &element);

        /**
         * @brief 移除所有子元素
         */
        void ClearChildren();

        /**
         * @brief         获取指定元素的索引
         * @param element 要查找的元素
         * @return        若找到指定元素则返回对应的索引，否则返回-1
         */
        int IndexOf(UIElement *element);

        /**
         * @brief         获取指定元素的索引
         * @param element 要查找的元素
         * @return        若找到指定元素则返回对应的索引，否则返回-1
         */
        int IndexOf(UIElement &element);

        /**
         * @brief       弹出当前元素的上下文菜单
         * @param point 弹出菜单左上角在屏幕中的位置
         */
        void ShowContextMenu(const Point &point);

        /**
         * @brief 移动到界面顶部
         */
        void MoveToTop();

        /**
         * @brief 移动到界面底部
         */
        void MoveToBottom();

        /**
         * @brief 判断当前元素是否为根节点
         */
        bool IsRootElement();

        /**
         * @brief 获取当前元素所在界面树的根节点
         */
        UIElement *GetRootElement();

        /**
         * @brief 获取当前元素在界面树上的下一个节点，若已是最后一个节点则返回根节点
         */
        UIElement *GetNextElement();

        /**
         * @brief 获取当前元素在界面树上的上一个节点，若元素为第一个节点则返回最后一个节点
         */
        UIElement *GetPreviousElement();

        /**
         * @brief 获取下一个TabStop属性为true的元素
         */
        UIElement *GetNextTabStopElement();

        /**
         * @brief 获取上一个TabStop属性为true的元素
         */
        UIElement *GetPreviousTabStopElement();

        /**
         * @brief 获取当前要显示的背景颜色：当Transparent为true时获取到祖先节点中首个Transparent为false的背景颜色，否则返回当前元素的背景颜色
         */
        Color GetRealBackColor();

        /**
         * @brief 获取当前要显示的文本颜色：当InheritTextColor为true时获取到祖先节点中首个InheritTextColor为false的文本颜色，否则返回当前元素的文本颜色
         */
        Color GetRealTextColor();

        /**
         * @brief         设置鼠标样式
         * @param hCursor 鼠标句柄
         */
        void SetCursor(HCURSOR hCursor);

        /**
         * @brief        设置鼠标样式
         * @param cursor 鼠标样式
         */
        void SetCursor(StandardCursor cursor);

        /**
         * @brief 将鼠标样式设置为默认样式
         */
        void ResetCursor();

        /**
         * @brief      设置对齐方式
         * @param horz 水平对齐方式
         * @param vert 垂直对齐方式
         */
        void SetAlignment(sw::HorizontalAlignment horz, sw::VerticalAlignment vert);

        /**
         * @brief 调整当前元素的尺寸，也可以用该函数更改默认Measure函数在当前横向或纵向对齐方式为拉伸时的DesireSize
         */
        void Resize(const Size &size);

        /**
         * @brief 判断指定的布局更新条件是否已设置
         */
        bool IsLayoutUpdateConditionSet(sw::LayoutUpdateCondition condition);

        /**
         * @brief 使元素的布局状态失效，并立即触发布局更新
         */
        void InvalidateMeasure();

        /**
         * @brief 获取Tag
         */
        virtual uint64_t GetTag() override;

        /**
         * @brief 设置Tag
         */
        virtual void SetTag(uint64_t tag) override;

        /**
         * @brief 获取布局标记
         */
        virtual uint64_t GetLayoutTag() override;

        /**
         * @brief 获取参与布局的子元素数量
         * @note  参与布局的子元素：即非collapsed状态的元素
         */
        virtual int GetChildLayoutCount() override;

        /**
         * @brief 获取对应索引处的子元素，只索引参与布局的子元素
         * @note  参与布局的子元素：即非collapsed状态的元素
         */
        virtual ILayout &GetChildLayoutAt(int index) override;

        /**
         * @brief 获取当前元素所需尺寸
         */
        virtual Size GetDesireSize() override;

        /**
         * @brief               测量元素所需尺寸
         * @param availableSize 可用的尺寸
         */
        virtual void Measure(const Size &availableSize) override;

        /**
         * @brief           安排元素位置
         * @param finalSize 最终元素所安排的位置
         */
        virtual void Arrange(const sw::Rect &finalPosition) override;

        /**
         * @brief  尝试将对象转换成UIElement
         * @return 若函数成功则返回UIElement指针，否则返回nullptr
         */
        virtual UIElement *ToUIElement() override;

    protected:
        /**
         * @brief           触发路由事件
         * @param eventType 事件类型
         */
        void RaiseRoutedEvent(RoutedEventType eventType);

        /**
         * @brief           触发路由事件
         * @param eventArgs 要触发事件的事件参数
         */
        void RaiseRoutedEvent(RoutedEventArgs &eventArgs);

        /**
         * @brief 获取Arrange时子元素的水平偏移量
         */
        double &GetArrangeOffsetX();

        /**
         * @brief 获取Arrange时子元素的垂直偏移量
         */
        double &GetArrangeOffsetY();

        /**
         * @brief        获取所有子元素在当前元素中最右边的位置（只考虑参与布局的子窗口且忽略悬浮的元素）
         * @param update 是否更字段
         * @return       _childRightmost字段
         */
        double GetChildRightmost(bool update);

        /**
         * @brief        获取所有子元素在当前元素中最底边的位置（只考虑参与布局的子窗口且忽略悬浮的元素）
         * @param update 是否更字段
         * @return       _childBottommost字段
         */
        double GetChildBottommost(bool update);

        /**
         * @brief 更新子元素的Z轴位置
         */
        void UpdateChildrenZOrder(bool invalidateMeasure = true);

        /**
         * @brief 更新兄弟元素的Z轴位置
         */
        void UpdateSiblingsZOrder(bool invalidateMeasure = true);

        /**
         * @brief 设置下一个TabStop属性为true的元素为焦点元素
         */
        void SetNextTabStopFocus();

        /**
         * @brief 设置上一个TabStop属性为true的元素为焦点元素
         */
        void SetPreviousTabStopFocus();

        /**
         * @brief      限定指定尺寸在最小和最大尺寸之间
         * @param size 要限定的尺寸
         */
        void ClampDesireSize(sw::Size &size);

        /**
         * @brief      限定指定矩形的尺寸在最小和最大尺寸之间
         * @param rect 要限定的矩形
         */
        void ClampDesireSize(sw::Rect &rect);

        /**
         * @brief               测量元素所需尺寸，无需考虑边框和边距
         * @param availableSize 可用的尺寸
         * @return              返回元素需要占用的尺寸
         */
        virtual Size MeasureOverride(const Size &availableSize);

        /**
         * @brief           安排子元素的位置，可重写该函数以实现自定义布局
         * @param finalSize 可用于排列子元素的最终尺寸
         */
        virtual void ArrangeOverride(const Size &finalSize);

        /**
         * @brief        设置背景颜色
         * @param color  要设置的颜色
         * @param redraw 是否重绘
         */
        virtual void SetBackColor(Color color, bool redraw);

        /**
         * @brief        设置文本颜色
         * @param color  要设置的颜色
         * @param redraw 是否重绘
         */
        virtual void SetTextColor(Color color, bool redraw);

        /**
         * @brief         添加子元素后调用该函数
         * @param element 添加的子元素
         */
        virtual void OnAddedChild(UIElement &element);

        /**
         * @brief         移除子元素后调用该函数
         * @param element 移除的子元素
         */
        virtual void OnRemovedChild(UIElement &element);

        /**
         * @brief 通过tab键将焦点移动到当前元素时调用该函数
         */
        virtual void OnTabStop();

        /**
         * @brief 当MinWidth、MinHeight、MaxWidth或MaxHeight属性更改时调用此函数
         */
        virtual void OnMinMaxSizeChanged();

        /**
         * @brief           路由事件经过当前元素时调用该函数
         * @param eventArgs 事件参数
         * @param handler   事件处理函数，值为空时表示当前元素没有注册该事件处理函数
         * @return          若已处理该事件则返回true，否则返回false以继调用处理函数
         */
        virtual bool OnRoutedEvent(RoutedEventArgs &eventArgs, const RoutedEventHandler &handler);

        /**
         * @brief  设置父窗口
         * @return 设置是否成功
         */
        virtual bool SetParent(WndBase *parent) override;

        /**
         * @brief           父窗口改变时调用此函数
         * @param newParent 新的父窗口
         */
        virtual void ParentChanged(WndBase *newParent) override;

        /**
         * @brief  接收到WM_CLOSE时调用该函数
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnClose() override;

        /**
         * @brief                   接收到WM_MOVE时调用该函数
         * @param newClientPosition 移动后用户区左上角的位置
         * @return                  若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMove(Point newClientPosition) override;

        /**
         * @brief               接收到WM_SIZE时调用该函数
         * @param newClientSize 改变后的用户区尺寸
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnSize(Size newClientSize) override;

        /**
         * @brief Text属性更改时调用此函数
         */
        virtual void OnTextChanged() override;

        /**
         * @brief       字体改变时调用该函数
         * @param hfont 字体句柄
         */
        virtual void FontChanged(HFONT hfont) override;

        /**
         * @brief Visible属性改变时调用此函数
         */
        virtual void VisibleChanged(bool newVisible) override;

        /**
         * @brief            接收到WM_SETFOCUS时调用该函数
         * @param hPrevFocus 丢失焦点的hwnd，可能为NULL
         * @return           若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnSetFocus(HWND hPrevFocus) override;

        /**
         * @brief            接收到WM_KILLFOCUS时调用该函数
         * @param hNextFocus 接收到焦点的hwnd，可能为NULL
         * @return           若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnKillFocus(HWND hNextFocus) override;

        /**
         * @brief       接收到WM_CHAR时调用该函数
         * @param ch    按键的字符代码
         * @param flags 附加信息
         * @return      若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnChar(wchar_t ch, KeyFlags flags) override;

        /**
         * @brief       接收到WM_KEYDOWN时调用该函数
         * @param key   虚拟按键
         * @param flags 附加信息
         * @return      若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnKeyDown(VirtualKey key, KeyFlags flags) override;

        /**
         * @brief       接收到WM_KEYUP时调用该函数
         * @param key   虚拟按键
         * @param flags 附加信息
         * @return      若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnKeyUp(VirtualKey key, KeyFlags flags) override;

        /**
         * @brief               接收到WM_MOUSEMOVE时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState      指示某些按键是否按下
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseMove(Point mousePosition, MouseKey keyState) override;

        /**
         * @brief  接收到WM_MOUSELEAVE时调用该函数
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseLeave() override;

        /**
         * @brief               接收到WM_MOUSEWHEEL时调用该函数
         * @param wheelDelta    滚轮滚动的距离，为120的倍数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState      指示某些按键是否按下
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseWheel(int wheelDelta, Point mousePosition, MouseKey keyState) override;

        /**
         * @brief               接收到WM_LBUTTONDOWN时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState      指示某些按键是否按下
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseLeftButtonDown(Point mousePosition, MouseKey keyState) override;

        /**
         * @brief               接收到WM_LBUTTONUP时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState      指示某些按键是否按下
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseLeftButtonUp(Point mousePosition, MouseKey keyState) override;

        /**
         * @brief               接收到WM_RBUTTONDOWN时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState      指示某些按键是否按下
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseRightButtonDown(Point mousePosition, MouseKey keyState) override;

        /**
         * @brief               接收到WM_RBUTTONUP时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState      指示某些按键是否按下
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseRightButtonUp(Point mousePosition, MouseKey keyState) override;

        /**
         * @brief               接收到WM_MBUTTONDOWN时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState      指示某些按键是否按下
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseMiddleButtonDown(Point mousePosition, MouseKey keyState) override;

        /**
         * @brief               接收到WM_MBUTTONUP时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState      指示某些按键是否按下
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseMiddleButtonUp(Point mousePosition, MouseKey keyState) override;

        /**
         * @brief               接收到WM_CONTEXTMENU后调用目标控件的该函数
         * @param isKeyboardMsg 消息是否由按下快捷键（Shift+F10、VK_APPS）产生
         * @param mousePosition 鼠标在屏幕中的位置
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnContextMenu(bool isKeyboardMsg, Point mousePosition) override;

        /**
         * @brief    当WM_COMMAND接收到菜单命令时调用该函数
         * @param id 菜单id
         */
        virtual void OnMenuCommand(int id) override;

        /**
         * @brief           父窗口接收到WM_CTLCOLORxxx时调用对应控件的该函数
         * @param hdc       控件的显示上下文句柄
         * @param hRetBrush 要返回的画笔
         * @return          若返回true则将hRetBrush作为消息的返回值，否则使用DefaultWndProc的返回值
         */
        virtual bool OnColor(HDC hdc, HBRUSH &hRetBrush) override;

        /**
         * @brief         接收到WM_SETCURSOR消息时调用该函数
         * @param hwnd    鼠标所在窗口的句柄
         * @param hitTest hit-test的结果，详见WM_NCHITTEST消息的返回值
         * @param message 触发该事件的鼠标消息，如WM_MOUSEMOVE
         * @param result  消息的返回值，默认为false
         * @return        若返回true则将result作为消息的返回值，否则使用DefaultWndProc的返回值
         */
        virtual bool OnSetCursor(HWND hwnd, HitTestResult hitTest, int message, bool &result) override;

        /**
         * @brief       接收到WM_DROPFILES时调用该函数
         * @param hDrop 描述拖入文件的句柄
         * @return      若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnDropFiles(HDROP hDrop) override;

    private:
        /**
         * @brief       设置水平对齐方式
         * @param value 要设置的值
         * @return      值是否发生改变
         */
        bool _SetHorzAlignment(sw::HorizontalAlignment value);

        /**
         * @brief       设置垂直对齐方式
         * @param value 要设置的值
         * @return      值是否发生改变
         */
        bool _SetVertAlignment(sw::VerticalAlignment value);

        /**
         * @brief 添加MeasureInvalidated标记
         */
        void _SetMeasureInvalidated();

        /**
         * @brief 更新_layoutVisibleChildren的内容
         */
        void _UpdateLayoutVisibleChildren();

        /**
         * @brief 循环获取界面树上的下一个节点
         */
        static UIElement *_GetNextElement(UIElement *element, bool searchChildren = true);

        /**
         * @brief 获取界面树上最深的最后一个节点
         */
        static UIElement *_GetDeepestLastElement(UIElement *element);

        /**
         * @brief 循环获取界面树上的上一个节点
         */
        static UIElement *_GetPreviousElement(UIElement *element);
    };
}
