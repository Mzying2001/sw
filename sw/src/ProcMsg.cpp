#include "ProcMsg.h"

sw::ProcMsg::ProcMsg(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    : hwnd(hwnd), uMsg(uMsg), wParam(wParam), lParam(lParam)
{
}
