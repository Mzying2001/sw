#include "Window.h"
#include <cmath>

/**
 * @brief 记录当前创建的窗口数
 */
static unsigned int _windowCount = 0;

/**
 * @brief 是否在关闭所有窗口后退出程序
 */
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
          }),

      ToolWindow(
          // get
          [&]() -> const bool & {
              static bool result;
              result = this->GetExtendedStyle(WS_EX_TOOLWINDOW);
              return result;
          },
          // set
          [&](const bool &value) {
              this->SetExtendedStyle(WS_EX_TOOLWINDOW, value);
          }),

      Background(
          // get
          [&]() -> const Color & {
              return this->_background;
          },
          // set
          [&](const Color &value) {
              this->_background = value;
              InvalidateRect(this->Handle, NULL, TRUE);
          })
{
    InitWindow(
        L"Window",           // Window text
        WS_OVERLAPPEDWINDOW, // Window style
        NULL,                // Parent window
        NULL);               // Menu
}

LRESULT sw::Window::WndProc(const ProcMsg &refMsg)
{
    switch (refMsg.uMsg) {
        case WM_SHOWWINDOW: {
            // 窗口首次启动时按照StartupLocation修改位置
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

        case WM_GETMINMAXINFO: {
            double scaleX     = Dip::ScaleX;
            double scaleY     = Dip::ScaleY;
            PMINMAXINFO pInfo = reinterpret_cast<PMINMAXINFO>(refMsg.lParam);
            // 按照设置限制窗口大小
            if (this->MaxWidth > 0) {
                LONG maxWidth           = std::lround(this->MaxWidth / scaleX);
                pInfo->ptMaxTrackSize.x = min(pInfo->ptMaxTrackSize.x, maxWidth);
            }
            if (this->MaxHeight > 0) {
                LONG maxHeight          = std::lround(this->MaxHeight / scaleY);
                pInfo->ptMaxTrackSize.y = min(pInfo->ptMaxTrackSize.y, maxHeight);
            }
            if (this->MinWidth > 0) {
                LONG minWidth           = std::lround(this->MinWidth / scaleX);
                pInfo->ptMinTrackSize.x = max(pInfo->ptMinTrackSize.x, minWidth);
            }
            if (this->MinHeight > 0) {
                LONG minHeight          = std::lround(this->MinHeight / scaleY);
                pInfo->ptMinTrackSize.y = max(pInfo->ptMinTrackSize.y, minHeight);
            }
            return 0;
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
    HDC hdc       = BeginPaint(this->Handle, &ps);
    HBRUSH hBrush = CreateSolidBrush(this->_background);
    FillRect(hdc, &ps.rcPaint, hBrush);
    DeleteObject(hBrush);
    EndPaint(this->Handle, &ps);
    return true;
}

void sw::Window::Show()
{
    this->WndBase::Show(SW_SHOW);
}
