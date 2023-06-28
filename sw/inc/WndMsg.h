#pragma once

#include <Windows.h>

namespace sw
{
    /**
     * @brief 包含SimpleWindow用到的一些窗口消息
     */
    enum WndMsg : UINT {

        /**
         * @brief SimpleWindow所用消息的起始位置
         */
        WM_SimpleWindowBegin = WM_USER + 0x3000,

        /**
         * @brief 控件布局发生变化时控件所在顶级窗口将收到该消息，wParam和lParam均未使用
         */
        WM_LayoutUpdated,

        /**
         * @brief 父窗口接收到WM_COMMAND时向控件发该消息，wParam和lParam按原样发回
         */
        WM_ParentReceivedCommand,
    };
}
