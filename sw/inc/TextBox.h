#pragma once

#include "Control.h"

namespace sw
{
    class TextBox : public Control
    {
    private:
        /**
         * @brief 在读取Text属性时用于判断是否需要更新储存的文本
         */
        bool _isTextChanged = false;

    public:
        TextBox();

        /**
         * @brief 是否只读
         */
        const Property<bool> ReadOnly;

        /**
         * @brief 是否允许多行文本
         */
        const Property<bool> MultiLine;

    protected:
        /**
         * @brief  获取窗口文本
         * @return _text字段
         */
        virtual std::wstring &GetText() override;

        /**
         * @brief      当父窗口接收到控件的WM_COMMAND时调用该函数
         * @param code 通知代码
         */
        virtual void OnCommand(int code) override;
    };
}
