#pragma once

#include "AbsoluteLayout.h"
#include "LayoutHost.h"
#include "UIElement.h"

namespace sw
{
    class Layer : public UIElement
    {
    private:
        bool _layoutDisabled          = false;
        LayoutHost *_layout           = nullptr;
        AbsoluteLayout _defaultLayout = AbsoluteLayout();

    public:
        /**
         * @brief 窗口布局方式
         */
        const Property<LayoutHost *> Layout;

    public:
        Layer();

    protected:
        /**
         * @brief               测量控件所需尺寸
         * @param availableSize 可用的尺寸
         */
        virtual void Measure(const Size &availableSize);

        /**
         * @brief           安排控件位置
         * @param finalSize 最终控件所安排的位置
         */
        virtual void Arrange(const sw::Rect &finalPosition);

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
         * @brief 禁用布局
         */
        void DisableLayout();

        /**
         * @brief 启用布局
         */
        void EnableLayout();
    };
}