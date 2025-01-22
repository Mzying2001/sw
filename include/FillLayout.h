#pragma once

#include "LayoutHost.h"

namespace sw
{
    /**
     * @brief 一种将全部元素都铺满的布局，一般用于在只有一个子元素的时候将该元素铺满整个可用区域
     */
    class FillLayout : public LayoutHost
    {
    public:
        /**
         * @brief 计算所需尺寸
         */
        virtual void MeasureOverride(Size &availableSize) override;

        /**
         * @brief 安排控件
         */
        virtual void ArrangeOverride(Size &finalSize) override;
    };
}
