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
                  this->_UpdateBorder();
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
                  this->_UpdateBorder();
              }
          })
{
    static WNDCLASSEXW wc = {0};

    if (wc.cbSize == 0) {
        wc.cbSize        = sizeof(wc);
        wc.hInstance     = App::Instance;
        wc.lpfnWndProc   = DefWindowProcW;
        wc.lpszClassName = _PanelClassName;
        wc.hCursor       = CursorHelper::GetCursorHandle(StandardCursor::Arrow);
        RegisterClassExW(&wc);
    }

    this->InitControl(_PanelClassName, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, WS_EX_NOACTIVATE);
    this->Rect             = sw::Rect(0, 0, 200, 200);
    this->Transparent      = true;
    this->InheritTextColor = true;
}

LRESULT sw::Panel::WndProc(const ProcMsg &refMsg)
{
    switch (refMsg.uMsg) {
        case WM_NCCALCSIZE: {
            auto result = this->DefaultWndProc(refMsg);
            RECT *pRect = refMsg.wParam == FALSE
                              ? reinterpret_cast<RECT *>(refMsg.lParam)
                              : reinterpret_cast<NCCALCSIZE_PARAMS *>(refMsg.lParam)->rgrc;
            this->_MinusBorderThickness(*pRect);
            this->_MinusPadding(*pRect);
            return result;
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

    if (this->_borderStyle != sw::BorderStyle::None) {
        DrawEdge(hdc, &rect, (UINT)this->_borderStyle, BF_RECT);
    }

    RECT rtPaddingOuter = rect;
    this->_MinusBorderThickness(rtPaddingOuter);

    RECT rtPaddingInner = rtPaddingOuter;
    this->_MinusPadding(rtPaddingInner);

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
    ReleaseDC(hwnd, hdc);
    return;
}

void sw::Panel::_UpdateBorder()
{
    SetWindowPos(this->Handle, nullptr, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
}

void sw::Panel::_MinusBorderThickness(RECT &rect)
{
    switch (this->_borderStyle) {
        case sw::BorderStyle::None: {
            break;
        }
        default: {
            rect.left += 2;
            rect.top += 2;
            rect.right -= 2;
            rect.bottom -= 2;
            break;
        }
    }
    rect.right  = Utils::Max(rect.left, rect.right);
    rect.bottom = Utils::Max(rect.top, rect.bottom);
}

void sw::Panel::_MinusPadding(RECT &rect)
{
    rect.left += Dip::DipToPxX(this->_padding.left);
    rect.top += Dip::DipToPxY(this->_padding.top);
    rect.right -= Dip::DipToPxX(this->_padding.right);
    rect.bottom -= Dip::DipToPxY(this->_padding.bottom);

    rect.right  = Utils::Max(rect.left, rect.right);
    rect.bottom = Utils::Max(rect.top, rect.bottom);
}
