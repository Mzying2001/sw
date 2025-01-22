#pragma once

#include "LayoutHost.h"

namespace sw
{
    /**
     * @brief 纵向自动换行布局
     */
    class WrapLayoutV : virtual public LayoutHost
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
