#include "Window.h"
#include "Utils.h"
#include <cmath>

#if !defined(WM_DPICHANGED)
#define WM_DPICHANGED 0x02E0
#endif

namespace
{
    /**
     * @brief 记录当前创建的窗口数
     */
    thread_local int _windowCount = 0;

    /**
     * @brief 窗口句柄保存Window指针的属性名称
     */
    constexpr wchar_t _WindowPtrProp[] = L"SWPROP_WindowPtr";
}

/**
 * @brief 当前线程的活动窗口
 */
const sw::ReadOnlyProperty<sw::Window *> sw::Window::ActiveWindow(
    []() -> sw::Window * {
        HWND hwnd = GetActiveWindow();
        // return _GetWindowPtr(hwnd); // vs2015无法识别此处的作用域？
        return reinterpret_cast<sw::Window *>(GetPropW(hwnd, _WindowPtrProp));
    } //
);

/**
 * @brief 当前线程已创建的窗口数
 */
const sw::ReadOnlyProperty<int> sw::Window::WindowCount(
    []() -> int {
        return _windowCount;
    } //
);

sw::Window::Window()
    : StartupLocation(
          // get
          [this]() -> WindowStartupLocation {
              return _startupLocation;
          },
          // set
          [this](const WindowStartupLocation &value) {
              _startupLocation = value;
          }),

      State(
          // get
          [this]() -> WindowState {
              HWND hwnd = Handle;
              if (IsIconic(hwnd)) {
                  return WindowState::Minimized;
              } else if (IsZoomed(hwnd)) {
                  return WindowState::Maximized;
              } else {
                  return WindowState::Normal;
              }
          },
          // set
          [this](const WindowState &value) {
              HWND hwnd = Handle;
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
          [this]() -> bool {
              return GetStyle(WS_SIZEBOX);
          },
          // set
          [this](const bool &value) {
              SetStyle(WS_SIZEBOX, value);
          }),

      MaximizeBox(
          // get
          [this]() -> bool {
              return GetStyle(WS_MAXIMIZEBOX);
          },
          // set
          [this](const bool &value) {
              SetStyle(WS_MAXIMIZEBOX, value);
          }),

      MinimizeBox(
          // get
          [this]() -> bool {
              return GetStyle(WS_MINIMIZEBOX);
          },
          // set
          [this](const bool &value) {
              SetStyle(WS_MINIMIZEBOX, value);
          }),

      Topmost(
          // get
          [this]() -> bool {
              return GetExtendedStyle(WS_EX_TOPMOST);
          },
          // set
          [this](const bool &value) {
              /*SetExtendedStyle(WS_EX_TOPMOST, value);*/
              HWND hWndInsertAfter = value ? HWND_TOPMOST : HWND_NOTOPMOST;
              SetWindowPos(Handle, hWndInsertAfter, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
          }),

      ToolWindow(
          // get
          [this]() -> bool {
              return GetExtendedStyle(WS_EX_TOOLWINDOW);
          },
          // set
          [this](const bool &value) {
              SetExtendedStyle(WS_EX_TOOLWINDOW, value);
          }),

      Menu(
          // get
          [this]() -> sw::Menu * {
              return _menu;
          },
          // set
          [this](sw::Menu *value) {
              _menu = value;
              SetMenu(Handle, value != nullptr ? value->GetHandle() : NULL);
          }),

      IsModal(
          // get
          [this]() -> bool {
              return _isModal;
          }),

      Owner(
          // get
          [this]() -> Window * {
              HWND hOwner = reinterpret_cast<HWND>(GetWindowLongPtrW(Handle, GWLP_HWNDPARENT));
              return _GetWindowPtr(hOwner);
          },
          // set
          [this](Window *value) {
              SetWindowLongPtrW(Handle, GWLP_HWNDPARENT, reinterpret_cast<LONG_PTR>(value ? value->Handle.Get() : NULL));
          }),

      IsLayered(
          // get
          [this]() -> bool {
              return GetExtendedStyle(WS_EX_LAYERED);
          },
          // set
          [this](const bool &value) {
              SetExtendedStyle(WS_EX_LAYERED, value);
          }),

      Opacity(
          // get
          [this]() -> double {
              BYTE result;
              return GetLayeredWindowAttributes(Handle, NULL, &result, NULL) ? (result / 255.0) : 1.0;
          },
          // set
          [this](const double &value) {
              double opacity = Utils::Min(1.0, Utils::Max(0.0, value));
              SetLayeredWindowAttributes(Handle, 0, (BYTE)std::lround(255 * opacity), LWA_ALPHA);
          }),

      Borderless(
          // get
          [this]() -> bool {
              return _isBorderless;
          },
          // set
          [this](const bool &value) {
              if (_isBorderless != value) {
                  _isBorderless = value;
                  SetStyle(WS_CAPTION | WS_THICKFRAME, !value);
              }
          }),

      DialogResult(
          // get
          [this]() -> int {
              return _dialogResult;
          },
          // set
          [this](const int &value) {
              _dialogResult = value;
              Close();
          }),

      RestoreRect(
          // get
          [this]() -> sw::Rect {
              WINDOWPLACEMENT wp{};
              wp.length = sizeof(WINDOWPLACEMENT);
              GetWindowPlacement(Handle, &wp);
              return wp.rcNormalPosition;
          }),

      IsLayoutDisabled(
          // get
          [this]() -> bool {
              return _IsLayoutDisabled();
          })
{
    InitWindow(L"Window", WS_OVERLAPPEDWINDOW, 0);
    _SetWindowPtr(Handle, *this);
    SetIcon(_GetWindowDefaultIcon());
}

sw::Window::~Window()
{
    if (!IsDestroyed) {
        DestroyWindow(Handle);
    }
}

LRESULT sw::Window::WndProc(const ProcMsg &refMsg)
{
    switch (refMsg.uMsg) {
        case WM_CREATE: {
            ++_windowCount;
            return TBase::WndProc(refMsg);
        }

        case WM_DESTROY: {
            _isDestroying = true;
            auto result   = TBase::WndProc(refMsg);
            bool quitted  = false;
            // 若当前窗口为模态窗口则在窗口关闭时退出消息循环
            if (_isModal) {
                App::QuitMsgLoop(_dialogResult);
                quitted = true;
            }
            // 所有窗口都关闭时若App::QuitMode为Auto则退出主消息循环
            if (--_windowCount <= 0 && App::QuitMode == AppQuitMode::Auto) {
                if (!quitted) App::QuitMsgLoop();
            }
            _isDestroying = false;
            return result;
        }

        case WM_SHOWWINDOW: {
            if (_isFirstShow) {
                _isFirstShow = false;
                OnFirstShow();
            }
            return TBase::WndProc(refMsg);
        }

        case WM_GETMINMAXINFO: {
            auto pInfo = reinterpret_cast<PMINMAXINFO>(refMsg.lParam);
            Size minSize{MinWidth, MinHeight};
            Size maxSize{MaxWidth, MaxHeight};
            if (minSize.width > 0) {
                pInfo->ptMinTrackSize.x = Utils::Max<LONG>(pInfo->ptMinTrackSize.x, Dip::DipToPxX(minSize.width));
            }
            if (minSize.height > 0) {
                pInfo->ptMinTrackSize.y = Utils::Max<LONG>(pInfo->ptMinTrackSize.y, Dip::DipToPxY(minSize.height));
            }
            if (maxSize.width > 0) {
                pInfo->ptMaxTrackSize.x = Utils::Min<LONG>(pInfo->ptMaxTrackSize.x, Dip::DipToPxX(maxSize.width));
            }
            if (maxSize.height > 0) {
                pInfo->ptMaxTrackSize.y = Utils::Min<LONG>(pInfo->ptMaxTrackSize.y, Dip::DipToPxY(maxSize.height));
            }
            return 0;
        }

        case WM_DPICHANGED: {
            OnDpiChanged(LOWORD(refMsg.wParam), HIWORD(refMsg.wParam));
            return 0;
        }

        case WM_ACTIVATE: {
            (refMsg.wParam == WA_INACTIVE) ? OnInactived() : OnActived();
            return 0;
        }

        case WM_UpdateLayout: {
            if (!_isDestroying && !_IsLayoutDisabled()) {
                UpdateLayout();
            }
            return 0;
        }

        case WM_PreSetParent: {
            HWND hParent = reinterpret_cast<HWND>(refMsg.wParam);
            SetStyle(WS_CHILD, hParent != NULL);
            return 0;
        }

        default: {
            return TBase::WndProc(refMsg);
        }
    }
}

sw::LayoutHost *sw::Window::GetDefaultLayout()
{
    return _defaultLayout.get();
}

bool sw::Window::OnCreate()
{
    return true;
}

bool sw::Window::OnClose()
{
    WindowClosingEventArgs args;
    RaiseRoutedEvent(args);

    if (!args.cancel) {
        TBase::OnClose();
        DestroyWindow(Handle);
    }
    return true;
}

bool sw::Window::OnDestroy()
{
    RaiseRoutedEvent(Window_Closed);
    return true;
}

bool sw::Window::OnEraseBackground(HDC hdc, LRESULT &result)
{
    result = 1; // 阻止擦除背景
    return true;
}

bool sw::Window::OnPaint()
{
    PAINTSTRUCT ps;
    HWND hwnd = Handle;
    HDC hdc   = BeginPaint(hwnd, &ps);

    RECT rtClient;
    GetClientRect(hwnd, &rtClient);

    SIZE sizeClient{
        rtClient.right - rtClient.left,
        rtClient.bottom - rtClient.top};

    // 创建内存 DC 和位图
    HDC hdcMem      = CreateCompatibleDC(hdc);
    HBITMAP hBmpWnd = CreateCompatibleBitmap(hdc, sizeClient.cx, sizeClient.cy);
    HBITMAP hBmpOld = (HBITMAP)SelectObject(hdcMem, hBmpWnd);

    // 在内存 DC 上进行绘制
    HBRUSH hBrush = CreateSolidBrush(GetRealBackColor());
    FillRect(hdcMem, &rtClient, hBrush);

    // 将内存 DC 的内容绘制到窗口客户区
    BitBlt(hdc, 0, 0, sizeClient.cx, sizeClient.cy, hdcMem, 0, 0, SRCCOPY);

    // 清理资源
    SelectObject(hdcMem, hBmpOld);
    DeleteObject(hBmpWnd);
    DeleteObject(hBrush);
    DeleteDC(hdcMem);

    EndPaint(hwnd, &ps);
    return true;
}

void sw::Window::OnMenuCommand(int id)
{
    if (ContextMenu::IsContextMenuID(id)) {
        TBase::OnMenuCommand(id);
        return;
    }
    if (_menu) {
        MenuItem *item = _menu->GetMenuItem(id);
        if (item) item->CallCommand();
    }
}

void sw::Window::OnMinMaxSizeChanged()
{
    if (!IsRootElement()) {
        TBase::OnMinMaxSizeChanged();
    }

    HWND hwnd = Handle;

    RECT rect;
    GetWindowRect(hwnd, &rect);

    SetWindowPos(
        hwnd, NULL, 0, 0, rect.right - rect.left, rect.bottom - rect.top,
        SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
}

void sw::Window::OnFirstShow()
{
    // 若未设置焦点元素则默认第一个元素为焦点元素
    if (ChildCount && GetAncestor(GetFocus(), GA_ROOT) != Handle) {
        GetChildAt(0).Focused = true;
    }

    // 按照StartupLocation修改位置
    if (_startupLocation == WindowStartupLocation::CenterScreen) {
        auto rect = Rect.Get();
        rect.left = (Screen::Width - rect.width) / 2;
        rect.top  = (Screen::Height - rect.height) / 2;
        Rect      = rect;
    } else if (_startupLocation == WindowStartupLocation::CenterOwner) {
        Window *owner = Owner;
        if (owner) {
            auto windowRect = Rect.Get();
            auto ownerRect  = owner->Rect.Get();
            windowRect.left = ownerRect.left + (ownerRect.width - windowRect.width) / 2;
            windowRect.top  = ownerRect.top + (ownerRect.height - windowRect.height) / 2;
            Rect            = windowRect;
        }
    }
}

void sw::Window::OnActived()
{
    SetFocus(_hPrevFocused);
    RaiseRoutedEvent(Window_Actived);
}

void sw::Window::OnInactived()
{
    RaiseRoutedEvent(Window_Inactived);
    _hPrevFocused = GetFocus();
}

void sw::Window::OnDpiChanged(int dpiX, int dpiY)
{
    Dip::Update(dpiX, dpiY);
    DisableLayout();

    {
        // Windows在DIP改变时会自动调整窗口大小，此时会先触发WM_WINDOWPOSCHANGED，再触发WM_DPICHANGED
        // 因此在先触发的WM_WINDOWPOSCHANGED消息中，（Dip类中）DPI信息未更新，从而导致窗口的Rect数据错误
        // 此处在更新DPI信息后手动发送一个WM_WINDOWPOSCHANGED以修正窗口的Rect数据

        HWND hwnd = Handle;

        RECT rect;
        GetWindowRect(hwnd, &rect);

        WINDOWPOS pos{};
        pos.hwnd  = hwnd;
        pos.x     = rect.left;
        pos.y     = rect.top;
        pos.cx    = rect.right - rect.left;
        pos.cy    = rect.bottom - rect.top;
        pos.flags = SWP_NOACTIVATE | SWP_NOZORDER;

        SendMessageW(WM_WINDOWPOSCHANGED, 0, reinterpret_cast<LPARAM>(&pos));
    }

    UpdateFont();

    QueryAllChildren([](UIElement *item) {
        return item->UpdateFont(), true;
    });

    EnableLayout();
}

sw::Window *sw::Window::ToWindow()
{
    return this;
}

void sw::Window::Close()
{
    TBase::Close();
}

void sw::Window::Show()
{
    TBase::Show(SW_SHOW);
}

int sw::Window::ShowDialog(Window *owner)
{
    if (owner != nullptr) {
        return ShowDialog(*owner);
    }

    int result = -1;

    if (!CheckAccess()) {
        return result; // 只能在创建窗口的线程调用
    }

    if (_isModal || IsDestroyed) {
        return result;
    }

    HWND hOwner = NULL;
    HWND hwnd   = Handle;

    owner  = this->Owner;
    hOwner = owner ? owner->Handle : reinterpret_cast<HWND>(GetWindowLongPtrW(hwnd, GWLP_HWNDPARENT));

    if (hOwner == NULL) {
        if ((hOwner = GetActiveWindow()) != NULL) {
            SetWindowLongPtrW(hwnd, GWLP_HWNDPARENT, reinterpret_cast<LONG_PTR>(hOwner));
        }
    }

    _isModal     = true;
    _hModalOwner = hOwner;

    if (hOwner == NULL) {
        Show();
        result = App::MsgLoop();
    } else {
        bool oldIsEnabled = IsWindowEnabled(hOwner);
        EnableWindow(hOwner, false);
        Show();
        result = App::MsgLoop();
        SetForegroundWindow(hOwner);
        EnableWindow(hOwner, oldIsEnabled);
    }
    return result;
}

int sw::Window::ShowDialog(Window &owner)
{
    int result = -1;

    if (!CheckAccess()) {
        return result; // 只能在创建窗口的线程调用
    }

    if (this == &owner || _isModal || IsDestroyed) {
        return result;
    }

    Owner        = &owner;
    _isModal     = true;
    _hModalOwner = owner.Handle;

    bool oldIsEnabled = owner.Enabled;
    owner.Enabled     = false;

    Show();
    result = App::MsgLoop();
    SetForegroundWindow(owner.Handle);

    if (oldIsEnabled) {
        owner.Enabled = true;
    }
    return result;
}

bool sw::Window::DisableLayout()
{
    if (!CheckAccess()) {
        return false; // 只能在创建窗口的线程调用
    }
    ++_disableLayoutCount;
    return true;
}

bool sw::Window::EnableLayout(bool reset)
{
    if (!CheckAccess()) {
        return false; // 只能在创建窗口的线程调用
    }

    if (reset) {
        _disableLayoutCount = 0;
    } else {
        _disableLayoutCount = Utils::Max(0, _disableLayoutCount - 1);
    }

    if (!_IsLayoutDisabled()) {
        UpdateLayout();
    }
    return true;
}

void sw::Window::SetIcon(HICON hIcon)
{
    SendMessageW(WM_SETICON, ICON_BIG, (LPARAM)hIcon);
    SendMessageW(WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
}

void sw::Window::DrawMenuBar()
{
    ::DrawMenuBar(Handle);
}

void sw::Window::SizeToContent()
{
    if (!IsRootElement()) {
        return; // 只对顶级窗口有效
    }

    // 该函数需要AutoSize为true，这里先备份其值以做后续恢复
    bool oldAutoSize = AutoSize;
    AutoSize         = true;

    // measure
    sw::Size measureSize(INFINITY, INFINITY);
    Measure(measureSize);

    // arrange
    sw::Size desireSize  = GetDesireSize();
    sw::Rect windowRect  = Rect;
    sw::Thickness margin = Margin;
    Arrange(sw::Rect{windowRect.left - margin.left, windowRect.top - margin.top, desireSize.width, desireSize.height});

    // 恢复AutoSize属性的值
    AutoSize = oldAutoSize;
}

bool sw::Window::_IsLayoutDisabled() const noexcept
{
    return _disableLayoutCount > 0;
}

sw::Window *sw::Window::_GetWindowPtr(HWND hwnd)
{
    return reinterpret_cast<sw::Window *>(GetPropW(hwnd, _WindowPtrProp));
}

void sw::Window::_SetWindowPtr(HWND hwnd, Window &wnd)
{
    SetPropW(hwnd, _WindowPtrProp, reinterpret_cast<HANDLE>(&wnd));
}

HICON sw::Window::_GetWindowDefaultIcon()
{
    static HICON hIcon = ExtractIconW(App::Instance, App::ExePath->c_str(), 0);
    return hIcon;
}
