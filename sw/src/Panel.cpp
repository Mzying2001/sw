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
          Property<sw::BorderStyle>::Init(this)
              .Getter([](Panel *self) -> sw::BorderStyle {
                  return self->_borderStyle;
              })
              .Setter([](Panel *self, sw::BorderStyle value) {
                  if (self->_borderStyle != value) {
                      self->_borderStyle = value;
                      self->UpdateBorder();
                  }
              })),

      Padding(
          Property<sw::Thickness>::Init(this)
              .Getter([](Panel *self) -> sw::Thickness {
                  return self->_padding;
              })
              .Setter([](Panel *self, const sw::Thickness &value) {
                  if (self->_padding != value) {
                      self->_padding = value;
                      self->UpdateBorder();
                  }
              }))
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

    Rect             = sw::Rect{0, 0, 200, 200};
    Transparent      = true;
    InheritTextColor = true;
}

void sw::Panel::UpdateBorder()
{
    SetWindowPos(Handle, nullptr, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
}

LRESULT sw::Panel::WndProc(ProcMsg &refMsg)
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

bool sw::Panel::OnEraseBackground(HDC hdc, LRESULT &result)
{
    result = 1;
    return true;
}

bool sw::Panel::OnPaint()
{
    PAINTSTRUCT ps;
    HWND hwnd = Handle;
    HDC hdc   = BeginPaint(hwnd, &ps);

    HBRUSH hBrush = CreateSolidBrush(GetRealBackColor());
    FillRect(hdc, &ps.rcPaint, hBrush);

    DeleteObject(hBrush);
    EndPaint(hwnd, &ps);
    return true;
}

bool sw::Panel::OnNcPaint(HRGN hRgn)
{
    HWND hwnd = Handle;
    HDC hdc   = GetWindowDC(hwnd);

    RECT rect;
    GetWindowRect(hwnd, &rect);
    OffsetRect(&rect, -rect.left, -rect.top);

    OnDrawBorder(hdc, rect);
    OnDrawPadding(hdc, rect);
    ReleaseDC(hwnd, hdc);

    DefaultWndProc(ProcMsg{
        hwnd, WM_NCPAINT, reinterpret_cast<WPARAM>(hRgn), 0}); // scrollbars
    return true;
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
