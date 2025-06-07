#pragma once

#include <Windows.h>

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

        // 在窗口线程上执行指定回调函数，lParam为指向std::function<void()>的指针，wParam表示是否对函数对象指针执行delete
        WM_InvokeFunction,

        // 在窗口线程上执行指定委托，lParam为指向sw::Action<>的指针，wParam表示是否对委托指针执行delete
        WM_InvokeAction,

        // SimpleWindow所用消息的结束位置
        WM_SimpleWindowEnd,
    };
}
