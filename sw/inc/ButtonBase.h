#pragma once

#include "Control.h"

namespace sw
{
    class ButtonBase : public Control
    {
    public:
        ButtonBase();

    protected:
        /**
         * @brief 初始化控件
         */
        void InitButtonBase(LPCWSTR lpWindowName, DWORD dwStyle);

        /**
         * @brief      接收到WM_ParentReceivedCommand消息时调用此函数
         * @param code 通知代码
         */
        virtual void ParentReceivedCommand(int code);
    };
}
