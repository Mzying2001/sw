#pragma once

#include "LayoutHost.h"

namespace sw
{
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
         * @brief 计算所需尺寸
         */
        virtual void MeasureOverride(Size &availableSize) override;

        /**
         * @brief 安排控件
         */
        virtual void ArrangeOverride(Size &finalSize) override;
    };
}
