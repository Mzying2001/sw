#pragma once

#include "LayoutHost.h"

namespace sw
{
    /**
     * @brief 均匀大小网格布局
     */
    class UniformGridLayout : public LayoutHost
    {
    public:
        /**
         * @brief 行数
         */
        int rows = 1;

        /**
         * @brief 列数
         */
        int columns = 1;

        /**
         * @brief 网格第一行中前导空白单元格的数量
         */
        int firstColumn = 0;

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
