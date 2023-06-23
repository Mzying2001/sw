#include "Window.h"

static unsigned int _windowCount = 0;

bool sw::Window::PostQuitWhenAllClosed = true;

sw::Window::Window()
    : WndBase(),

      State(
          // get
          [&]() -> const WindowState & {
              static WindowState state;
              HWND hwnd = this->Handle;
              if (IsIconic(hwnd)) {
                  state = WindowState::Minimized;
              } else if (IsZoomed(hwnd)) {
                  state = WindowState::Maximized;
              } else {
                  state = WindowState::Normal;
              }
              return state;
          },
          // set
          [&](const WindowState &value) {
              HWND hwnd = this->Handle;
              switch (value) {
                  case WindowState::Normal:
                      ShowWindow(hwnd, SW_RESTORE);
                      break;
                  case WindowState::Minimized:
                      ShowWindow(hwnd, SW_MINIMIZE);
                      break;
                  case WindowState::Maximized:
                      ShowWindow(hwnd, SW_MAXIMIZE);
                      break;
              }
          })
{
    InitWndBase(
        NULL,                // Optional window styles
        L"Window",           // Window text
        WS_OVERLAPPEDWINDOW, // Window style
        NULL,                // Parent window
        NULL);               // Menu
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

void sw::Window::Show(int nCmdShow)
{
    if (_isFirstShow) {
        _isFirstShow = false;
        if (this->StartupLocation == CenterScreen) {
            sw::Rect rect = this->Rect;
            rect.left     = (Screen::Width - rect.width) / 2;
            rect.top      = (Screen::Height - rect.height) / 2;
            this->Rect    = rect;
        }
    }
    this->WndBase::Show(nCmdShow);
}

void sw::Window::Show()
{
    this->Show(SW_SHOW);
}
