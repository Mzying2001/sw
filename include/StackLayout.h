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
         * @brief 计算所需尺寸
         */
        virtual void MeasureOverride(Size &availableSize) override;

        /**
         * @brief 安排控件
         */
        virtual void ArrangeOverride(Size &finalSize) override;
    };
}
