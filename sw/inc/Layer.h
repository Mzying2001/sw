#pragma once

#include "AbsoluteLayout.h"
#include "LayoutHost.h"
#include "ScrollEnums.h"
#include "UIElement.h"

namespace sw
{
    class Layer : virtual public UIElement
    {
    private:
        /**
         * @brief 是否关闭布局，当该字段为true时调用UpdateLayout不会更新布局，可以用DisableLayout和EnableLayout设置该字段
         */
        bool _layoutDisabled = false;

        /**
         * @brief 指向所使用布局方式对象的指针，调用GetLayoutHost获取所指向的对象，当该字段为nullptr时GetLayoutHost获取到的是_defaultLayout
         */
        LayoutHost *_layout = nullptr;

        /**
         * @brief 默认的布局方式，当Layout为nullptr时使用该布局
         */
        AbsoluteLayout _defaultLayout = AbsoluteLayout();

        /**
         * @brief 记录水平滚动条是否已被禁止
         */
        bool _horizontalScrollDisabled = true;

        /**
         * @brief 记录垂直滚动条是否已被禁止
         */
        bool _verticalScrollDisabled = true;

    public:
        /**
         * @brief 窗口布局方式，赋值后将自动与所指向的布局关联，每个布局只能关联一个对象，设置为nullptr可恢复默认布局
         */
        const Property<LayoutHost *> Layout;

        /**
         * @brief 是否显示横向滚动条
         */
        const Property<bool> HorizontalScrollBar;

        /**
         * @brief 是否显示纵向滚动条
         */
        const Property<bool> VerticalScrollBar;

        /**
         * @brief 横向滚动条位置
         */
        const Property<double> HorizontalScrollPos;

        /**
         * @brief 纵向滚动条位置
         */
        const Property<double> VerticalScrollPos;

        /**
         * @brief 横向滚动条可设置的最大位置
         */
        const ReadOnlyProperty<double> HorizontalScrollLimit;

        /**
         * @brief 纵向滚动条可设置的最大位置
         */
        const ReadOnlyProperty<double> VerticalScrollLimit;

    public:
        /**
         * @brief 初始化Layer
         */
        Layer();

    protected:
        /**
         * @brief 获取Layout，若Layout为空则返回默认Layout
         */
        LayoutHost &GetLayoutHost();

        /**
         * @brief 更新布局
         */
        void UpdateLayout();

        /**
         * @brief           触发滚动条相关事件时调用该函数
         * @param scrollbar 滚动条类型
         * @param event     滚动条事件
         * @param pos       当event为ThumbPosition或ThubmTrack时表示当前滚动条位置，其他情况固定为0
         */
        virtual void OnScroll(ScrollOrientation scrollbar, ScrollEvent event, double pos);

        /**
         * @brief       接收到WM_VSCROLL时调用目标控件的该函数
         * @param event 事件类型，即消息wParam的低字
         * @param pos   当前滚动条的位置，仅当event为SB_THUMBPOSITION或SB_THUMBTRACK时有效
         * @return      若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnVerticalScroll(int event, int pos) override;

        /**
         * @brief       接收到WM_HSCROLL时调用目标控件的该函数
         * @param event 事件类型，即消息wParam的低字
         * @param pos   当前滚动条的位置，仅当event为SB_THUMBPOSITION或SB_THUMBTRACK时有效
         * @return      若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnHorizontalScroll(int event, int pos) override;

    public:
        /**
         * @brief               测量控件所需尺寸
         * @param availableSize 可用的尺寸
         */
        virtual void Measure(const Size &availableSize) override;

        /**
         * @brief           安排控件位置
         * @param finalSize 最终控件所安排的位置
         */
        virtual void Arrange(const sw::Rect &finalPosition) override;

        /**
         * @brief 获取一个bool值，表示当前使用布局方式是否为绝对布局
         */
        bool IsUsingAbsoluteLayout();

        /**
         * @brief 禁用布局
         */
        void DisableLayout();

        /**
         * @brief 启用布局
         */
        void EnableLayout();

        /**
         * @brief        获取横向滚动条的范围
         * @param refMin 滚动范围最小值
         * @param refMax 滚动范围最大值
         */
        void GetHorizontalScrollRange(double &refMin, double &refMax);

        /**
         * @brief        获取纵向滚动条的范围
         * @param refMin 滚动范围最小值
         * @param refMax 滚动范围最大值
         */
        void GetVerticalScrollRange(double &refMin, double &refMax);

        /**
         * @brief     设置横向滚动条的范围
         * @param min 滚动范围最小值
         * @param max 滚动范围最大值
         */
        void SetHorizontalScrollRange(double min, double max);

        /**
         * @brief     设置纵向滚动条的范围
         * @param min 滚动范围最小值
         * @param max 滚动范围最大值
         */
        void SetVerticalScrollRange(double min, double max);

        /**
         * @brief 获取水平滚动条滚动页面大小
         */
        double GetHorizontalScrollPageSize();

        /**
         * @brief 获取垂直滚动条滚动页面大小
         */
        double GetVerticalScrollPageSize();

        /**
         * @brief 设置水平滚动条滚动页面大小
         */
        void SetHorizontalScrollPageSize(double pageSize);

        /**
         * @brief 设置垂直滚动条滚动页面大小
         */
        void SetVerticalScrollPageSize(double pageSize);

        /**
         * @brief 根据子元素更新滚动条范围，当使用绝对布局时该函数无效
         */
        void UpdateScrollRange();

        /**
         * @brief 将垂直滚动条移动到顶部
         */
        void ScrollToTop();

        /**
         * @brief 将垂直滚动条移动到底部
         */
        void ScrollToBottom();

        /**
         * @brief 将水平滚动条移动到最左
         */
        void ScrollToLeft();

        /**
         * @brief 将水平滚动条移动到最右
         */
        void ScrollToRight();
    };
}
