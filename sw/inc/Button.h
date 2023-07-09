#pragma once

#include "ButtonBase.h"

namespace sw
{
    class Button : public ButtonBase
    {
    public:
        Button();

    protected:
        /**
         * @brief           接收到WM_SETFOCUS时调用该函数
         * @param hPreFocus 丢失焦点的hwnd，可能为NULL
         * @return          若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnSetFocus(HWND hPreFocus);

        /**
         * @brief            接收到WM_KILLFOCUS时调用该函数
         * @param hNextFocus 接收到焦点的hwnd，可能为NULL
         * @return           若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnKillFocus(HWND hNextFocus);
    };
}
