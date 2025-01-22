#pragma once

#include "LayoutHost.h"

namespace sw
{
    /**
     * @brief 横向自动换行布局
     */
    class WrapLayoutH : virtual public LayoutHost
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
