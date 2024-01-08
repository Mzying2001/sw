#include "Window.h"
#include "Utils.h"
#include <cmath>

#if !defined(WM_DPICHANGED)
#define WM_DPICHANGED 0x02E0
#endif

/**
 * @brief 记录当前创建的窗口数
 */
static int _windowCount = 0;

/**
 * @brief DPI更新时调用该函数递归地更新所有子项的字体
 */
static void _UpdateFontForAllChild(sw::UIElement &element);

/**
 * @brief  获取窗口默认图标（即当前exe图标）
 * @return 图标句柄
 */
static HICON _GetWindowDefaultIcon();

/**
 * @brief 程序的当前活动窗体
 */
const sw::ReadOnlyProperty<sw::Window *> sw::Window::ActiveWindow(
    []() -> sw::Window *const & {
        static sw::Window *pWindow;
        HWND hwnd = GetActiveWindow();
        pWindow   = dynamic_cast<sw::Window *>(sw::WndBase::GetWndBase(hwnd));
        return pWindow;
    } //
);

/**
 * @brief 当前已创建的窗口数
 */
const sw::ReadOnlyProperty<int> sw::Window::WindowCount(
    []() -> const int & {
        return _windowCount;
    } //
);

sw::Window::Window()
    : StartupLocation(
          // get
          [&]() -> const WindowStartupLocation & {
              return this->_startupLocation;
          },
          // set
          [&](const WindowStartupLocation &value) {
              this->_startupLocation = value;
          }),

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

      Menu(
          // get
          [&]() -> sw::Menu *const & {
              return this->_menu;
          },
          // set
          [&](sw::Menu *const &value) {
              this->_menu = value;
              SetMenu(this->Handle, value != nullptr ? value->GetHandle() : NULL);
          })
{
    this->InitWindow(L"Window", WS_OVERLAPPEDWINDOW, 0);
    this->SetIcon(_GetWindowDefaultIcon());
}

LRESULT sw::Window::WndProc(const ProcMsg &refMsg)
{
    switch (refMsg.uMsg) {
        case WM_CREATE: {
            ++_windowCount;
            return this->WndBase::WndProc(refMsg);
        }

        case WM_DESTROY: {
            // 若当前窗口为模态窗口则在窗口关闭时退出消息循环
            if (this->IsModal()) {
                App::QuitMsgLoop();
            }
            // 所有窗口都关闭时若App::QuitMode为Auto则退出主消息循环
            if (--_windowCount <= 0 && App::QuitMode.Get() == AppQuitMode::Auto) {
                App::QuitMsgLoop();
            }
            return this->WndBase::WndProc(refMsg);
        }

        case WM_SHOWWINDOW: {
            if (this->_isFirstShow) {
                this->_isFirstShow = false;
                this->OnFirstShow();
            }
            return this->WndBase::WndProc(refMsg);
        }

        case WM_GETMINMAXINFO: {
            static double &scaleX = const_cast<double &>(Dip::ScaleX.Get());
            static double &scaleY = const_cast<double &>(Dip::ScaleY.Get());
            PMINMAXINFO pInfo     = reinterpret_cast<PMINMAXINFO>(refMsg.lParam);
            // 按照设置限制窗口大小
            if (this->_maxWidth > 0) {
                LONG maxWidth           = std::lround(this->_maxWidth / scaleX);
                pInfo->ptMaxTrackSize.x = Utils::Min(pInfo->ptMaxTrackSize.x, maxWidth);
            }
            if (this->_maxHeight > 0) {
                LONG maxHeight          = std::lround(this->_maxHeight / scaleY);
                pInfo->ptMaxTrackSize.y = Utils::Min(pInfo->ptMaxTrackSize.y, maxHeight);
            }
            if (this->_minWidth > 0) {
                LONG minWidth           = std::lround(this->_minWidth / scaleX);
                pInfo->ptMinTrackSize.x = Utils::Max(pInfo->ptMinTrackSize.x, minWidth);
            }
            if (this->_minHeight > 0) {
                LONG minHeight          = std::lround(this->_minHeight / scaleY);
                pInfo->ptMinTrackSize.y = Utils::Max(pInfo->ptMinTrackSize.y, minHeight);
            }
            return 0;
        }

        case WM_DPICHANGED: {
            Dip::Update(LOWORD(refMsg.wParam), HIWORD(refMsg.wParam));
            this->UpdateLayout();
            _UpdateFontForAllChild(*this);
            return 0;
        }

        case WM_ACTIVATE: {
            if (refMsg.wParam == WA_INACTIVE)
                this->OnInactived();
            else
                this->OnActived();
            return 0;
        }

        case WM_UpdateLayout: {
            this->UpdateLayout();
            return 0;
        }

        default: {
            return this->WndBase::WndProc(refMsg);
        }
    }
}

sw::LayoutHost *sw::Window::GetDefaultLayout()
{
    return this->_layout.get();
}

bool sw::Window::OnClose()
{
    WindowClosingEventArgs args;
    RaiseRoutedEvent(args);

    if (!args.cancel) {
        this->UIElement::OnClose();
    }

    return true;
}

bool sw::Window::OnDestroy()
{
    RaiseRoutedEvent(Window_Closed);
    return true;
}

bool sw::Window::OnEraseBackground(int &result)
{
    result = 1; // 阻止擦除背景
    return true;
}

bool sw::Window::OnPaint()
{
    PAINTSTRUCT ps;
    HWND hwnd = this->Handle;
    HDC hdc   = BeginPaint(hwnd, &ps);

    RECT rtClient;
    GetClientRect(hwnd, &rtClient);

    // 创建内存 DC 和位图
    HDC hdcMem         = CreateCompatibleDC(hdc);
    HBITMAP hBitmap    = CreateCompatibleBitmap(hdc, rtClient.right - rtClient.left, rtClient.bottom - rtClient.top);
    HBITMAP hBitmapOld = (HBITMAP)SelectObject(hdcMem, hBitmap);

    // 在内存 DC 上进行绘制
    HBRUSH hBrush = CreateSolidBrush(this->GetRealBackColor());
    FillRect(hdcMem, &rtClient, hBrush);

    // 将内存 DC 的内容绘制到窗口客户区
    BitBlt(hdc, 0, 0, rtClient.right - rtClient.left, rtClient.bottom - rtClient.top, hdcMem, 0, 0, SRCCOPY);

    // 清理资源
    SelectObject(hdcMem, hBitmapOld);
    DeleteObject(hBitmap);
    DeleteObject(hBrush);
    DeleteDC(hdcMem);

    EndPaint(hwnd, &ps);
    return true;
}

void sw::Window::OnMenuCommand(int id)
{
    if (ContextMenu::IsContextMenuID(id)) {
        this->UIElement::OnMenuCommand(id);
        return;
    }
    if (this->_menu) {
        MenuItem *item = this->_menu->GetMenuItem(id);
        if (item) item->CallCommand();
    }
}

void sw::Window::OnFirstShow()
{
    // 若未设置焦点元素则默认第一个元素为焦点元素
    if (this->ChildCount && GetAncestor(GetFocus(), GA_ROOT) != this->Handle) {
        this->operator[](0).Focused = true;
    }

    // 按照StartupLocation修改位置
    if (this->_startupLocation == WindowStartupLocation::CenterScreen) {
        sw::Rect rect = this->Rect;
        rect.left     = (Screen::Width - rect.width) / 2;
        rect.top      = (Screen::Height - rect.height) / 2;
        this->Rect    = rect;
    } else if (this->_startupLocation == WindowStartupLocation::CenterOwner) {
        if (this->IsModal()) {
            sw::Rect windowRect = this->Rect;
            sw::Rect ownerRect  = this->_modalOwner->Rect;
            windowRect.left     = ownerRect.left + (ownerRect.width - windowRect.width) / 2;
            windowRect.top      = ownerRect.top + (ownerRect.height - windowRect.height) / 2;
            this->Rect          = windowRect;
        }
    }
}

void sw::Window::OnActived()
{
    SetFocus(this->_hPrevFocused);
    this->RaiseRoutedEvent(Window_Actived);
}

void sw::Window::OnInactived()
{
    this->RaiseRoutedEvent(Window_Inactived);
    this->_hPrevFocused = GetFocus();
}

void sw::Window::Show()
{
    this->WndBase::Show(SW_SHOW);
}

void sw::Window::ShowDialog(Window &owner)
{
    if (this->IsModal() || this == &owner || this->IsDestroyed) {
        return;
    }

    this->_modalOwner = &owner;
    SetWindowLongPtrW(this->Handle, GWLP_HWNDPARENT, reinterpret_cast<LONG_PTR>(owner.Handle.Get()));

    bool oldIsEnabled = owner.Enabled;
    owner.Enabled     = false;

    this->Show();
    App::MsgLoop();
    SetForegroundWindow(owner.Handle);

    if (oldIsEnabled) {
        owner.Enabled = true;
    }
}

void sw::Window::SetIcon(HICON hIcon)
{
    this->SendMessageW(WM_SETICON, ICON_BIG, (LPARAM)hIcon);
    this->SendMessageW(WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
}

void sw::Window::DrawMenuBar()
{
    ::DrawMenuBar(this->Handle);
}

bool sw::Window::IsModal()
{
    return this->_modalOwner != nullptr;
}

void _UpdateFontForAllChild(sw::UIElement &element)
{
    element.UpdateFont();

    int count = element.ChildCount;
    for (int i = 0; i < count; ++i) {
        _UpdateFontForAllChild(element[i]);
    }
}

HICON _GetWindowDefaultIcon()
{
    static HICON hIcon = ExtractIconW(sw::App::Instance, sw::App::ExePath->c_str(), 0);
    return hIcon;
}
