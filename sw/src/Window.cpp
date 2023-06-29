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
              this->Redraw();
          }),

      MaxWidth(
          // get
          [&]() -> const double & {
              return this->_maxWidth;
          },
          // set
          [&](const double &value) {
              this->_maxWidth = value;
              this->Width     = this->Width;
          }),

      MaxHeight(
          // get
          [&]() -> const double & {
              return this->_maxHeight;
          },
          // set
          [&](const double &value) {
              this->_maxHeight = value;
              this->Height     = this->Height;
          }),

      MinWidth(
          // get
          [&]() -> const double & {
              return this->_minWidth;
          },
          // set
          [&](const double &value) {
              this->_minWidth = value;
              this->Width     = this->Width;
          }),

      MinHeight(
          // get
          [&]() -> const double & {
              return this->_minHeight;
          },
          // set
          [&](const double &value) {
              this->_minHeight = value;
              this->Height     = this->Height;
          }),

      Layout(
          // get
          [&]() -> LayoutHost *const & {
              return this->_layout;
          },
          // set
          [&](LayoutHost *const &value) {
              if (value != nullptr)
                  value->Associate(this);
              this->_layout = value;
          })
{
    this->InitWindow(
        L"Window",           // Window text
        WS_OVERLAPPEDWINDOW, // Window style
        NULL,                // Parent window
        NULL);               // Menu

    this->_defaultLayout.Associate(this);
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
            if (this->_maxWidth > 0) {
                LONG maxWidth           = std::lround(this->_maxWidth / scaleX);
                pInfo->ptMaxTrackSize.x = min(pInfo->ptMaxTrackSize.x, maxWidth);
            }
            if (this->_maxHeight > 0) {
                LONG maxHeight          = std::lround(this->_maxHeight / scaleY);
                pInfo->ptMaxTrackSize.y = min(pInfo->ptMaxTrackSize.y, maxHeight);
            }
            if (this->_minWidth > 0) {
                LONG minWidth           = std::lround(this->_minWidth / scaleX);
                pInfo->ptMinTrackSize.x = max(pInfo->ptMinTrackSize.x, minWidth);
            }
            if (this->_minHeight > 0) {
                LONG minHeight          = std::lround(this->_minHeight / scaleY);
                pInfo->ptMinTrackSize.y = max(pInfo->ptMinTrackSize.y, minHeight);
            }
            return 0;
        }

        case WM_LayoutUpdated: {
            sw::Rect rect    = this->Rect;
            Thickness margin = this->Margin;
            // 更新布局
            sw::Rect pos(
                rect.left - margin.left,
                rect.top - margin.top,
                rect.width + margin.left + margin.right,
                rect.height + margin.top + margin.bottom);
            this->Measure(Size(pos.width, pos.height));
            this->Arrange(pos);
            return 0;
        }

        default: {
            return this->UIElement::WndProc(refMsg);
        }
    }
}

bool sw::Window::OnClose()
{
    // 触发路由事件，cancel表示是否取消本次关闭
    bool cancel = false;
    RaiseRoutedEvent(RoutedEventType::WindowClosing, &cancel);
    if (!cancel) {
        this->UIElement::OnClose();
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
    // 触发路由事件
    RaiseRoutedEvent(RoutedEventType::WindowClosed);
    // 所有窗口都关闭时若PostQuitWhenAllClosed为true则退出程序
    if (!--_windowCount && PostQuitWhenAllClosed) {
        App::Quit();
    }
    return true;
}

bool sw::Window::OnPaint()
{
    PAINTSTRUCT ps;
    HWND hwnd     = this->Handle;
    HDC hdc       = BeginPaint(hwnd, &ps);
    HBRUSH hBrush = CreateSolidBrush(this->_background);
    FillRect(hdc, &ps.rcPaint, hBrush);
    DeleteObject(hBrush);
    EndPaint(hwnd, &ps);
    return true;
}

void sw::Window::Measure(const Size &availableSize)
{
    Size size           = availableSize;
    sw::Rect windowRect = this->Rect;
    sw::Rect clientRect = this->ClientRect;
    Thickness margin    = this->Margin;

    // 考虑窗口边框
    size.width -= (windowRect.width - clientRect.width) + margin.left + margin.top;
    size.height -= (windowRect.height - clientRect.height) + margin.top + margin.bottom;

    this->UIElement::Measure(availableSize);
    this->GetLayoutHost().Measure(size);
}

void sw::Window::Arrange(const sw::Rect &finalPosition)
{
    this->UIElement::Arrange(finalPosition);
    this->GetLayoutHost().Arrange(this->ClientRect);
}

sw::LayoutHost &sw::Window::GetLayoutHost()
{
    return this->_layout == nullptr ? this->_defaultLayout : *this->_layout;
}

void sw::Window::ApplyLayout()
{
    // TODO
}

void sw::Window::Show()
{
    this->WndBase::Show(SW_SHOW);
}
