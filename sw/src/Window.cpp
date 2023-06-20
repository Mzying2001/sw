#include "Window.h"

static unsigned int _windowCount = 0;

bool sw::Window::PostQuitWhenAllClosed = true;

sw::Window::Window()
    : WndBase(
          NULL,
          L"Window",
          WS_OVERLAPPEDWINDOW,
          CW_USEDEFAULT,
          CW_USEDEFAULT,
          CW_USEDEFAULT,
          CW_USEDEFAULT,
          NULL,
          NULL,
          NULL)
{
    ++_windowCount;
}

LRESULT sw::Window::WndProc(const ProcMsg &refMsg)
{
    switch (refMsg.uMsg) {
        case WM_DESTROY: {
            this->OnDestroy();
            return 0;
        }

        case WM_PAINT: {
            this->OnPaint();
            return 0;
        }

        default: {
            return this->WndBase::WndProc(refMsg);
        }
    }
}

void sw::Window::OnDestroy()
{
    if (!--_windowCount && PostQuitWhenAllClosed) {
        PostQuitMessage(0);
    }
}

void sw::Window::OnPaint()
{
    PAINTSTRUCT ps{};
    HDC hdc = BeginPaint(this->Handle, &ps);
    FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
    EndPaint(this->Handle, &ps);
}

void sw::Window::Show()
{
    ShowWindow(this->Handle, SW_SHOW);
}
