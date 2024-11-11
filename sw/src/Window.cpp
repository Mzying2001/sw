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
    int _windowCount = 0;

    /**
     * @brief 窗口句柄保存Window指针的属性名称
     */
    constexpr wchar_t _WindowPtrProp[] = L"SWPROP_WindowPtr";

    /**
     * @brief      通过窗口句柄获取Window指针
     * @param hwnd 窗口句柄
     * @return     若函数成功则返回对象的指针，否则返回nullptr
     */
    sw::Window *_GetWindowPtr(HWND hwnd)
    {
        return reinterpret_cast<sw::Window *>(GetPropW(hwnd, _WindowPtrProp));
    }

    /**
     * @brief      关联窗口句柄与Window对象
     * @param hwnd 窗口句柄
     * @param wnd  与句柄关联的对象
     */
    void _SetWindowPtr(HWND hwnd, sw::Window &wnd)
    {
        SetPropW(hwnd, _WindowPtrProp, reinterpret_cast<HANDLE>(&wnd));
    }

    /**
     * @brief DPI更新时调用该函数递归地更新所有子项的字体
     */
    void _UpdateFontForAllChild(sw::UIElement &element)
    {
        element.UpdateFont();
        int count = element.ChildCount;
        for (int i = 0; i < count; ++i) {
            _UpdateFontForAllChild(element[i]);
        }
    }

    /**
     * @brief  获取窗口默认图标（即当前exe图标）
     * @return 图标句柄
     */
    HICON _GetWindowDefaultIcon()
    {
        static HICON hIcon = ExtractIconW(sw::App::Instance, sw::App::ExePath->c_str(), 0);
        return hIcon;
    }
}

/**
 * @brief 程序的当前活动窗体
 */
const sw::ReadOnlyPtrProperty<sw::Window *> sw::Window::ActiveWindow(
    []() -> sw::Window * {
        HWND hwnd = GetActiveWindow();
        return _GetWindowPtr(hwnd);
    } //
);

/**
 * @brief 当前已创建的窗口数
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
              return this->_startupLocation;
          },
          // set
          [this](const WindowStartupLocation &value) {
              this->_startupLocation = value;
          }),

      State(
          // get
          [this]() -> WindowState {
              HWND hwnd = this->Handle;
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
          [this]() -> bool {
              return this->GetStyle(WS_SIZEBOX);
          },
          // set
          [this](const bool &value) {
              this->SetStyle(WS_SIZEBOX, value);
          }),

      MaximizeBox(
          // get
          [this]() -> bool {
              return this->GetStyle(WS_MAXIMIZEBOX);
          },
          // set
          [this](const bool &value) {
              this->SetStyle(WS_MAXIMIZEBOX, value);
          }),

      MinimizeBox(
          // get
          [this]() -> bool {
              return this->GetStyle(WS_MINIMIZEBOX);
          },
          // set
          [this](const bool &value) {
              this->SetStyle(WS_MINIMIZEBOX, value);
          }),

      Topmost(
          // get
          [this]() -> bool {
              return this->GetExtendedStyle(WS_EX_TOPMOST);
          },
          // set
          [this](const bool &value) {
              /*this->SetExtendedStyle(WS_EX_TOPMOST, value);*/
              HWND hWndInsertAfter = value ? HWND_TOPMOST : HWND_NOTOPMOST;
              SetWindowPos(this->Handle, hWndInsertAfter, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
          }),

      ToolWindow(
          // get
          [this]() -> bool {
              return this->GetExtendedStyle(WS_EX_TOOLWINDOW);
          },
          // set
          [this](const bool &value) {
              this->SetExtendedStyle(WS_EX_TOOLWINDOW, value);
          }),

      MaxWidth(
          // get
          [this]() -> double {
              return this->_maxWidth;
          },
          // set
          [this](const double &value) {
              this->_maxWidth = value;
              this->Width     = this->Width;
          }),

      MaxHeight(
          // get
          [this]() -> double {
              return this->_maxHeight;
          },
          // set
          [this](const double &value) {
              this->_maxHeight = value;
              this->Height     = this->Height;
          }),

      MinWidth(
          // get
          [this]() -> double {
              return this->_minWidth;
          },
          // set
          [this](const double &value) {
              this->_minWidth = value;
              this->Width     = this->Width;
          }),

      MinHeight(
          // get
          [this]() -> double {
              return this->_minHeight;
          },
          // set
          [this](const double &value) {
              this->_minHeight = value;
              this->Height     = this->Height;
          }),

      Menu(
          // get
          [this]() -> sw::Menu * {
              return this->_menu;
          },
          // set
          [this](sw::Menu *value) {
              this->_menu = value;
              SetMenu(this->Handle, value != nullptr ? value->GetHandle() : NULL);
          }),

      IsModal(
          // get
          [this]() -> bool {
              return this->_modalOwner != nullptr;
          }),

      Owner(
          // get
          [this]() -> Window * {
              HWND hOwner = reinterpret_cast<HWND>(GetWindowLongPtrW(this->Handle, GWLP_HWNDPARENT));
              return _GetWindowPtr(hOwner);
          },
          // set
          [this](Window *value) {
              SetWindowLongPtrW(this->Handle, GWLP_HWNDPARENT, reinterpret_cast<LONG_PTR>(value ? value->Handle.Get() : NULL));
          })
{
    this->InitWindow(L"Window", WS_OVERLAPPEDWINDOW, 0);
    _SetWindowPtr(this->Handle, *this);
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
            if (this->IsModal) {
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
            auto pInfo = reinterpret_cast<PMINMAXINFO>(refMsg.lParam);
            // 按照设置限制窗口大小
            if (this->_maxWidth > 0) {
                LONG maxWidth           = Dip::DipToPxX(this->_maxWidth);
                pInfo->ptMaxTrackSize.x = Utils::Min(pInfo->ptMaxTrackSize.x, maxWidth);
            }
            if (this->_maxHeight > 0) {
                LONG maxHeight          = Dip::DipToPxY(this->_maxHeight);
                pInfo->ptMaxTrackSize.y = Utils::Min(pInfo->ptMaxTrackSize.y, maxHeight);
            }
            if (this->_minWidth > 0) {
                LONG minWidth           = Dip::DipToPxX(this->_minWidth);
                pInfo->ptMinTrackSize.x = Utils::Max(pInfo->ptMinTrackSize.x, minWidth);
            }
            if (this->_minHeight > 0) {
                LONG minHeight          = Dip::DipToPxY(this->_minHeight);
                pInfo->ptMinTrackSize.y = Utils::Max(pInfo->ptMinTrackSize.y, minHeight);
            }
            return 0;
        }

        case WM_DPICHANGED: {
            this->OnDpiChanged(LOWORD(refMsg.wParam), HIWORD(refMsg.wParam));
            return 0;
        }

        case WM_ACTIVATE: {
            (refMsg.wParam == WA_INACTIVE)
                ? this->OnInactived()
                : this->OnActived();
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
        Window *owner = this->Owner;
        if (owner) {
            sw::Rect windowRect = this->Rect;
            sw::Rect ownerRect  = owner->Rect;
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

void sw::Window::OnDpiChanged(int dpiX, int dpiY)
{
    bool layoutDisabled = this->IsLayoutDisabled();

    Dip::Update(dpiX, dpiY);
    this->DisableLayout();

    {
        // Windows在DIP改变时会自动调整窗口大小，此时会先触发WM_WINDOWPOSCHANGED，再触发WM_DPICHANGED
        // 因此在先触发的WM_WINDOWPOSCHANGED消息中，（Dip类中）DPI信息未更新，从而导致窗口的Rect数据错误
        // 此处在更新DPI信息后手动发送一个WM_WINDOWPOSCHANGED以修正窗口的Rect数据

        HWND hwnd = this->Handle;

        RECT rect;
        GetWindowRect(hwnd, &rect);

        WINDOWPOS pos{};
        pos.hwnd  = hwnd;
        pos.x     = rect.left;
        pos.y     = rect.top;
        pos.cx    = rect.right - rect.left;
        pos.cy    = rect.bottom - rect.top;
        pos.flags = SWP_NOACTIVATE | SWP_NOZORDER;

        this->SendMessageW(WM_WINDOWPOSCHANGED, 0, reinterpret_cast<LPARAM>(&pos));
    }

    _UpdateFontForAllChild(*this);

    if (!layoutDisabled) {
        this->EnableLayout();
    }
}

void sw::Window::Show(int nCmdShow)
{
    this->WndBase::Show(nCmdShow);
}

void sw::Window::ShowDialog(Window &owner)
{
    if (this == &owner || this->IsModal || this->IsDestroyed) {
        return;
    }

    this->Owner       = &owner;
    this->_modalOwner = &owner;

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

void sw::Window::SizeToContent()
{
    if (!this->IsRootElement()) {
        return; // 只对顶级窗口有效
    }

    // 该函数需要AutoSize为true，这里先备份其值以做后续恢复
    bool oldAutoSize = this->AutoSize;
    this->AutoSize   = true;

    // measure
    sw::Size measureSize(INFINITY, INFINITY);
    this->Measure(measureSize);

    // arrange
    sw::Size desireSize  = this->GetDesireSize();
    sw::Rect windowRect  = this->Rect;
    sw::Thickness margin = this->Margin;
    this->Arrange(sw::Rect(windowRect.left - margin.left, windowRect.top - margin.top, desireSize.width, desireSize.height));

    // 恢复AutoSize属性的值
    this->AutoSize = oldAutoSize;
}
