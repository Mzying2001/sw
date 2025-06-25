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
         * @brief               测量元素所需尺寸，无需考虑边框和边距
         * @param availableSize 可用的尺寸
         * @return              返回元素需要占用的尺寸
         */
        virtual Size MeasureOverride(const Size &availableSize) override;

        /**
         * @brief           安排子元素的位置，可重写该函数以实现自定义布局
         * @param finalSize 可用于排列子元素的最终尺寸
         */
        virtual void ArrangeOverride(const Size &finalSize) override;
    };
}
