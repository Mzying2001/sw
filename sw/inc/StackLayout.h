#pragma once

#include "Alignment.h"
#include "StackLayoutH.h"
#include "StackLayoutV.h"

namespace sw
{
    /**
     * @brief 堆叠布局
     */
    class StackLayout : public StackLayoutH, public StackLayoutV
    {
    public:
        /**
         * @brief 排列方式
         */
        Orientation orientation = Orientation::Vertical;

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
