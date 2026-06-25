#include "Window.h"
#include "App.h"
#include "Menu.h"
#include "Screen.h"
#include "Utils.h"
#include "WndMsg.h"

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
    Property<sw::Window *>::Init()
        .Getter([]() -> sw::Window * {
            HWND hwnd = GetActiveWindow();
            auto *wnd = reinterpret_cast<sw::Window *>(GetPropW(hwnd, _WindowPtrProp));
            return wnd != nullptr && wnd->CheckAccess() ? wnd : nullptr;
        }) //
);

/**
 * @brief 当前线程已创建的窗口数
 */
const sw::ReadOnlyProperty<int> sw::Window::WindowCount(
    Property<int>::Init()
        .Getter([]() -> int {
            return _windowCount;
        }) //
);

sw::Window::Window()
    : StartupLocation(
          Property<WindowStartupLocation>::Init(this)
              .Getter([](Window *self) -> WindowStartupLocation {
                  return self->_startupLocation;
              })
              .Setter([](Window *self, WindowStartupLocation value) {
                  if (self->_startupLocation != value) {
                      self->_startupLocation = value;
                      self->RaisePropertyChanged(&Window::StartupLocation);
                  }
              })),

      State(
          Property<WindowState>::Init(this)
              .Getter([](Window *self) -> WindowState {
                  HWND hwnd = self->Handle;
                  if (IsIconic(hwnd)) {
                      return WindowState::Minimized;
                  } else if (IsZoomed(hwnd)) {
                      return WindowState::Maximized;
                  } else {
                      return WindowState::Normal;
                  }
              })
              .Setter([](Window *self, WindowState value) {
                  if (self->State == value) {
                      return;
                  }
                  switch (value) {
                      case WindowState::Normal:
                          ShowWindow(self->Handle, SW_RESTORE);
                          break;
                      case WindowState::Minimized:
                          ShowWindow(self->Handle, SW_MINIMIZE);
                          break;
                      case WindowState::Maximized:
                          ShowWindow(self->Handle, SW_MAXIMIZE);
                          break;
                  }
                  self->RaisePropertyChanged(&Window::State);
              })),

      SizeBox(
          Property<bool>::Init(this)
              .Getter([](Window *self) -> bool {
                  return self->GetStyle(WS_SIZEBOX);
              })
              .Setter([](Window *self, bool value) {
                  if (self->SizeBox != value) {
                      self->SetStyle(WS_SIZEBOX, value);
                      self->RaisePropertyChanged(&Window::SizeBox);
                  }
              })),

      MaximizeBox(
          Property<bool>::Init(this)
              .Getter([](Window *self) -> bool {
                  return self->GetStyle(WS_MAXIMIZEBOX);
              })
              .Setter([](Window *self, bool value) {
                  if (self->MaximizeBox != value) {
                      self->SetStyle(WS_MAXIMIZEBOX, value);
                      self->RaisePropertyChanged(&Window::MaximizeBox);
                  }
              })),

      MinimizeBox(
          Property<bool>::Init(this)
              .Getter([](Window *self) -> bool {
                  return self->GetStyle(WS_MINIMIZEBOX);
              })
              .Setter([](Window *self, bool value) {
                  if (self->MinimizeBox != value) {
                      self->SetStyle(WS_MINIMIZEBOX, value);
                      self->RaisePropertyChanged(&Window::MinimizeBox);
                  }
              })),

      Topmost(
          Property<bool>::Init(this)
              .Getter([](Window *self) -> bool {
                  return self->GetExtendedStyle(WS_EX_TOPMOST);
              })
              .Setter([](Window *self, bool value) {
                  if (self->Topmost != value) {
                      HWND hWndInsertAfter = value ? HWND_TOPMOST : HWND_NOTOPMOST;
                      SetWindowPos(self->Handle, hWndInsertAfter, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
                      self->RaisePropertyChanged(&Window::Topmost);
                  }
              })),

      ToolWindow(
          Property<bool>::Init(this)
              .Getter([](Window *self) -> bool {
                  return self->GetExtendedStyle(WS_EX_TOOLWINDOW);
              })
              .Setter([](Window *self, bool value) {
                  if (self->ToolWindow != value) {
                      self->SetExtendedStyle(WS_EX_TOOLWINDOW, value);
                      self->RaisePropertyChanged(&Window::ToolWindow);
                  }
              })),

      Menu(
          Property<sw::Menu *>::Init(this)
              .Getter([](Window *self) -> sw::Menu * {
                  return self->_menu;
              })
              .Setter([](Window *self, sw::Menu *value) {
                  if (self->_menu != value) {
                      self->_menu = value;
                      SetMenu(self->Handle, value != nullptr ? value->Handle.Get() : NULL);
                      self->RaisePropertyChanged(&Window::Menu);
                  }
              })),

      IsModal(
          Property<bool>::Init(this)
              .Getter([](Window *self) -> bool {
                  return self->_isModal;
              })),

      Owner(
          Property<Window *>::Init(this)
              .Getter([](Window *self) -> Window * {
                  HWND hOwner = reinterpret_cast<HWND>(GetWindowLongPtrW(self->Handle, GWLP_HWNDPARENT));
                  return _GetWindowPtr(hOwner);
              })
              .Setter([](Window *self, Window *value) {
                  if (self->Owner != value) {
                      HWND hOwner = value ? value->Handle.Get() : NULL;
                      SetWindowLongPtrW(self->Handle, GWLP_HWNDPARENT, reinterpret_cast<LONG_PTR>(hOwner));
                      self->RaisePropertyChanged(&Window::Owner);
                  }
              })),

      IsLayered(
          Property<bool>::Init(this)
              .Getter([](Window *self) -> bool {
                  return self->GetExtendedStyle(WS_EX_LAYERED);
              })
              .Setter([](Window *self, bool value) {
                  if (self->IsLayered != value) {
                      self->SetExtendedStyle(WS_EX_LAYERED, value);
                      self->RaisePropertyChanged(&Window::IsLayered);
                  }
              })),

      Opacity(
          Property<double>::Init(this)
              .Getter([](Window *self) -> double {
                  BYTE result;
                  return GetLayeredWindowAttributes(self->Handle, NULL, &result, NULL) ? (result / 255.0) : 1.0;
              })
              .Setter([](Window *self, double value) {
                  if (self->Opacity != value) {
                      double opacity = Utils::Min(1.0, Utils::Max(0.0, value));
                      SetLayeredWindowAttributes(self->Handle, 0, (BYTE)std::lround(255 * opacity), LWA_ALPHA);
                      self->RaisePropertyChanged(&Window::Opacity);
                  }
              })),

      Borderless(
          Property<bool>::Init(this)
              .Getter([](Window *self) -> bool {
                  return self->_isBorderless;
              })
              .Setter([](Window *self, bool value) {
                  if (self->_isBorderless != value) {
                      self->_isBorderless = value;
                      self->SetStyle(WS_CAPTION | WS_THICKFRAME, !value);
                      self->RaisePropertyChanged(&Window::Borderless);
                  }
              })),

      DialogResult(
          Property<int>::Init(this)
              .Getter([](Window *self) -> int {
                  return self->_dialogResult;
              })
              .Setter([](Window *self, int value) {
                  if (self->_dialogResult != value) {
                      self->_dialogResult = value;
                      self->RaisePropertyChanged(&Window::DialogResult);
                  }
                  self->Close();
              })),

      RestoreRect(
          Property<sw::Rect>::Init(this)
              .Getter([](Window *self) -> sw::Rect {
                  WINDOWPLACEMENT wp{};
                  wp.length = sizeof(WINDOWPLACEMENT);
                  GetWindowPlacement(self->Handle, &wp);
                  return wp.rcNormalPosition;
              })),

      IsLayoutDisabled(
          Property<bool>::Init(this)
              .Getter([](Window *self) -> bool {
                  return self->_IsLayoutDisabled();
              }))
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

LRESULT sw::Window::WndProc(ProcMsg &refMsg)
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
            if (_isFirstShow && refMsg.wParam) {
                _isFirstShow = false;
                OnFirstShow();
            }
            return TBase::WndProc(refMsg);
        }

        case WM_GETMINMAXINFO: {
            _ClampMinMaxSize(reinterpret_cast<PMINMAXINFO>(refMsg.lParam));
            return 0;
        }

        case WM_DPICHANGED: {
            int dpiX   = LOWORD(refMsg.wParam);
            int dpiY   = HIWORD(refMsg.wParam);
            auto &rect = *reinterpret_cast<RECT *>(refMsg.lParam);
            return OnDpiChanged(dpiX, dpiY, rect) ? 0 : DefaultWndProc(refMsg);
        }

        case WM_ACTIVATE: {
            if (refMsg.wParam != WA_INACTIVE) {
                OnActived();
            } else {
                OnInactived();
            }
            return 0;
        }

        case WM_UpdateLayout: {
            if (!_isDestroying && !_IsLayoutDisabled()) {
                UpdateLayout();
            }
            return 0;
        }

        case WM_PreSetParent: {
            auto style   = GetStyle();
            HWND hParent = (HWND)refMsg.wParam;
            if (hParent != NULL) {
                style |= WS_CHILD;
                style &= ~WS_POPUP;
            } else {
                style &= ~WS_CHILD;
                style |= WS_POPUP;
            }
            SetStyle(style);
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
    PAINTSTRUCT ps{};
    HWND hwnd = Handle;
    HDC hdc   = BeginPaint(hwnd, &ps);

    if (hdc != NULL &&
        ps.rcPaint.left < ps.rcPaint.right &&
        ps.rcPaint.top < ps.rcPaint.bottom) //
    {
        auto color    = static_cast<COLORREF>(GetRealBackColor());
        HBRUSH hBrush = CreateSolidBrush(color);

        if (hBrush != NULL) {
            FillRect(hdc, &ps.rcPaint, hBrush);
            DeleteObject(hBrush);
        }
    }

    EndPaint(hwnd, &ps);
    return true;
}

void sw::Window::OnMenuCommand(int id)
{
    bool handled = false;

    if (_menu != nullptr) {
        handled = _menu->RaiseClickedEvent(id);
    }

    if (!handled) {
        TBase::OnMenuCommand(id);
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
    if (ChildCount > 0 && (GetAncestor(GetFocus(), GA_ROOT) != Handle)) {
        GetChildAt(0).Focused = true;
    }

    if (!IsRootElement()) {
        return; // StartupLocation仅对顶级窗口生效
    }

    // 按照StartupLocation修改位置
    switch (_startupLocation) {
        case WindowStartupLocation::Manual: {
            break;
        }
        case WindowStartupLocation::CenterOwner: {
            auto owner = Owner.Get();
            if (owner) {
                _CenterWindow(owner->Rect);
                break;
            } else {
                // fallthrough
            }
        }
        case WindowStartupLocation::CenterScreen: {
            _CenterWindow(sw::Rect{0, 0, Screen::Width, Screen::Height});
            break;
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

bool sw::Window::OnDpiChanged(int dpiX, int dpiY, RECT &newRect)
{
    DisableLayout();
    Dip::Update(dpiX, dpiY);

    QueryAllElements([](UIElement *item) {
        item->LayoutUpdateCondition |= LayoutUpdateCondition::Supressed;
        item->UpdateFont();
        item->LayoutUpdateCondition &= ~LayoutUpdateCondition::Supressed;
        return true;
    });

    UpdateInternalRect();
    Rect = newRect;

    EnableLayout();
    return true;
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
        hOwner = GetActiveWindow();
        hOwner = (hOwner == hwnd) ? NULL : hOwner;
    }

    if (hOwner != NULL) {
        SetWindowLongPtrW(hwnd, GWLP_HWNDPARENT, reinterpret_cast<LONG_PTR>(hOwner));
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

    if (_disableLayoutCount == 1) {
        RaisePropertyChanged(&Window::IsLayoutDisabled);
    }
    return true;
}

bool sw::Window::EnableLayout(bool reset)
{
    if (!CheckAccess()) {
        return false; // 只能在创建窗口的线程调用
    }

    bool oldValue = _IsLayoutDisabled();

    if (reset) {
        _disableLayoutCount = 0;
    } else {
        _disableLayoutCount = Utils::Max(0, _disableLayoutCount - 1);
    }

    bool newValue = _IsLayoutDisabled();

    if (oldValue != newValue) {
        RaisePropertyChanged(&Window::IsLayoutDisabled);
    }
    if (!newValue) {
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

bool sw::Window::SizeToContent()
{
    if (!IsRootElement()) {
        return false; // 只对顶级窗口有效
    }

    if (!AutoSize) {
        return false; // 依赖AutoSize属性
    }

    sw::Size measureSize{INFINITY, INFINITY};
    Measure(measureSize);

    sw::Size desireSize  = GetDesireSize();
    sw::Rect windowRect  = Rect;
    sw::Thickness margin = Margin;

    Arrange(sw::Rect{
        windowRect.left - margin.left,
        windowRect.top - margin.top,
        desireSize.width, desireSize.height});

    return true;
}

bool sw::Window::_IsLayoutDisabled() const noexcept
{
    return _disableLayoutCount > 0;
}

void sw::Window::_CenterWindow(const sw::Rect &rect)
{
    auto windowRect = Rect.Get();

    Rect = sw::Rect{
        rect.left + (rect.width - windowRect.width) / 2,
        rect.top + (rect.height - windowRect.height) / 2,
        windowRect.width, windowRect.height};
}

void sw::Window::_ClampMinMaxSize(PMINMAXINFO pInfo)
{
    if (pInfo == nullptr) {
        return;
    }

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
