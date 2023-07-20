#pragma once

#include "Alignment.h"
#include "Control.h"

namespace sw
{
    class Label : public Control
    {
    public:
        /**
         * @brief 文本的水平对齐方式，可设为左对齐、中心对齐、右对齐
         */
        const Property<sw::HorizontalAlignment> HorizontalContentAlignment;

        /**
         * @brief 文本的垂直对齐方式，仅支持顶部对齐和中心对齐
         */
        const Property<sw::VerticalAlignment> VerticalContentAlignment;

    public:
        Label();
    };
}
