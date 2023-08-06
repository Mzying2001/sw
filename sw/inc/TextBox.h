#pragma once

#include "TextBoxBase.h"

namespace sw
{
    class TextBox : public TextBoxBase
    {
    public:
        /**
         * @brief 是否允许多行文本
         */
        const Property<bool> MultiLine;

    public:
        /**
         * @brief 初始化编辑框
         */
        TextBox();
    };
}
