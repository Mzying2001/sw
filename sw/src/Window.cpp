#include "Window.h"

static unsigned int _windowCount = 0;

bool sw::Window::PostQuitWhenAllClosed = true;

sw::Window::Window()
    : WndBase()
{
    InitWndBase(
        NULL,                // Optional window styles
        L"Window",           // Window text
        WS_OVERLAPPEDWINDOW, // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        NULL,  // Parent window
        NULL); // Menu
}

LRESULT sw::Window::WndProc(const ProcMsg &refMsg)
{
    /*switch (refMsg.uMsg) {
        default: {
            return this->WndBase::WndProc(refMsg);
        }
    }*/
    return this->WndBase::WndProc(refMsg);
}

bool sw::Window::OnCreate()
{
    ++_windowCount;
    return true;
}

bool sw::Window::OnDestroy()
{
    if (!--_windowCount && PostQuitWhenAllClosed) {
        PostQuitMessage(0);
    }
    return true;
}

bool sw::Window::OnPaint()
{
    PAINTSTRUCT ps{};
    HDC hdc = BeginPaint(this->Handle, &ps);
    FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
    EndPaint(this->Handle, &ps);
    return true;
}

void sw::Window::Show()
{
    ShowWindow(this->Handle, SW_SHOW);
}
