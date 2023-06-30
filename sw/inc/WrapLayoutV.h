#pragma once

#include "LayoutHost.h"

namespace sw
{
    class WrapLayoutV : virtual public LayoutHost
    {
    public:
        /**
         * @brief 计算所需尺寸
         */
        virtual void MeasureOverride(Size &availableSize);

        /**
         * @brief 安排控件
         */
        virtual void ArrangeOverride(Size &finalSize);
    };
}