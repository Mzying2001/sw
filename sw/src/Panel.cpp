#include "Panel.h"
#include "Utils.h"

namespace
{
    /**
     * @brief 面板的窗口类名
     */
    constexpr wchar_t _PanelClassName[] = L"sw::Panel";
}

sw::Panel::Panel()
    : BorderStyle(
          // get
          [this]() -> sw::BorderStyle {
              return _borderStyle;
          },
          // set
          [this](const sw::BorderStyle &value) {
              if (_borderStyle != value) {
                  _borderStyle = value;
                  UpdateBorder();
              }
          }),

      Padding(
          // get
          [this]() -> sw::Thickness {
              return _padding;
          },
          // set
          [this](const sw::Thickness &value) {
              if (_padding != value) {
                  _padding = value;
                  UpdateBorder();
              }
          })
{
    static thread_local ATOM panelClsAtom = 0;

    if (panelClsAtom == 0) {
        WNDCLASSEXW wc{};
        wc.cbSize        = sizeof(wc);
        wc.hInstance     = App::Instance;
        wc.lpfnWndProc   = DefWindowProcW;
        wc.lpszClassName = _PanelClassName;
        wc.hCursor       = CursorHelper::GetCursorHandle(StandardCursor::Arrow);
        panelClsAtom     = RegisterClassExW(&wc);
    }

    InitControl(_PanelClassName, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, WS_EX_NOACTIVATE);
    Rect             = sw::Rect(0, 0, 200, 200);
    Transparent      = true;
    InheritTextColor = true;
}

void sw::Panel::UpdateBorder()
{
    SetWindowPos(Handle, nullptr, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
}

LRESULT sw::Panel::WndProc(const ProcMsg &refMsg)
{
    switch (refMsg.uMsg) {
        case WM_NCCALCSIZE: {
            auto result = DefaultWndProc(refMsg);
            RECT *pRect = nullptr;

            if (refMsg.wParam == FALSE) {
                pRect = reinterpret_cast<RECT *>(refMsg.lParam);
            } else {
                pRect = reinterpret_cast<NCCALCSIZE_PARAMS *>(refMsg.lParam)->rgrc;
            }
            OnDrawBorder(NULL, *pRect);
            OnDrawPadding(NULL, *pRect);
            return result;
        }

        case WM_UpdateLayout: {
            if (IsInHierarchy)
                UpdateLayout();
            return 0;
        }

        default: {
            return WndBase::WndProc(refMsg);
        }
    }
}

bool sw::Panel::OnPaint()
{
    PAINTSTRUCT ps;
    HWND hwnd = Handle;
    HDC hdc   = BeginPaint(hwnd, &ps);

    RECT clientRect;
    GetClientRect(hwnd, &clientRect);

    HBRUSH hBrush = CreateSolidBrush(GetRealBackColor());
    FillRect(hdc, &clientRect, hBrush);

    DeleteObject(hBrush);
    EndPaint(hwnd, &ps);
    return true;
}

void sw::Panel::OnEndNcPaint()
{
    HWND hwnd = Handle;
    HDC hdc   = GetWindowDC(hwnd);

    RECT rect;
    GetWindowRect(hwnd, &rect);

    rect.right -= rect.left;
    rect.bottom -= rect.top;
    rect.left = 0;
    rect.top  = 0;

    OnDrawBorder(hdc, rect);
    OnDrawPadding(hdc, rect);

    ReleaseDC(hwnd, hdc);
    return;
}

void sw::Panel::OnDrawBorder(HDC hdc, RECT &rect)
{
    if (_borderStyle == sw::BorderStyle::None) {
        return;
    }

    if (hdc != NULL) {
        DrawEdge(hdc, &rect, (UINT)_borderStyle, BF_RECT);
    }

    int cx = GetSystemMetrics(SM_CXEDGE);
    int cy = GetSystemMetrics(SM_CYEDGE);

    rect.left += cx;
    rect.top += cy;
    rect.right -= cx;
    rect.bottom -= cy;

    rect.right  = Utils::Max(rect.left, rect.right);
    rect.bottom = Utils::Max(rect.top, rect.bottom);
}

void sw::Panel::OnDrawPadding(HDC hdc, RECT &rect)
{
    if (_padding.left == 0 && _padding.top == 0 &&
        _padding.right == 0 && _padding.bottom == 0) {
        return;
    }

    RECT rtPaddingOuter  = rect;
    RECT &rtPaddingInner = rect;

    rtPaddingInner.left += Dip::DipToPxX(_padding.left);
    rtPaddingInner.top += Dip::DipToPxY(_padding.top);
    rtPaddingInner.right -= Dip::DipToPxX(_padding.right);
    rtPaddingInner.bottom -= Dip::DipToPxY(_padding.bottom);

    rtPaddingInner.right  = Utils::Max(rtPaddingInner.left, rtPaddingInner.right);
    rtPaddingInner.bottom = Utils::Max(rtPaddingInner.top, rtPaddingInner.bottom);

    if (hdc != NULL) {
        HRGN hRgnOuter = CreateRectRgnIndirect(&rtPaddingOuter);
        HRGN hRgnInner = CreateRectRgnIndirect(&rtPaddingInner);
        HRGN hRgnDiff  = CreateRectRgn(0, 0, 0, 0);
        CombineRgn(hRgnDiff, hRgnOuter, hRgnInner, RGN_DIFF);

        HBRUSH hBrush = CreateSolidBrush(GetRealBackColor());
        FillRgn(hdc, hRgnDiff, hBrush);

        DeleteObject(hRgnOuter);
        DeleteObject(hRgnInner);
        DeleteObject(hRgnDiff);
        DeleteObject(hBrush);
    }
}
