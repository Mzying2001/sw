#pragma once

#include "AbsoluteLayout.h"
#include "LayoutHost.h"
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

    public:
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
         * @brief 禁用布局
         */
        void DisableLayout();

        /**
         * @brief 启用布局
         */
        void EnableLayout();
    };
}