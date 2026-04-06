#pragma once

#include "Dip.h"
#include "LayoutHost.h"
#include "ScrollEnums.h"
#include "UIElement.h"
#include <cmath>

namespace sw
{
    template <typename TBase, typename = void>
    class Layer; // 向前声明

    /**
     * @brief 表示可以设置布局方式的元素类型，如窗口、面板等
     */
    template <typename TBase>
    class Layer<TBase,
                typename std::enable_if<std::is_base_of<UIElement, TBase>::value>::type>
        : public TBase
    {
    private:
        /**
         * @brief 滚动条滚动一行的距离
         */
        static constexpr int _LayerScrollBarLineInterval = 20;

    private:
        /**
         * @brief 是否按照布局方式与子元素自动调整尺寸
         */
        bool _autoSize = true;

        /**
         * @brief 指向所自定义的布局方式对象的指针
         */
        LayoutHost *_customLayout = nullptr;

        /**
         * @brief 记录水平滚动条是否已被禁止
         */
        bool _horizontalScrollDisabled = true;

        /**
         * @brief 记录垂直滚动条是否已被禁止
         */
        bool _verticalScrollDisabled = true;

        /**
         * @brief 是否在鼠标滚动时滚动内容
         */
        bool _mouseWheelScrollEnabled = true;

    public:
        /**
         * @brief 自定义的布局方式，赋值后将自动与所指向的布局关联，每个布局只能关联一个对象，设为nullptr可恢复默认布局
         */
        const Property<LayoutHost *> Layout{
            Property<LayoutHost *>::Init(this)
                .Getter([](Layer *self) -> LayoutHost * {
                    return self->_customLayout;
                })
                .Setter([](Layer *self, LayoutHost *value) {
                    if (value != nullptr)
                        value->Associate(self);
                    self->_customLayout = value;
                    self->InvalidateMeasure();
                })};

        /**
         * @brief 是否按照布局方式与子元素自动调整尺寸，该属性仅在当前元素已设置布局方式并且非顶级元素时有效
         */
        const Property<bool> AutoSize{
            Property<bool>::Init(this)
                .Getter([](Layer *self) -> bool {
                    return self->_autoSize;
                })
                .Setter([](Layer *self, bool value) {
                    if (self->_autoSize != value) {
                        self->_autoSize = value;
                        self->InvalidateMeasure();
                    }
                })};

        /**
         * @brief 是否显示横向滚动条
         */
        const Property<bool> HorizontalScrollBar{
            Property<bool>::Init(this)
                .Getter([](Layer *self) -> bool {
                    return self->GetStyle(WS_HSCROLL);
                })
                .Setter([](Layer *self, bool value) {
                    if (self->HorizontalScrollBar == value) {
                        return;
                    }
                    if (value) {
                        ShowScrollBar(self->Handle, SB_HORZ, value);
                        self->HorizontalScrollPos = self->HorizontalScrollPos;
                    } else {
                        self->HorizontalScrollPos = 0;
                        ShowScrollBar(self->Handle, SB_HORZ, value);
                    }
                })};

        /**
         * @brief 是否显示纵向滚动条
         */
        const Property<bool> VerticalScrollBar{
            Property<bool>::Init(this)
                .Getter([](Layer *self) -> bool {
                    return self->GetStyle(WS_VSCROLL);
                })
                .Setter([](Layer *self, bool value) {
                    if (self->VerticalScrollBar == value) {
                        return;
                    }
                    if (value) {
                        ShowScrollBar(self->Handle, SB_VERT, value);
                        self->VerticalScrollPos = self->VerticalScrollPos;
                    } else {
                        self->VerticalScrollPos = 0;
                        ShowScrollBar(self->Handle, SB_VERT, value);
                    }
                })};

        /**
         * @brief 横向滚动条位置
         */
        const Property<double> HorizontalScrollPos{
            Property<double>::Init(this)
                .Getter([](Layer *self) -> double {
                    SCROLLINFO info{};
                    info.cbSize = sizeof(info);
                    info.fMask  = SIF_POS;
                    GetScrollInfo(self->Handle, SB_HORZ, &info);
                    return Dip::PxToDipX(info.nPos);
                })
                .Setter([](Layer *self, double value) {
                    SCROLLINFO info{};
                    info.cbSize = sizeof(info);
                    info.fMask  = SIF_POS;
                    info.nPos   = Dip::DipToPxX(value);
                    SetScrollInfo(self->Handle, SB_HORZ, &info, true);

                    LayoutHost *layout = self->_GetLayout();

                    if (layout != nullptr && !self->_horizontalScrollDisabled && self->HorizontalScrollBar) {
                        self->GetInternalArrangeOffsetX() = -self->HorizontalScrollPos;
                        self->_MeasureAndArrangeWithoutResize(*layout, self->ClientRect->GetSize());
                    }
                })};

        /**
         * @brief 纵向滚动条位置
         */
        const Property<double> VerticalScrollPos{
            Property<double>::Init(this)
                .Getter([](Layer *self) -> double {
                    SCROLLINFO info{};
                    info.cbSize = sizeof(info);
                    info.fMask  = SIF_POS;
                    GetScrollInfo(self->Handle, SB_VERT, &info);
                    return Dip::PxToDipY(info.nPos);
                })
                .Setter(
                    [](Layer *self, double value) {
                        SCROLLINFO info{};
                        info.cbSize = sizeof(info);
                        info.fMask  = SIF_POS;
                        info.nPos   = Dip::DipToPxY(value);
                        SetScrollInfo(self->Handle, SB_VERT, &info, true);

                        LayoutHost *layout = self->_GetLayout();

                        if (layout != nullptr && !self->_verticalScrollDisabled && self->VerticalScrollBar) {
                            self->GetInternalArrangeOffsetY() = -self->VerticalScrollPos;
                            self->_MeasureAndArrangeWithoutResize(*layout, self->ClientRect->GetSize());
                        }
                    })};

        /**
         * @brief 横向滚动条可设置的最大位置
         */
        const ReadOnlyProperty<double> HorizontalScrollLimit{
            Property<double>::Init(this)
                .Getter([](Layer *self) -> double {
                    if (self->_horizontalScrollDisabled) {
                        return 0;
                    }
                    SCROLLINFO info{};
                    info.cbSize = sizeof(info);
                    info.fMask  = SIF_RANGE | SIF_PAGE;
                    GetScrollInfo(self->Handle, SB_HORZ, &info);
                    return Dip::PxToDipX(info.nMax - info.nPage + 1);
                })};

        /**
         * @brief 纵向滚动条可设置的最大位置
         */
        const ReadOnlyProperty<double> VerticalScrollLimit{
            Property<double>::Init(this)
                .Getter([](Layer *self) -> double {
                    if (self->_verticalScrollDisabled) {
                        return 0;
                    }
                    SCROLLINFO info{};
                    info.cbSize = sizeof(info);
                    info.fMask  = SIF_RANGE | SIF_PAGE;
                    GetScrollInfo(self->Handle, SB_VERT, &info);
                    return Dip::PxToDipY(info.nMax - info.nPage + 1);
                })};

        /**
         * @brief 是否在鼠标滚动时滚动内容
         */
        const Property<bool> MouseWheelScrollEnabled{
            Property<bool>::Init(this)
                .Getter([](Layer *self) -> bool {
                    return self->_mouseWheelScrollEnabled;
                })
                .Setter([](Layer *self, bool value) {
                    self->_mouseWheelScrollEnabled = value;
                })};

    protected:
        /**
         * @brief 更新布局
         */
        void UpdateLayout()
        {
            LayoutHost *layout = _GetLayout();

            if (layout == nullptr) {
                _MeasureAndArrangeWithoutLayout();
            } else {
                _MeasureAndArrangeWithoutResize(*layout, this->ClientRect->GetSize());
            }

            UpdateScrollRange();
            // Redraw();
        }

        /**
         * @brief 获取默认布局对象
         */
        virtual LayoutHost *GetDefaultLayout()
        {
            return nullptr;
        }

        /**
         * @brief           触发滚动条相关事件时调用该函数
         * @param scrollbar 滚动条类型
         * @param event     滚动条事件
         * @param pos       当event为ThumbPosition或ThubmTrack时表示当前滚动条位置，其他情况固定为0
         */
        virtual void OnScroll(ScrollOrientation scrollbar, ScrollEvent event, double pos)
        {
            ScrollingEventArgs args(scrollbar, event, pos);
            this->RaiseRoutedEvent(args);

            if (args.cancel) {
                return;
            }

            if (scrollbar == ScrollOrientation::Horizontal) {
                // 水平滚动条
                switch (event) {
                    case ScrollEvent::ThubmTrack: {
                        HorizontalScrollPos = pos;
                        break;
                    }
                    case ScrollEvent::Left: {
                        ScrollToLeft();
                        break;
                    }
                    case ScrollEvent::Right: {
                        ScrollToRight();
                        break;
                    }
                    case ScrollEvent::PageLeft: {
                        ScrollHorizontal(-GetHorizontalScrollPageSize());
                        break;
                    }
                    case ScrollEvent::PageRight: {
                        ScrollHorizontal(GetHorizontalScrollPageSize());
                        break;
                    }
                    case ScrollEvent::LineLeft: {
                        ScrollHorizontal(-_LayerScrollBarLineInterval);
                        break;
                    }
                    case ScrollEvent::LineRight: {
                        ScrollHorizontal(_LayerScrollBarLineInterval);
                        break;
                    }
                    default: {
                        break;
                    }
                }
            } else {
                // 垂直滚动条
                switch (event) {
                    case ScrollEvent::ThubmTrack: {
                        VerticalScrollPos = pos;
                        break;
                    }
                    case ScrollEvent::Bottom: {
                        ScrollToBottom();
                        break;
                    }
                    case ScrollEvent::Top: {
                        ScrollToTop();
                        break;
                    }
                    case ScrollEvent::PageUp: {
                        ScrollVertical(-GetVerticalScrollPageSize());
                        break;
                    }
                    case ScrollEvent::PageDown: {
                        ScrollVertical(GetVerticalScrollPageSize());
                        break;
                    }
                    case ScrollEvent::LineUp: {
                        ScrollVertical(-_LayerScrollBarLineInterval);
                        break;
                    }
                    case ScrollEvent::LineDown: {
                        ScrollVertical(_LayerScrollBarLineInterval);
                        break;
                    }
                    default: {
                        break;
                    }
                }
            }
        }

        /**
         * @brief       接收到WM_VSCROLL时调用目标控件的该函数
         * @param event 事件类型，即消息wParam的低字
         * @param pos   当前滚动条的位置，仅当event为SB_THUMBPOSITION或SB_THUMBTRACK时有效
         * @return      若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnVerticalScroll(int event, int pos) override
        {
            OnScroll(ScrollOrientation::Vertical, (ScrollEvent)event,
                     (event == SB_THUMBTRACK || event == SB_THUMBPOSITION) ? Dip::PxToDipY(pos) : (0.0));
            return true;
        }

        /**
         * @brief       接收到WM_HSCROLL时调用目标控件的该函数
         * @param event 事件类型，即消息wParam的低字
         * @param pos   当前滚动条的位置，仅当event为SB_THUMBPOSITION或SB_THUMBTRACK时有效
         * @return      若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnHorizontalScroll(int event, int pos) override
        {
            OnScroll(ScrollOrientation::Horizontal, (ScrollEvent)event,
                     (event == SB_THUMBTRACK || event == SB_THUMBPOSITION) ? Dip::PxToDipX(pos) : (0.0));
            return true;
        }

        /**
         * @brief           安排元素位置
         * @param finalSize 最终元素所安排的位置
         */
        virtual void Arrange(const sw::Rect &finalPosition) override
        {
            TBase::Arrange(finalPosition);
            UpdateScrollRange();
        }

        /**
         * @brief               测量元素所需尺寸，无需考虑边框和边距
         * @param availableSize 可用的尺寸
         * @return              返回元素需要占用的尺寸
         */
        virtual Size MeasureOverride(const Size &availableSize) override
        {
            LayoutHost *layout = _GetLayout();

            // 未设置布局时无法使用自动尺寸
            // 若未使用自动尺寸，则按照普通元素measure
            if (layout == nullptr || !_autoSize) {
                return TBase::MeasureOverride(availableSize);
            }

            return layout->MeasureOverride(availableSize);
        }

        /**
         * @brief           安排子元素的位置，可重写该函数以实现自定义布局
         * @param finalSize 可用于排列子元素的最终尺寸
         */
        virtual void ArrangeOverride(const Size &finalSize) override
        {
            LayoutHost *layout = _GetLayout();

            if (layout == nullptr) {
                // 未设置布局方式，此时需要对子元素进行Measure和Arrange
                _MeasureAndArrangeWithoutLayout();
            } else if (!_autoSize) {
                // 已设置布局方式，但是AutoSize被取消，此时子元素也未Measure
                _MeasureAndArrangeWithoutResize(*layout, finalSize);
            } else {
                // 已设置布局方式且AutoSize为true，此时子元素已Measure，调用Arrange即可
                layout->ArrangeOverride(finalSize);
            }
        }

        /**
         * @brief            尝试将指定的矩形区域移动到可视区域内
         * @param screenRect 要移动到可视区域的矩形在屏幕中的位置
         * @return           若已处理该请求则返回true，否则返回false以继续向上冒泡
         */
        virtual bool RequestBringIntoView(const sw::Rect &screenRect) override
        {
            bool handled = false;

            sw::Point pos = this->PointFromScreen(screenRect.GetPos());
            sw::Rect rect = {pos.x, pos.y, screenRect.width, screenRect.height};

            sw::Rect clientRect = this->ClientRect;

            if (VerticalScrollBar) {
                double curPos = VerticalScrollPos;
                if (rect.top < curPos) {
                    VerticalScrollPos = rect.top;
                } else if (rect.top + rect.height > curPos + clientRect.height) {
                    if (rect.height >= clientRect.height) {
                        VerticalScrollPos = rect.top;
                    } else {
                        VerticalScrollPos = rect.top + rect.height - clientRect.height;
                    }
                }
                handled = true;
            }

            if (HorizontalScrollBar) {
                double curPos = HorizontalScrollPos;
                if (rect.left < curPos) {
                    HorizontalScrollPos = rect.left;
                } else if (rect.left + rect.width > curPos + clientRect.width) {
                    if (rect.width >= clientRect.width) {
                        HorizontalScrollPos = rect.left;
                    } else {
                        HorizontalScrollPos = rect.left + rect.width - clientRect.width;
                    }
                }
                handled = true;
            }

            return handled;
        }

        /**
         * @brief           路由事件经过当前元素时调用该函数
         * @param eventArgs 事件参数
         * @param handler   事件处理函数，值为空时表示当前元素没有注册该事件处理函数
         */
        virtual void OnRoutedEvent(RoutedEventArgs &eventArgs, const RoutedEventHandler &handler) override
        {
            TBase::OnRoutedEvent(eventArgs, handler);

            if (!eventArgs.handled &&
                eventArgs.eventType == UIElement_MouseWheel && _mouseWheelScrollEnabled) //
            {
                auto &wheelArgs = static_cast<MouseWheelEventArgs &>(eventArgs);
                bool shiftDown  = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
                double offset   = -std::copysign(_LayerScrollBarLineInterval, wheelArgs.wheelDelta);
                if (shiftDown) {
                    if (HorizontalScrollBar) {
                        ScrollHorizontal(offset);
                        eventArgs.handled = true;
                    }
                } else {
                    if (VerticalScrollBar) {
                        ScrollVertical(offset);
                        eventArgs.handled = true;
                    }
                }
            }
        }

    public:
        /**
         * @brief        获取横向滚动条的范围
         * @param refMin 滚动范围最小值
         * @param refMax 滚动范围最大值
         */
        void GetHorizontalScrollRange(double &refMin, double &refMax)
        {
            INT nMin = 0, nMax = 0;
            GetScrollRange(this->Handle, SB_HORZ, &nMin, &nMax);

            refMin = Dip::PxToDipX(nMin);
            refMax = Dip::PxToDipX(nMax);
        }

        /**
         * @brief        获取纵向滚动条的范围
         * @param refMin 滚动范围最小值
         * @param refMax 滚动范围最大值
         */
        void GetVerticalScrollRange(double &refMin, double &refMax)
        {
            INT nMin = 0, nMax = 0;
            GetScrollRange(this->Handle, SB_VERT, &nMin, &nMax);

            refMin = Dip::PxToDipY(nMin);
            refMax = Dip::PxToDipY(nMax);
        }

        /**
         * @brief     设置横向滚动条的范围
         * @param min 滚动范围最小值
         * @param max 滚动范围最大值
         */
        void SetHorizontalScrollRange(double min, double max)
        {
            SCROLLINFO info{};
            info.cbSize = sizeof(info);
            info.fMask  = SIF_RANGE | SIF_PAGE;
            info.nMin   = Dip::DipToPxX(min);
            info.nMax   = Dip::DipToPxX(max);
            info.nPage  = Dip::DipToPxX(this->ClientWidth);

            SetScrollInfo(this->Handle, SB_HORZ, &info, true);
        }

        /**
         * @brief     设置纵向滚动条的范围
         * @param min 滚动范围最小值
         * @param max 滚动范围最大值
         */
        void SetVerticalScrollRange(double min, double max)
        {
            SCROLLINFO info{};
            info.cbSize = sizeof(info);
            info.fMask  = SIF_RANGE | SIF_PAGE;
            info.nMin   = Dip::DipToPxY(min);
            info.nMax   = Dip::DipToPxY(max);
            info.nPage  = Dip::DipToPxY(this->ClientHeight);

            SetScrollInfo(this->Handle, SB_VERT, &info, true);
        }

        /**
         * @brief 获取水平滚动条滚动页面大小
         */
        double GetHorizontalScrollPageSize()
        {
            SCROLLINFO info{};
            info.cbSize = sizeof(info);
            info.fMask  = SIF_PAGE;
            GetScrollInfo(this->Handle, SB_HORZ, &info);
            return Dip::PxToDipX(info.nPage);
        }

        /**
         * @brief 获取垂直滚动条滚动页面大小
         */
        double GetVerticalScrollPageSize()
        {
            SCROLLINFO info{};
            info.cbSize = sizeof(info);
            info.fMask  = SIF_PAGE;
            GetScrollInfo(this->Handle, SB_VERT, &info);
            return Dip::PxToDipY(info.nPage);
        }

        /**
         * @brief 设置水平滚动条滚动页面大小
         */
        void SetHorizontalScrollPageSize(double pageSize)
        {
            SCROLLINFO info{};
            info.cbSize = sizeof(info);
            info.fMask  = SIF_PAGE;
            info.nPage  = Dip::DipToPxX(pageSize);
            SetScrollInfo(this->Handle, SB_HORZ, &info, true);
        }

        /**
         * @brief 设置垂直滚动条滚动页面大小
         */
        void SetVerticalScrollPageSize(double pageSize)
        {
            SCROLLINFO info{};
            info.cbSize = sizeof(info);
            info.fMask  = SIF_PAGE;
            info.nPage  = Dip::DipToPxY(pageSize);
            SetScrollInfo(this->Handle, SB_VERT, &info, true);
        }

        /**
         * @brief 根据子元素更新滚动条范围，未设定布局方式时该函数无效
         */
        void UpdateScrollRange()
        {
            if (_GetLayout() == nullptr) {
                // 当未设置布局方式时滚动条和控件位置需要手动设置
                // 将以下俩字段设为false确保xxxScrollLimit属性在未设置布局方式时仍可用
                _horizontalScrollDisabled = false;
                _verticalScrollDisabled   = false;
                return;
            }

            if (HorizontalScrollBar) {
                double childRightmost = this->GetChildRightmost(true);

                if (int(childRightmost - this->ClientWidth) > 0) {
                    _horizontalScrollDisabled = false;
                    EnableScrollBar(this->Handle, SB_HORZ, ESB_ENABLE_BOTH);
                    SetHorizontalScrollRange(0, childRightmost);

                    // 当尺寸改变时确保子元素位置与滚动条同步
                    double pos = HorizontalScrollPos;
                    if (-this->GetInternalArrangeOffsetX() > pos) {
                        HorizontalScrollPos = pos;
                    }

                } else {
                    HorizontalScrollPos = 0;
                    EnableScrollBar(this->Handle, SB_HORZ, ESB_DISABLE_BOTH);
                    _horizontalScrollDisabled = true;
                }
            }

            if (VerticalScrollBar) {
                double childBottommost = this->GetChildBottommost(true);

                if (int(childBottommost - this->ClientHeight) > 0) {
                    _verticalScrollDisabled = false;
                    EnableScrollBar(this->Handle, SB_VERT, ESB_ENABLE_BOTH);
                    SetVerticalScrollRange(0, childBottommost);

                    // 当尺寸改变时确保子元素位置与滚动条同步
                    double pos = VerticalScrollPos;
                    if (-this->GetInternalArrangeOffsetY() > pos) {
                        VerticalScrollPos = pos;
                    }

                } else {
                    VerticalScrollPos = 0;
                    EnableScrollBar(this->Handle, SB_VERT, ESB_DISABLE_BOTH);
                    _verticalScrollDisabled = true;
                }
            }
        }

        /**
         * @brief 将垂直滚动条移动到顶部
         */
        void ScrollToTop()
        {
            VerticalScrollPos = 0;
        }

        /**
         * @brief 将垂直滚动条移动到底部
         */
        void ScrollToBottom()
        {
            VerticalScrollPos = VerticalScrollLimit;
        }

        /**
         * @brief 将水平滚动条移动到最左
         */
        void ScrollToLeft()
        {
            HorizontalScrollPos = 0;
        }

        /**
         * @brief 将水平滚动条移动到最右
         */
        void ScrollToRight()
        {
            HorizontalScrollPos = HorizontalScrollLimit;
        }

        /**
         * @brief        水平滚动
         * @param offset 滚动的偏移量
         */
        void ScrollHorizontal(double offset)
        {
            HorizontalScrollPos += offset;
        }

        /**
         * @brief        垂直滚动
         * @param offset 滚动的偏移量
         */
        void ScrollVertical(double offset)
        {
            VerticalScrollPos += offset;
        }

    private:
        /**
         * @brief 获取布局对象，若Layout属性被赋值则返回设置的对象，否则返回默认布局对象
         */
        LayoutHost *_GetLayout()
        {

            auto layout = (_customLayout != nullptr) ? _customLayout : GetDefaultLayout();
            return (layout != nullptr && layout->IsAssociated(this)) ? layout : nullptr;
        }

        /**
         * @brief 在没有设定布局方式时，使用该函数对子元素Measure和Arrange
         */
        void _MeasureAndArrangeWithoutLayout()
        {
            this->GetInternalArrangeOffsetX() = 0;
            this->GetInternalArrangeOffsetY() = 0;

            int childCount = this->GetChildLayoutCount();

            for (int i = 0; i < childCount; ++i) {
                // measure
                UIElement &item = static_cast<UIElement &>(this->GetChildLayoutAt(i));
                item.Measure(Size{INFINITY, INFINITY});
                // arrange
                Size desireSize      = item.GetDesireSize();
                sw::Rect itemRect    = item.Rect;
                Thickness itemMargin = item.Margin;
                item.Arrange(sw::Rect{itemRect.left - itemMargin.left, itemRect.top - itemMargin.top, desireSize.width, desireSize.height});
            }
        }

        /**
         * @brief 使用设定的布局方式对子元素进行Measure和Arrange，不改变当前的尺寸和DesireSize
         */
        void _MeasureAndArrangeWithoutResize(LayoutHost &layout, const Size &clientSize)
        {
            if (layout.IsAssociated(this)) {
                layout.MeasureOverride(clientSize);
                layout.ArrangeOverride(clientSize);
            }
        }
    };

    /*================================================================================*/

    extern template class Layer<UIElement>;
}
