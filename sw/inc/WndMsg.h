#pragma once

#include <windows.h>

namespace sw
{
    /**
     * @brief 包含SimpleWindow用到的一些窗口消息
     */
    enum WndMsg : UINT {
        // SimpleWindow所用消息的起始位置
        WM_SimpleWindowBegin = WM_USER + 0x3000,

        // 控件布局发生变化时控件所在顶级窗口将收到该消息，wParam和lParam均未使用
        WM_UpdateLayout,

        // 在窗口线程上执行指定委托，lParam为指向sw::Action<>的指针，wParam表示是否对委托指针执行delete
        WM_InvokeAction,

        // 在WndBase::SetParent函数中设置父窗口之前发送该消息，wParam为新的父窗口句柄，lParam未使用
        WM_PreSetParent,

        // SimpleWindow所用消息的结束位置
        WM_SimpleWindowEnd,
    };
}
