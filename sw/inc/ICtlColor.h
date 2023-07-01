#pragma once

#include <Windows.h>

namespace sw
{
    /**
     * @brief CtlColor接口，在父窗口接收到WM_CTLCOLORxxx消息时调用该接口
     */
    class ICtlColor
    {
    public:
        /**
         * @brief 父窗口接收到WM_CTLCOLORxxx的回调
         */
        virtual LRESULT CtlColor(HDC hdc, HWND hwnd) = 0;
    };
}
