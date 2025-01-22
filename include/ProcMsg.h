#pragma once

#include <Windows.h>

namespace sw
{
    /**
     * @brief 对Windows窗口消息的封装
     */
    struct ProcMsg {
        /**
         * @brief 接收到消息的窗口句柄
         */
        HWND hwnd;

        /**
         * @brief 消息类型
         */
        UINT uMsg;

        /**
         * @brief 消息的附加信息
         */
        WPARAM wParam;

        /**
         * @brief 消息的附加信息
         */
        LPARAM lParam;

        /**
         * @brief 构造所有字段均为0的ProcMsg
         */
        ProcMsg();

        /**
         * @brief 构造ProcMsg
         */
        ProcMsg(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    };
}
