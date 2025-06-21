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
    if (refMsg.uMsg != WM_NCCALCSIZE) {
        return this->WndBase::WndProc(refMsg);
    }

    auto result = this->DefaultWndProc(refMsg);

    if (refMsg.wParam == FALSE) {
        auto pRect = reinterpret_cast<RECT *>(refMsg.lParam);
        this->_MinusBorderThickness(*pRect);
    } else {
        auto pNCCSP = reinterpret_cast<NCCALCSIZE_PARAMS *>(refMsg.lParam);
        this->_MinusBorderThickness(pNCCSP->rgrc[0]);
    }
    return result;
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
    if (this->_borderStyle == sw::BorderStyle::None) {
        return;
    }

    HWND hwnd = this->Handle;
    HDC hdc   = GetWindowDC(hwnd);

    RECT rect;
    GetWindowRect(hwnd, &rect);

    rect.right -= rect.left;
    rect.bottom -= rect.top;
    rect.left = 0;
    rect.top  = 0;

    DrawEdge(hdc, &rect, (UINT)this->_borderStyle, BF_RECT);
    ReleaseDC(hwnd, hdc);
    return;
}

void sw::Panel::_UpdateBorder()
{
    SetWindowPos(this->Handle, nullptr, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
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
