#pragma once

#include "Control.h"
#include "Layer.h"

namespace sw
{
    class Panel : public Control, public Layer
    {
    public:
        Panel();

        /**
         * @brief               测量控件所需尺寸
         * @param availableSize 可用的尺寸
         */
        virtual void Measure(const Size &availableSize);

        /**
         * @brief           安排控件位置
         * @param finalSize 最终控件所安排的位置
         */
        virtual void Arrange(const sw::Rect &finalPosition);
    };
}
