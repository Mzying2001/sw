#include "Window.h"

static unsigned int _windowCount = 0;

bool sw::Window::PostQuitWhenAllClosed = true;

sw::Window::Window()
    : State(
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
          }),

      SizeBox(
          // get
          [&]() -> const bool & {
              static bool result;
              result = this->GetStyle(WS_SIZEBOX);
              return result;
          },
          // set
          [&](const bool &value) {
              this->SetStyle(WS_SIZEBOX, value);
          }),

      MaximizeBox(
          // get
          [&]() -> const bool & {
              static bool result;
              result = this->GetStyle(WS_MAXIMIZEBOX);
              return result;
          },
          // set
          [&](const bool &value) {
              this->SetStyle(WS_MAXIMIZEBOX, value);
          }),

      MinimizeBox(
          // get
          [&]() -> const bool & {
              static bool result;
              result = this->GetStyle(WS_MINIMIZEBOX);
              return result;
          },
          // set
          [&](const bool &value) {
              this->SetStyle(WS_MINIMIZEBOX, value);
          }),

      Topmost(
          // get
          [&]() -> const bool & {
              static bool result;
              result = this->GetExtendedStyle(WS_EX_TOPMOST);
              return result;
          },
          // set
          [&](const bool &value) {
              /*this->SetExtendedStyle(WS_EX_TOPMOST, value);*/
              HWND hWndInsertAfter = value ? HWND_TOPMOST : HWND_NOTOPMOST;
              SetWindowPos(this->Handle, hWndInsertAfter, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
          })
{
    InitWndBase(
        NULL,                // Optional window styles
        L"Window",           // Window text
        WS_OVERLAPPEDWINDOW, // Window style
        NULL,                // Parent window
        NULL);               // Menu
}

bool sw::Window::GetStyle(LONG_PTR style)
{
    return GetWindowLongPtrW(this->Handle, GWL_STYLE) & style;
}

void sw::Window::SetStyle(LONG_PTR style, bool value)
{
    HWND hwnd = this->Handle;
    if (value) {
        style = GetWindowLongPtrW(hwnd, GWL_STYLE) | style;
    } else {
        style = GetWindowLongPtrW(hwnd, GWL_STYLE) & ~style;
    }
    SetWindowLongPtrW(hwnd, GWL_STYLE, style);
}

bool sw::Window::GetExtendedStyle(LONG_PTR style)
{
    return GetWindowLongPtrW(this->Handle, GWL_EXSTYLE) & style;
}

void sw::Window::SetExtendedStyle(LONG_PTR style, bool value)
{
    HWND hwnd = this->Handle;
    if (value) {
        style = GetWindowLongPtrW(hwnd, GWL_EXSTYLE) | style;
    } else {
        style = GetWindowLongPtrW(hwnd, GWL_EXSTYLE) & ~style;
    }
    SetWindowLongPtrW(hwnd, GWL_EXSTYLE, style);
}

LRESULT sw::Window::WndProc(const ProcMsg &refMsg)
{
    switch (refMsg.uMsg) {
        case WM_SHOWWINDOW: {
            if (this->_isFirstShow) {
                this->_isFirstShow = false;
                if (this->StartupLocation == WindowStartupLocation::CenterScreen) {
                    sw::Rect rect = this->Rect;
                    rect.left     = (Screen::Width - rect.width) / 2;
                    rect.top      = (Screen::Height - rect.height) / 2;
                    this->Rect    = rect;
                }
            }
            return this->UIElement::WndProc(refMsg);
        }

        default: {
            return this->UIElement::WndProc(refMsg);
        }
    }
}

bool sw::Window::OnClose()
{
    // 引发路由事件，cancel表示是否取消本次关闭
    bool cancel = false;
    RaiseRoutedEvent(RoutedEventType::WindowClosing, &cancel);
    if (!cancel) {
        DestroyWindow(this->Handle);
    }
    return true;
}

bool sw::Window::OnCreate()
{
    ++_windowCount;
    return true;
}

bool sw::Window::OnDestroy()
{
    // 路由事件
    RaiseRoutedEvent(RoutedEventType::WindowClosed);
    // 所有窗口都关闭时若PostQuitWhenAllClosed为true则退出程序
    if (!--_windowCount && PostQuitWhenAllClosed) {
        App::Quit();
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
    this->WndBase::Show(SW_SHOW);
}
