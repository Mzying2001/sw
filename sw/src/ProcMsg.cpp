#include "ProcMsg.h"

sw::ProcMsg::ProcMsg()
    : ProcMsg(NULL, 0, 0, 0)
{
}

sw::ProcMsg::ProcMsg(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    : hwnd(hwnd), uMsg(uMsg), wParam(wParam), lParam(lParam)
{
}
