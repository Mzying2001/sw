#pragma once

#include "LayoutHost.h"

namespace sw
{
    class WrapLayout : public LayoutHost
    {
    public:
        /**
         * @brief               测量控件所需尺寸
         * @param availableSize 可用的尺寸
         */
        virtual void Measure(const Size &availableSize);

        /**
         * @brief               安排控件位置
         * @param finalPosition 最终控件所安排的位置
         */
        virtual void Arrange(const Rect &finalPosition);
    };
}