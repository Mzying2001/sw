#include "WndBase.h"
#include <atomic>

namespace
{
    /**
     * @brief _check字段的值，用于判断给定指针是否为指向WndBase的指针
     */
    constexpr uint32_t _WndBaseMagicNumber = 0x946dba7e;

    /**
     * @brief 窗口句柄保存WndBase指针的属性名称
     */
    constexpr wchar_t _WndBasePtrProp[] = L"SWPROP_WndBasePtr";

    /**
     * @brief 窗口类名
     */
    constexpr wchar_t _WindowClassName[] = L"sw::Window";

    /**
     * @brief 控件id计数器
     */
    std::atomic<int> _controlIdCounter{1073741827};
}

sw::WndBase::WndBase()
    : _check(_WndBaseMagicNumber),

      Handle(
          // get
          [this]() -> HWND {
              return this->_hwnd;
          }),

      Font(
          // get
          [this]() -> sw::Font {
              return this->_font;
          },
          // set
          [this](const sw::Font &value) {
              this->_font = value;
              this->UpdateFont();
          }),

      FontName(
          // get
          [this]() -> std::wstring {
              return this->_font.name;
          },
          // set
          [this](const std::wstring &value) {
              if (this->_font.name != value) {
                  this->_font.name = value;
                  this->UpdateFont();
              }
          }),

      FontSize(
          // get
          [this]() -> double {
              return this->_font.size;
          },
          // set
          [this](const double &value) {
              if (this->_font.size != value) {
                  this->_font.size = value;
                  this->UpdateFont();
              }
          }),

      FontWeight(
          // get
          [this]() -> sw::FontWeight {
              return this->_font.weight;
          },
          // set
          [this](const sw::FontWeight &value) {
              if (this->_font.weight != value) {
                  this->_font.weight = value;
                  this->UpdateFont();
              }
          }),

      Rect(
          // get
          [this]() -> sw::Rect {
              return this->_rect;
          },
          // set
          [this](const sw::Rect &value) {
              if (this->_rect != value) {
                  int left   = Dip::DipToPxX(value.left);
                  int top    = Dip::DipToPxY(value.top);
                  int width  = Dip::DipToPxX(value.width);
                  int height = Dip::DipToPxY(value.height);
                  SetWindowPos(this->_hwnd, NULL, left, top, width, height, SWP_NOACTIVATE | SWP_NOZORDER);
              }
          }),

      Left(
          // get
          [this]() -> double {
              return this->_rect.left;
          },
          // set
          [this](const double &value) {
              if (this->_rect.left != value) {
                  int x = Dip::DipToPxX(value);
                  int y = Dip::DipToPxY(this->_rect.top);
                  SetWindowPos(this->_hwnd, NULL, x, y, 0, 0, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE);
              }
          }),

      Top(
          // get
          [this]() -> double {
              return this->_rect.top;
          },
          // set
          [this](const double &value) {
              if (this->_rect.top != value) {
                  int x = Dip::DipToPxX(this->_rect.left);
                  int y = Dip::DipToPxY(value);
                  SetWindowPos(this->_hwnd, NULL, x, y, 0, 0, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE);
              }
          }),

      Width(
          // get
          [this]() -> double {
              return this->_rect.width;
          },
          // set
          [this](const double &value) {
              if (this->_rect.width != value) {
                  int cx = Dip::DipToPxX(value);
                  int cy = Dip::DipToPxY(this->_rect.height);
                  SetWindowPos(this->_hwnd, NULL, 0, 0, cx, cy, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);
              }
          }),

      Height(
          // get
          [this]() -> double {
              return this->_rect.height;
          },
          // set
          [this](const double &value) {
              if (this->_rect.height != value) {
                  int cx = Dip::DipToPxX(this->_rect.width);
                  int cy = Dip::DipToPxY(value);
                  SetWindowPos(this->_hwnd, NULL, 0, 0, cx, cy, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);
              }
          }),

      ClientRect(
          // get
          [this]() -> sw::Rect {
              RECT rect;
              GetClientRect(this->_hwnd, &rect);
              return rect;
          }),

      ClientWidth(
          // get
          [this]() -> double {
              return this->ClientRect->width;
          }),

      ClientHeight(
          // get
          [this]() -> double {
              return this->ClientRect->height;
          }),

      Enabled(
          // get
          [this]() -> bool {
              return IsWindowEnabled(this->_hwnd);
          },
          // set
          [this](const bool &value) {
              EnableWindow(this->_hwnd, value);
          }),

      Visible(
          // get
          [this]() -> bool {
              return this->GetStyle(WS_VISIBLE);
          },
          // set
          [this](const bool &value) {
              ShowWindow(this->_hwnd, value ? SW_SHOW : SW_HIDE);
              this->VisibleChanged(value);
          }),

      Text(
          // get
          [this]() -> std::wstring {
              return this->GetInternalText();
          },
          // set
          [this](const std::wstring &value) {
              this->SetInternalText(value);
          }),

      Focused(
          // get
          [this]() -> bool {
              return this->_focused;
          },
          // set
          [this](const bool &value) {
              SetFocus(value ? this->_hwnd : NULL);
          }),

      Parent(
          // get
          [this]() -> WndBase * {
              HWND hwnd = GetParent(this->_hwnd);
              return WndBase::GetWndBase(hwnd);
          }),

      IsDestroyed(
          // get
          [this]() -> bool {
              return this->_isDestroyed;
          }),

      AcceptFiles(
          // get
          [this]() -> bool {
              return this->GetExtendedStyle(WS_EX_ACCEPTFILES);
          },
          // set
          [this](const bool &value) {
              this->SetExtendedStyle(WS_EX_ACCEPTFILES, value);
          }),

      IsControl(
          // get
          [this]() -> bool {
              return this->_isControl;
          }),

      ClassName(
          // get
          [this]() -> std::wstring {
              std::wstring result(256, L'\0');
              result.resize(GetClassNameW(this->_hwnd, &result[0], (int)result.size()));
              return result;
          }),

      IsGroupStart(
          // get
          [this]() -> bool {
              return this->GetStyle(WS_GROUP);
          },
          // set
          [this](const bool &value) {
              this->SetStyle(WS_GROUP, value);
          }),

      IsMouseCaptured(
          // get
          [this]() -> bool {
              return GetCapture() == this->_hwnd;
          })
{
    this->_font = sw::Font::GetDefaultFont();
}

sw::WndBase::~WndBase()
{
    if (this->_hwnd != NULL && !this->_isDestroyed) {
        DestroyWindow(this->_hwnd);
    }
    if (this->_hfont != NULL) {
        DeleteObject(this->_hfont);
    }

    // 将_check字段置零，标记当前对象无效
    const_cast<uint32_t &>(this->_check) = 0;
}

sw::UIElement *sw::WndBase::ToUIElement()
{
    return nullptr;
}

sw::Control *sw::WndBase::ToControl()
{
    return nullptr;
}

sw::Window *sw::WndBase::ToWindow()
{
    return nullptr;
}

bool sw::WndBase::Equals(const WndBase &other) const
{
    return this == &other;
}

std::wstring sw::WndBase::ToString() const
{
    return L"WndBase{ClassName=" + this->ClassName + L", Handle=" + std::to_wstring(reinterpret_cast<uintptr_t>(this->_hwnd)) + L"}";
}

void sw::WndBase::InitWindow(LPCWSTR lpWindowName, DWORD dwStyle, DWORD dwExStyle)
{
    static thread_local ATOM wndClsAtom = 0;

    if (this->_hwnd != NULL) {
        return;
    }

    if (wndClsAtom == 0) {
        WNDCLASSEXW wc{};
        wc.cbSize        = sizeof(wc);
        wc.hInstance     = App::Instance;
        wc.lpfnWndProc   = WndBase::_WndProc;
        wc.lpszClassName = _WindowClassName;
        wc.hCursor       = CursorHelper::GetCursorHandle(StandardCursor::Arrow);
        wndClsAtom       = RegisterClassExW(&wc);
    }

    if (lpWindowName) {
        this->_text = lpWindowName;
    }

    this->_hwnd = CreateWindowExW(
        dwExStyle,           // Optional window styles
        _WindowClassName,    // Window class
        this->_text.c_str(), // Window text
        dwStyle,             // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        NULL,          // Parent window
        NULL,          // Menu
        App::Instance, // Instance handle
        this           // Additional application data
    );

    WndBase::_SetWndBase(this->_hwnd, *this);

    RECT rect;
    GetWindowRect(this->_hwnd, &rect);
    this->_rect = rect;

    this->HandleInitialized(this->_hwnd);
    this->UpdateFont();
}

void sw::WndBase::InitControl(LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, DWORD dwExStyle, LPVOID lpParam)
{
    if (this->_hwnd != NULL) {
        return;
    }

    if (lpWindowName) {
        this->_text = lpWindowName;
    }

    WndBase *container =
        WndBase::_GetControlInitContainer();

    HMENU id = reinterpret_cast<HMENU>(
        static_cast<uintptr_t>(WndBase::_NextControlId()));

    this->_hwnd = CreateWindowExW(
        dwExStyle,           // Optional window styles
        lpClassName,         // Window class
        this->_text.c_str(), // Window text
        dwStyle,             // Window style
        0, 0, 0, 0,          // Size and position
        container->_hwnd,    // Parent window
        id,                  // Control id
        App::Instance,       // Instance handle
        lpParam              // Additional application data
    );

    this->_isControl = true;
    WndBase::_SetWndBase(this->_hwnd, *this);

    this->_originalWndProc = reinterpret_cast<WNDPROC>(SetWindowLongPtrW(
        this->_hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndBase::_WndProc)));

    this->HandleInitialized(this->_hwnd);
    this->UpdateFont();
}

LRESULT sw::WndBase::DefaultWndProc(const ProcMsg &refMsg)
{
    if (this->_originalWndProc == nullptr ||
        this->_originalWndProc == WndBase::_WndProc) {
        return DefWindowProcW(refMsg.hwnd, refMsg.uMsg, refMsg.wParam, refMsg.lParam);
    } else {
        return CallWindowProcW(this->_originalWndProc, refMsg.hwnd, refMsg.uMsg, refMsg.wParam, refMsg.lParam);
    }
}

LRESULT sw::WndBase::WndProc(const ProcMsg &refMsg)
{
    switch (refMsg.uMsg) {
        case WM_CREATE: {
            return this->OnCreate() ? 0 : this->DefaultWndProc(refMsg);
        }

        case WM_CLOSE: {
            return this->OnClose() ? 0 : this->DefaultWndProc(refMsg);
        }

        case WM_DESTROY: {
            return this->OnDestroy() ? 0 : this->DefaultWndProc(refMsg);
        }

        case WM_NCDESTROY: {
            LRESULT result     = this->DefaultWndProc(refMsg);
            this->_isDestroyed = true;
            return result;
        }

        case WM_PAINT: {
            LRESULT result = this->OnPaint() ? 0 : this->DefaultWndProc(refMsg);
            this->OnEndPaint();
            return result;
        }

        case WM_NCPAINT: {
            LRESULT result = this->OnNcPaint(reinterpret_cast<HRGN>(refMsg.wParam)) ? 0 : this->DefaultWndProc(refMsg);
            this->OnEndNcPaint();
            return result;
        }

        case WM_WINDOWPOSCHANGED: {
            auto pWndPos = reinterpret_cast<PWINDOWPOS>(refMsg.lParam);
            if ((pWndPos->flags & SWP_NOMOVE) == 0) {
                this->_rect.left = Dip::PxToDipX(pWndPos->x);
                this->_rect.top  = Dip::PxToDipY(pWndPos->y);
            }
            if ((pWndPos->flags & SWP_NOSIZE) == 0) {
                this->_rect.width  = Dip::PxToDipX(pWndPos->cx);
                this->_rect.height = Dip::PxToDipY(pWndPos->cy);
            }
            return this->DefaultWndProc(refMsg);
        }

        case WM_MOVE: {
            int xPos = (int)(short)LOWORD(refMsg.lParam); // horizontal position
            int yPos = (int)(short)HIWORD(refMsg.lParam); // vertical position
            return this->OnMove(POINT{xPos, yPos}) ? 0 : this->DefaultWndProc(refMsg);
        }

        case WM_SIZE: {
            int width  = LOWORD(refMsg.lParam); // the new width of the client area
            int height = HIWORD(refMsg.lParam); // the new height of the client area
            return this->OnSize(SIZE{width, height}) ? 0 : this->DefaultWndProc(refMsg);
        }

        case WM_SETTEXT: {
            LRESULT result = this->DefaultWndProc(refMsg);
            if (result == TRUE) {
                this->_text = reinterpret_cast<PCWSTR>(refMsg.lParam);
                this->OnTextChanged();
            }
            return result;
        }

        case WM_SETFOCUS: {
            this->_focused = true;
            return this->OnSetFocus((HWND)refMsg.wParam) ? 0 : this->DefaultWndProc(refMsg);
        }

        case WM_KILLFOCUS: {
            this->_focused = false;
            return this->OnKillFocus((HWND)refMsg.wParam) ? 0 : this->DefaultWndProc(refMsg);
        }

        case WM_MOUSEMOVE: {
            POINT mousePosition{GET_X_LPARAM(refMsg.lParam), GET_Y_LPARAM(refMsg.lParam)};
            return this->OnMouseMove(Point(mousePosition), (MouseKey)refMsg.wParam) ? 0 : this->DefaultWndProc(refMsg);
        }

        case WM_MOUSELEAVE: {
            return this->OnMouseLeave() ? 0 : this->DefaultWndProc(refMsg);
        }

        case WM_MOUSEWHEEL: {
            int fwKeys = GET_KEYSTATE_WPARAM(refMsg.wParam);
            int zDelta = GET_WHEEL_DELTA_WPARAM(refMsg.wParam);
            POINT mousePosition{GET_X_LPARAM(refMsg.lParam), GET_Y_LPARAM(refMsg.lParam)};
            return this->OnMouseWheel(zDelta, Point(mousePosition), (MouseKey)fwKeys) ? 0 : this->DefaultWndProc(refMsg);
        }

        case WM_LBUTTONDOWN: {
            POINT mousePosition{GET_X_LPARAM(refMsg.lParam), GET_Y_LPARAM(refMsg.lParam)};
            return this->OnMouseLeftButtonDown(Point(mousePosition), (MouseKey)refMsg.wParam) ? 0 : this->DefaultWndProc(refMsg);
        }

        case WM_LBUTTONUP: {
            POINT mousePosition{GET_X_LPARAM(refMsg.lParam), GET_Y_LPARAM(refMsg.lParam)};
            return this->OnMouseLeftButtonUp(Point(mousePosition), (MouseKey)refMsg.wParam) ? 0 : this->DefaultWndProc(refMsg);
        }

        case WM_LBUTTONDBLCLK: {
            POINT mousePosition{GET_X_LPARAM(refMsg.lParam), GET_Y_LPARAM(refMsg.lParam)};
            return this->OnMouseLeftButtonDoubleClick(Point(mousePosition), (MouseKey)refMsg.wParam) ? 0 : this->DefaultWndProc(refMsg);
        }

        case WM_RBUTTONDOWN: {
            POINT mousePosition{GET_X_LPARAM(refMsg.lParam), GET_Y_LPARAM(refMsg.lParam)};
            return this->OnMouseRightButtonDown(Point(mousePosition), (MouseKey)refMsg.wParam) ? 0 : this->DefaultWndProc(refMsg);
        }

        case WM_RBUTTONUP: {
            POINT mousePosition{GET_X_LPARAM(refMsg.lParam), GET_Y_LPARAM(refMsg.lParam)};
            return this->OnMouseRightButtonUp(Point(mousePosition), (MouseKey)refMsg.wParam) ? 0 : this->DefaultWndProc(refMsg);
        }

        case WM_RBUTTONDBLCLK: {
            POINT mousePosition{GET_X_LPARAM(refMsg.lParam), GET_Y_LPARAM(refMsg.lParam)};
            return this->OnMouseRightButtonDoubleClick(Point(mousePosition), (MouseKey)refMsg.wParam) ? 0 : this->DefaultWndProc(refMsg);
        }

        case WM_MBUTTONDOWN: {
            POINT mousePosition{GET_X_LPARAM(refMsg.lParam), GET_Y_LPARAM(refMsg.lParam)};
            return this->OnMouseMiddleButtonDown(Point(mousePosition), (MouseKey)refMsg.wParam) ? 0 : this->DefaultWndProc(refMsg);
        }

        case WM_MBUTTONUP: {
            POINT mousePosition{GET_X_LPARAM(refMsg.lParam), GET_Y_LPARAM(refMsg.lParam)};
            return this->OnMouseMiddleButtonUp(Point(mousePosition), (MouseKey)refMsg.wParam) ? 0 : this->DefaultWndProc(refMsg);
        }

        case WM_MBUTTONDBLCLK: {
            POINT mousePosition{GET_X_LPARAM(refMsg.lParam), GET_Y_LPARAM(refMsg.lParam)};
            return this->OnMouseMiddleButtonDoubleClick(Point(mousePosition), (MouseKey)refMsg.wParam) ? 0 : this->DefaultWndProc(refMsg);
        }

        case WM_CHAR: {
            return this->OnChar((wchar_t)refMsg.wParam, refMsg.lParam) ? 0 : this->DefaultWndProc(refMsg);
        }

        case WM_DEADCHAR: {
            return this->OnDeadChar((wchar_t)refMsg.wParam, refMsg.lParam) ? 0 : this->DefaultWndProc(refMsg);
        }

        case WM_KEYDOWN: {
            return this->OnKeyDown((VirtualKey)refMsg.wParam, refMsg.lParam) ? 0 : this->DefaultWndProc(refMsg);
        }

        case WM_KEYUP: {
            return this->OnKeyUp((VirtualKey)refMsg.wParam, refMsg.lParam) ? 0 : this->DefaultWndProc(refMsg);
        }

        case WM_SYSCHAR: {
            return this->OnSysChar((wchar_t)refMsg.wParam, refMsg.lParam) ? 0 : this->DefaultWndProc(refMsg);
        }

        case WM_SYSDEADCHAR: {
            return this->OnSysDeadChar((wchar_t)refMsg.wParam, refMsg.lParam) ? 0 : this->DefaultWndProc(refMsg);
        }

        case WM_SYSKEYDOWN: {
            return this->OnSysKeyDown((VirtualKey)refMsg.wParam, refMsg.lParam) ? 0 : this->DefaultWndProc(refMsg);
        }

        case WM_SYSKEYUP: {
            return this->OnSysKeyUp((VirtualKey)refMsg.wParam, refMsg.lParam) ? 0 : this->DefaultWndProc(refMsg);
        }

        case WM_COMMAND: {
            if (refMsg.lParam != (LPARAM)NULL) {
                // 接收到控件消息，获取控件对象
                WndBase *pControl = WndBase::GetWndBase(reinterpret_cast<HWND>(refMsg.lParam));
                // Windows一些控件的内部句柄也是使用WM_COMMAND实现一些功能，如可编辑状态的组合框
                // 此处尝试获取控件WndBase对象，若为nullptr说明为内部句柄，此时调用原来的WndProc
                if (pControl == nullptr) {
                    this->DefaultWndProc(refMsg);
                } else {
                    this->OnControlCommand(pControl, HIWORD(refMsg.wParam), LOWORD(refMsg.wParam));
                }
            } else {
                // 接收到菜单或快捷键消息
                int id = LOWORD(refMsg.wParam);
                HIWORD(refMsg.wParam) ? this->OnAcceleratorCommand(id) : this->OnMenuCommand(id);
            }
            return 0;
        }

        case WM_NOTIFY: {
            LRESULT result = 0;

            NMHDR *pNMHDR = reinterpret_cast<NMHDR *>(refMsg.lParam);
            bool handled  = this->OnNotify(pNMHDR, result);

            if (!handled) {
                WndBase *pWnd = WndBase::GetWndBase(pNMHDR->hwndFrom);
                if (pWnd) handled = pWnd->OnNotified(pNMHDR, result);
            }

            return handled ? result : this->DefaultWndProc(refMsg);
        }

        case WM_CTLCOLORMSGBOX:
        case WM_CTLCOLOREDIT:
        case WM_CTLCOLORLISTBOX:
        case WM_CTLCOLORBTN:
        case WM_CTLCOLORDLG:
        case WM_CTLCOLORSCROLLBAR:
        case WM_CTLCOLORSTATIC: {
            HWND hControl     = (HWND)refMsg.lParam;
            WndBase *pControl = WndBase::GetWndBase(hControl);
            if (pControl == nullptr) {
                return this->DefaultWndProc(refMsg);
            } else {
                HDC hdc       = (HDC)refMsg.wParam;
                HBRUSH hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
                return this->OnCtlColor(pControl, hdc, hBrush) ? (LRESULT)hBrush : this->DefaultWndProc(refMsg);
            }
        }

        case WM_SETCURSOR: {
            HWND hwnd   = (HWND)refMsg.wParam;
            int hitTest = LOWORD(refMsg.lParam);
            int message = HIWORD(refMsg.lParam);
            bool result = false;
            return this->OnSetCursor(hwnd, (HitTestResult)hitTest, message, result) ? result : this->DefaultWndProc(refMsg);
        }

        case WM_CONTEXTMENU: {
            WndBase *pWnd = WndBase::GetWndBase((HWND)refMsg.wParam);
            if (pWnd == nullptr) {
                return this->DefaultWndProc(refMsg);
            } else {
                int xPos = GET_X_LPARAM(refMsg.lParam);
                int yPos = GET_Y_LPARAM(refMsg.lParam);
                bool res = this->OnContextMenu(xPos == -1 && yPos == -1, POINT{xPos, yPos});
                return res ? 0 : this->DefaultWndProc(refMsg);
            }
        }

        case WM_MENUSELECT: {
            HMENU hMenu = (HMENU)refMsg.lParam;
            int id      = LOWORD(refMsg.wParam);
            int flags   = HIWORD(refMsg.wParam);
            return this->OnMenuSelect(hMenu, id, flags) ? 0 : this->DefaultWndProc(refMsg);
        }

        case WM_VSCROLL: {
            if (!refMsg.lParam /*refMsg.lParam == NULL*/) {
                return this->OnVerticalScroll(LOWORD(refMsg.wParam), (int16_t)HIWORD(refMsg.wParam)) ? 0 : this->DefaultWndProc(refMsg);
            }
            WndBase *pWnd = WndBase::GetWndBase(reinterpret_cast<HWND>(refMsg.lParam));
            if (pWnd) {
                return pWnd->OnVerticalScroll(LOWORD(refMsg.wParam), (int16_t)HIWORD(refMsg.wParam)) ? 0 : this->DefaultWndProc(refMsg);
            } else {
                return this->DefaultWndProc(refMsg);
            }
        }

        case WM_HSCROLL: {
            if (!refMsg.lParam /*refMsg.lParam == NULL*/) {
                return this->OnHorizontalScroll(LOWORD(refMsg.wParam), (int16_t)HIWORD(refMsg.wParam)) ? 0 : this->DefaultWndProc(refMsg);
            }
            WndBase *pWnd = WndBase::GetWndBase(reinterpret_cast<HWND>(refMsg.lParam));
            if (pWnd) {
                return pWnd->OnHorizontalScroll(LOWORD(refMsg.wParam), (int16_t)HIWORD(refMsg.wParam)) ? 0 : this->DefaultWndProc(refMsg);
            } else {
                return this->DefaultWndProc(refMsg);
            }
        }

        case WM_ENABLE: {
            return this->OnEnabledChanged(refMsg.wParam) ? 0 : this->DefaultWndProc(refMsg);
        }

        case WM_NCHITTEST: {
            Point testPoint      = POINT{GET_X_LPARAM(refMsg.lParam), GET_Y_LPARAM(refMsg.lParam)};
            HitTestResult result = (HitTestResult)this->DefaultWndProc(refMsg);
            this->OnNcHitTest(testPoint, result);
            return (LRESULT)result;
        }

        case WM_ERASEBKGND: {
            LRESULT result = 0;
            return this->OnEraseBackground(reinterpret_cast<HDC>(refMsg.wParam), result) ? result : this->DefaultWndProc(refMsg);
        }

        case WM_DRAWITEM: {
            auto pDrawItem = reinterpret_cast<DRAWITEMSTRUCT *>(refMsg.lParam);
            if (this->OnDrawItem((int)refMsg.wParam, pDrawItem)) {
                return TRUE;
            }
            if (pDrawItem->CtlType != ODT_MENU && pDrawItem->hwndItem != NULL) {
                WndBase *pWnd = GetWndBase(pDrawItem->hwndItem);
                if (pWnd && pWnd->OnDrawItemSelf(pDrawItem)) return TRUE;
            }
            return this->DefaultWndProc(refMsg);
        }

        case WM_MEASUREITEM: {
            auto pMeasure = reinterpret_cast<MEASUREITEMSTRUCT *>(refMsg.lParam);
            if (this->OnMeasureItem((int)refMsg.wParam, pMeasure)) {
                return TRUE;
            }
            if (pMeasure->CtlType != ODT_MENU) {
                WndBase *pWnd = GetWndBase(GetDlgItem(this->_hwnd, (int)refMsg.wParam));
                if (pWnd && pWnd->OnMeasureItemSelf(pMeasure)) return TRUE;
            }
            return this->DefaultWndProc(refMsg);
        }

        case WM_DROPFILES: {
            return this->OnDropFiles(reinterpret_cast<HDROP>(refMsg.wParam)) ? 0 : this->DefaultWndProc(refMsg);
        }

        case WM_InvokeAction: {
            auto pAction = reinterpret_cast<Action<> *>(refMsg.lParam);
            if (pAction && *pAction) pAction->Invoke();
            if (refMsg.wParam) delete pAction;
            return 0;
        }

        default: {
            return this->DefaultWndProc(refMsg);
        }
    }
}

void sw::WndBase::UpdateInternalText()
{
    int len = GetWindowTextLengthW(this->_hwnd);

    if (len <= 0) {
        this->_text.clear();
        return;
    }

    // wchar_t *buf = new wchar_t[len + 1];
    // GetWindowTextW(this->_hwnd, buf, len + 1);
    // this->_text = buf;
    // delete[] buf;

    this->_text.resize(len + 1);
    GetWindowTextW(this->_hwnd, &this->_text[0], len + 1);
    this->_text.resize(len);
}

std::wstring &sw::WndBase::GetInternalText()
{
    return this->_text;
}

void sw::WndBase::SetInternalText(const std::wstring &value)
{
    SetWindowTextW(this->_hwnd, value.c_str());
}

bool sw::WndBase::OnCreate()
{
    return false;
}

bool sw::WndBase::OnClose()
{
    return false;
}

bool sw::WndBase::OnDestroy()
{
    return false;
}

bool sw::WndBase::OnPaint()
{
    return false;
}

void sw::WndBase::OnEndPaint()
{
}

bool sw::WndBase::OnNcPaint(HRGN hRgn)
{
    return false;
}

void sw::WndBase::OnEndNcPaint()
{
}

bool sw::WndBase::OnMove(const Point &newClientPosition)
{
    return false;
}

bool sw::WndBase::OnSize(const Size &newClientSize)
{
    return false;
}

void sw::WndBase::OnTextChanged()
{
}

bool sw::WndBase::OnSetFocus(HWND hPrevFocus)
{
    return false;
}

bool sw::WndBase::OnKillFocus(HWND hNextFocus)
{
    return false;
}

bool sw::WndBase::OnMouseMove(const Point &mousePosition, MouseKey keyState)
{
    return false;
}

bool sw::WndBase::OnMouseLeave()
{
    return false;
}

bool sw::WndBase::OnMouseWheel(int wheelDelta, const Point &mousePosition, MouseKey keyState)
{
    return false;
}

bool sw::WndBase::OnMouseLeftButtonDown(const Point &mousePos, MouseKey keyState)
{
    return false;
}

bool sw::WndBase::OnMouseLeftButtonUp(const Point &mousePos, MouseKey keyState)
{
    return false;
}

bool sw::WndBase::OnMouseLeftButtonDoubleClick(const Point &mousePos, MouseKey keyState)
{
    return false;
}

bool sw::WndBase::OnMouseRightButtonDown(const Point &mousePos, MouseKey keyState)
{
    return false;
}

bool sw::WndBase::OnMouseRightButtonUp(const Point &mousePos, MouseKey keyState)
{
    return false;
}

bool sw::WndBase::OnMouseRightButtonDoubleClick(const Point &mousePos, MouseKey keyState)
{
    return false;
}

bool sw::WndBase::OnMouseMiddleButtonDown(const Point &mousePos, MouseKey keyState)
{
    return false;
}

bool sw::WndBase::OnMouseMiddleButtonUp(const Point &mousePos, MouseKey keyState)
{
    return false;
}

bool sw::WndBase::OnMouseMiddleButtonDoubleClick(const Point &mousePos, MouseKey keyState)
{
    return false;
}

bool sw::WndBase::OnChar(wchar_t ch, const KeyFlags &flags)
{
    return false;
}

bool sw::WndBase::OnDeadChar(wchar_t ch, const KeyFlags &flags)
{
    return false;
}

bool sw::WndBase::OnKeyDown(VirtualKey key, const KeyFlags &flags)
{
    return false;
}

bool sw::WndBase::OnKeyUp(VirtualKey key, const KeyFlags &flags)
{
    return false;
}

bool sw::WndBase::OnSysChar(wchar_t ch, const KeyFlags &flags)
{
    return false;
}

bool sw::WndBase::OnSysDeadChar(wchar_t ch, const KeyFlags &flags)
{
    return false;
}

bool sw::WndBase::OnSysKeyDown(VirtualKey key, const KeyFlags &flags)
{
    return false;
}

bool sw::WndBase::OnSysKeyUp(VirtualKey key, const KeyFlags &flags)
{
    return false;
}

void sw::WndBase::VisibleChanged(bool newVisible)
{
}

bool sw::WndBase::SetParent(WndBase *parent)
{
    bool success;
    HWND hParent;

    if (parent == nullptr) {
        if (!this->_isControl) {
            hParent = NULL;
        } else {
            hParent = WndBase::_GetControlInitContainer()->_hwnd;
        }
    } else {
        hParent = parent->_hwnd;
    }

    this->SendMessageW(WM_PreSetParent, (WPARAM)hParent, 0);
    success = ::SetParent(this->_hwnd, hParent) != NULL;

    if (success) {
        this->ParentChanged(parent);
    }

    return success;
}

void sw::WndBase::ParentChanged(WndBase *newParent)
{
}

void sw::WndBase::OnCommand(int code)
{
}

void sw::WndBase::OnControlCommand(WndBase *pControl, int code, int id)
{
    pControl->OnCommand(code);
}

void sw::WndBase::OnMenuCommand(int id)
{
}

void sw::WndBase::OnAcceleratorCommand(int id)
{
}

void sw::WndBase::HandleInitialized(HWND hwnd)
{
}

void sw::WndBase::FontChanged(HFONT hfont)
{
}

bool sw::WndBase::OnSetCursor(HWND hwnd, HitTestResult hitTest, int message, bool &result)
{
    return false;
}

bool sw::WndBase::OnContextMenu(bool isKeyboardMsg, const Point &mousePosition)
{
    return false;
}

bool sw::WndBase::OnMenuSelect(HMENU hMenu, int id, int flags)
{
    return false;
}

bool sw::WndBase::OnNotify(NMHDR *pNMHDR, LRESULT &result)
{
    return false;
}

bool sw::WndBase::OnNotified(NMHDR *pNMHDR, LRESULT &result)
{
    return false;
}

bool sw::WndBase::OnVerticalScroll(int event, int pos)
{
    return false;
}

bool sw::WndBase::OnHorizontalScroll(int event, int pos)
{
    return false;
}

bool sw::WndBase::OnEnabledChanged(bool newValue)
{
    return false;
}

bool sw::WndBase::OnCtlColor(WndBase *pControl, HDC hdc, HBRUSH &hRetBrush)
{
    return pControl->OnColor(hdc, hRetBrush);
}

bool sw::WndBase::OnColor(HDC hdc, HBRUSH &hRetBrush)
{
    return false;
}

void sw::WndBase::OnNcHitTest(const Point &testPoint, HitTestResult &result)
{
}

bool sw::WndBase::OnEraseBackground(HDC hdc, LRESULT &result)
{
    return false;
}

bool sw::WndBase::OnDrawItem(int id, DRAWITEMSTRUCT *pDrawItem)
{
    return false;
}

bool sw::WndBase::OnDrawItemSelf(DRAWITEMSTRUCT *pDrawItem)
{
    return false;
}

bool sw::WndBase::OnMeasureItem(int id, MEASUREITEMSTRUCT *pMeasure)
{
    return false;
}

bool sw::WndBase::OnMeasureItemSelf(MEASUREITEMSTRUCT *pMeasure)
{
    return false;
}

bool sw::WndBase::OnDropFiles(HDROP hDrop)
{
    return false;
}

void sw::WndBase::Show(int nCmdShow)
{
    ShowWindow(this->_hwnd, nCmdShow);
}

void sw::WndBase::Close()
{
    this->SendMessageW(WM_CLOSE, 0, 0);
}

void sw::WndBase::Update()
{
    UpdateWindow(this->_hwnd);
}

void sw::WndBase::UpdateFont()
{
    if (this->_hfont != NULL) {
        DeleteObject(this->_hfont);
    }
    this->_hfont = this->_font.CreateHandle();
    this->SendMessageW(WM_SETFONT, (WPARAM)this->_hfont, TRUE);
    this->FontChanged(this->_hfont);
}

HFONT sw::WndBase::GetFontHandle()
{
    return this->_hfont;
}

void sw::WndBase::Redraw(bool erase, bool updateWindow)
{
    InvalidateRect(this->_hwnd, NULL, erase);
    if (updateWindow) UpdateWindow(this->_hwnd);
}

bool sw::WndBase::IsVisible() const
{
    return IsWindowVisible(this->_hwnd);
}

DWORD sw::WndBase::GetStyle() const
{
    return DWORD(GetWindowLongPtrW(this->_hwnd, GWL_STYLE));
}

void sw::WndBase::SetStyle(DWORD style)
{
    SetWindowLongPtrW(this->_hwnd, GWL_STYLE, LONG_PTR(style));
}

bool sw::WndBase::GetStyle(DWORD mask) const
{
    return (DWORD(GetWindowLongPtrW(this->_hwnd, GWL_STYLE)) & mask) == mask;
}

void sw::WndBase::SetStyle(DWORD mask, bool value)
{
    DWORD newstyle =
        value ? (DWORD(GetWindowLongPtrW(this->_hwnd, GWL_STYLE)) | mask)
              : (DWORD(GetWindowLongPtrW(this->_hwnd, GWL_STYLE)) & ~mask);
    SetWindowLongPtrW(this->_hwnd, GWL_STYLE, LONG_PTR(newstyle));
}

DWORD sw::WndBase::GetExtendedStyle() const
{
    return DWORD(GetWindowLongPtrW(this->_hwnd, GWL_EXSTYLE));
}

void sw::WndBase::SetExtendedStyle(DWORD style)
{
    SetWindowLongPtrW(this->_hwnd, GWL_EXSTYLE, LONG_PTR(style));
}

bool sw::WndBase::GetExtendedStyle(DWORD mask)
{
    return (DWORD(GetWindowLongPtrW(this->_hwnd, GWL_EXSTYLE)) & mask) == mask;
}

void sw::WndBase::SetExtendedStyle(DWORD mask, bool value)
{
    DWORD newstyle =
        value ? (DWORD(GetWindowLongPtrW(this->_hwnd, GWL_EXSTYLE)) | mask)
              : (DWORD(GetWindowLongPtrW(this->_hwnd, GWL_EXSTYLE)) & ~mask);
    SetWindowLongPtrW(this->_hwnd, GWL_EXSTYLE, LONG_PTR(newstyle));
}

sw::Point sw::WndBase::PointToScreen(const Point &point) const
{
    POINT p = point;
    ClientToScreen(this->_hwnd, &p);
    return p;
}

sw::Point sw::WndBase::PointFromScreen(const Point &screenPoint) const
{
    POINT p = screenPoint;
    ScreenToClient(this->_hwnd, &p);
    return p;
}

LRESULT sw::WndBase::SendMessageA(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return ::SendMessageA(this->_hwnd, uMsg, wParam, lParam);
}

LRESULT sw::WndBase::SendMessageW(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return ::SendMessageW(this->_hwnd, uMsg, wParam, lParam);
}

BOOL sw::WndBase::PostMessageA(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return ::PostMessageA(this->_hwnd, uMsg, wParam, lParam);
}

BOOL sw::WndBase::PostMessageW(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return ::PostMessageW(this->_hwnd, uMsg, wParam, lParam);
}

sw::HitTestResult sw::WndBase::NcHitTest(const Point &testPoint)
{
    POINT point = testPoint;
    return (HitTestResult)this->SendMessageW(WM_NCHITTEST, 0, MAKELPARAM(point.x, point.y));
}

void sw::WndBase::Invoke(const SimpleAction &action)
{
    if (action == nullptr)
        return;

    if (this->CheckAccess())
        action();
    else {
        Action<> &a = const_cast<Action<> &>(action); // safe here
        this->SendMessageW(WM_InvokeAction, false, reinterpret_cast<LPARAM>(&a));
    }
}

void sw::WndBase::InvokeAsync(const SimpleAction &action)
{
    if (action == nullptr)
        return;
    else {
        Action<> *p = new Action<>(action);
        this->PostMessageW(WM_InvokeAction, true, reinterpret_cast<LPARAM>(p));
    }
}

DWORD sw::WndBase::GetThreadId() const
{
    return GetWindowThreadProcessId(this->_hwnd, NULL);
}

bool sw::WndBase::CheckAccess() const
{
    return this->GetThreadId() == GetCurrentThreadId();
}

bool sw::WndBase::CheckAccess(const WndBase &other) const
{
    return this == &other || this->GetThreadId() == other.GetThreadId();
}

sw::WndBase *sw::WndBase::GetWndBase(HWND hwnd) noexcept
{
    // clang-format off
    static struct _InternalRaiiAtomHelper {
        ATOM value;
        _InternalRaiiAtomHelper() : value(GlobalAddAtomW(_WndBasePtrProp)) {}
        ~_InternalRaiiAtomHelper() { GlobalDeleteAtom(value); }
    } _atom;
    // clang-format on

    auto p = reinterpret_cast<WndBase *>(GetProp(hwnd, MAKEINTATOM(_atom.value)));
    return (p == nullptr || p->_check != _WndBaseMagicNumber) ? nullptr : p;
}

bool sw::WndBase::IsPtrValid(const WndBase *ptr) noexcept
{
    return ptr != nullptr && ptr->_check == _WndBaseMagicNumber;
}

LRESULT sw::WndBase::_WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    WndBase *pWnd = nullptr;

    if (hwnd != NULL) {
        pWnd = WndBase::GetWndBase(hwnd);
    }

    if (pWnd == nullptr && (uMsg == WM_NCCREATE || uMsg == WM_CREATE)) {
        auto temp = reinterpret_cast<WndBase *>(
            reinterpret_cast<LPCREATESTRUCTW>(lParam)->lpCreateParams);
        if (IsPtrValid(temp)) pWnd = temp;
    }

    if (pWnd != nullptr) {
        ProcMsg msg{hwnd, uMsg, wParam, lParam};
        return pWnd->WndProc(msg);
    }

    return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}

sw::WndBase *sw::WndBase::_GetControlInitContainer()
{
    static thread_local class _ThreadGuard
    {
    public:
        // 标记线程是否正在退出
        bool exitflag = false;

        // 控件初始容器
        std::unique_ptr<WndBase> container;

        // 线程退出时会调用析构函数
        ~_ThreadGuard()
        {
            this->exitflag = true;

            if (this->container != nullptr &&
                !this->container->_isDestroyed) //
            {
                // 线程退出时，容器窗口一般还未被销毁，此时消息循环已经结束，
                // 直接调用DestroyWindow无法销毁窗口，因此此处需要创建一个
                // 临时消息循环，发送WM_CLOSE以确保窗口正常销毁。
                this->container->PostMessageW(WM_CLOSE, 0, 0);

                // 临时消息循环
                for (MSG msg{}; GetMessageW(&msg, NULL, 0, 0);) {
                    TranslateMessage(&msg);
                    DispatchMessageW(&msg);
                }
            }
        }
    } _guard;

    class _ControlInitContainer : public WndBase
    {
    public:
        _ControlInitContainer()
        {
            this->InitWindow(L"", WS_POPUP, 0);
        }

        LRESULT WndProc(const ProcMsg &refMsg) override
        {
            switch (refMsg.uMsg) {
                case WM_CLOSE: {
                    DestroyWindow(this->_hwnd);
                    return 0;
                }
                case WM_NCDESTROY: {
                    if (_guard.exitflag) PostQuitMessage(0);
                    return this->WndBase::WndProc(refMsg);
                }
                default: {
                    return this->WndBase::WndProc(refMsg);
                }
            }
        }
    };

    if (_guard.container == nullptr ||
        _guard.container->_isDestroyed) {
        _guard.container = std::make_unique<_ControlInitContainer>();
    }
    return _guard.container.get();
}

int sw::WndBase::_NextControlId()
{
    return _controlIdCounter.fetch_add(1);
}

void sw::WndBase::_SetWndBase(HWND hwnd, WndBase &wnd)
{
    SetPropW(hwnd, _WndBasePtrProp, reinterpret_cast<HANDLE>(&wnd));
}
