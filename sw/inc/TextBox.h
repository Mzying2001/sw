#pragma once

#include "TextBoxBase.h"

namespace sw
{
    class TextBox : public TextBoxBase
    {
    private:
        /**
         * @brief 是否自动换行
         */
        bool _autoWrap = false;

    public:
        /**
         * @brief 是否自动换行，仅在MultiLine属性为true时有效
         */
        const Property<bool> AutoWrap;

        /**
         * @brief 是否允许多行文本
         */
        const Property<bool> MultiLine;

        /**
         * @brief 是否显示横向滚动条
         */
        const Property<bool> HorizontalScrollBar;

        /**
         * @brief 是否显示纵向滚动条
         */
        const Property<bool> VerticalScrollBar;

    public:
        /**
         * @brief 初始化编辑框
         */
        TextBox();
    };
}
