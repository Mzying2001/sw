#pragma once

#include <Windows.h>

namespace sw
{
    struct ProcMsg {
        HWND hwnd;
        UINT uMsg;
        WPARAM wParam;
        LPARAM lParam;

        ProcMsg();
        ProcMsg(const HWND &hwnd, const UINT &uMsg, const WPARAM &wParam, const LPARAM &lParam);
    };
}
