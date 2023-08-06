#pragma once

#include "TextBoxBase.h"

namespace sw
{
    class PasswordBox : public TextBoxBase
    {
    public:
        /**
         * @brief 密码框显示的字符
         */
        const Property<wchar_t> PasswordChar;

    public:
        /**
         * @brief 初始化密码框
         */
        PasswordBox();
    };
}
