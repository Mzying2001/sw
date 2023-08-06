#pragma once

#include "Control.h"

namespace sw
{
    class ButtonBase : public Control
    {
    protected:
        /**
         * @brief 初始化ButtonBase
         */
        ButtonBase();

        /**
         * @brief 初始化控件
         */
        void InitButtonBase(LPCWSTR lpWindowName, DWORD dwStyle, DWORD dwExStyle);

        /**
         * @brief      当父窗口接收到控件的WM_COMMAND时调用该函数
         * @param code 通知代码
         */
        virtual void OnCommand(int code) override;
    };
}
