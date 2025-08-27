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
              return this->_borderStyle;
          },
          // set
          [this](const sw::BorderStyle &value) {
              if (this->_borderStyle != value) {
                  this->_borderStyle = value;
                  this->UpdateBorder();
              }
          }),

      Padding(
          // get
          [this]() -> sw::Thickness {
              return this->_padding;
          },
          // set
          [this](const sw::Thickness &value) {
              if (this->_padding != value) {
                  this->_padding = value;
                  this->UpdateBorder();
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

    this->InitControl(_PanelClassName, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, WS_EX_NOACTIVATE);
    this->Rect             = sw::Rect(0, 0, 200, 200);
    this->Transparent      = true;
    this->InheritTextColor = true;
}

void sw::Panel::UpdateBorder()
{
    SetWindowPos(this->Handle, nullptr, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
}

LRESULT sw::Panel::WndProc(const ProcMsg &refMsg)
{
    switch (refMsg.uMsg) {
        case WM_NCCALCSIZE: {
            auto result = this->DefaultWndProc(refMsg);
            RECT *pRect = refMsg.wParam == FALSE
                              ? reinterpret_cast<RECT *>(refMsg.lParam)
                              : reinterpret_cast<NCCALCSIZE_PARAMS *>(refMsg.lParam)->rgrc;
            this->OnDrawBorder(NULL, *pRect);
            this->OnDrawPadding(NULL, *pRect);
            return result;
        }

        case WM_UpdateLayout: {
            if (this->IsInHierarchy)
                this->UpdateLayout();
            return 0;
        }

        default: {
            return this->WndBase::WndProc(refMsg);
        }
    }
}

bool sw::Panel::OnPaint()
{
    PAINTSTRUCT ps;
    HWND hwnd = this->Handle;
    HDC hdc   = BeginPaint(hwnd, &ps);

    RECT clientRect;
    GetClientRect(hwnd, &clientRect);

    HBRUSH hBrush = CreateSolidBrush(this->GetRealBackColor());
    FillRect(hdc, &clientRect, hBrush);

    DeleteObject(hBrush);
    EndPaint(hwnd, &ps);
    return true;
}

void sw::Panel::OnEndNcPaint()
{
    HWND hwnd = this->Handle;
    HDC hdc   = GetWindowDC(hwnd);

    RECT rect;
    GetWindowRect(hwnd, &rect);

    rect.right -= rect.left;
    rect.bottom -= rect.top;
    rect.left = 0;
    rect.top  = 0;

    this->OnDrawBorder(hdc, rect);
    this->OnDrawPadding(hdc, rect);

    ReleaseDC(hwnd, hdc);
    return;
}

void sw::Panel::OnDrawBorder(HDC hdc, RECT &rect)
{
    if (this->_borderStyle == sw::BorderStyle::None) {
        return;
    }

    if (hdc != NULL) {
        DrawEdge(hdc, &rect, (UINT)this->_borderStyle, BF_RECT);
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
    if (this->_padding.left == 0 && this->_padding.top == 0 &&
        this->_padding.right == 0 && this->_padding.bottom == 0) {
        return;
    }

    RECT rtPaddingOuter  = rect;
    RECT &rtPaddingInner = rect;

    rtPaddingInner.left += Dip::DipToPxX(this->_padding.left);
    rtPaddingInner.top += Dip::DipToPxY(this->_padding.top);
    rtPaddingInner.right -= Dip::DipToPxX(this->_padding.right);
    rtPaddingInner.bottom -= Dip::DipToPxY(this->_padding.bottom);

    rtPaddingInner.right  = Utils::Max(rtPaddingInner.left, rtPaddingInner.right);
    rtPaddingInner.bottom = Utils::Max(rtPaddingInner.top, rtPaddingInner.bottom);

    if (hdc != NULL) {
        HRGN hRgnOuter = CreateRectRgnIndirect(&rtPaddingOuter);
        HRGN hRgnInner = CreateRectRgnIndirect(&rtPaddingInner);
        HRGN hRgnDiff  = CreateRectRgn(0, 0, 0, 0);
        CombineRgn(hRgnDiff, hRgnOuter, hRgnInner, RGN_DIFF);

        HBRUSH hBrush = CreateSolidBrush(this->GetRealBackColor());
        FillRgn(hdc, hRgnDiff, hBrush);

        DeleteObject(hRgnOuter);
        DeleteObject(hRgnInner);
        DeleteObject(hRgnDiff);
        DeleteObject(hBrush);
    }
}
