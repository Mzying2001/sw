#include "Panel.h"

/**
 * @brief 面板的窗口类名
 */
static constexpr wchar_t _PanelClassName[] = L"sw::Panel";

sw::Panel::Panel()
    : BorderStyle(
          // get
          [&]() -> const sw::BorderStyle & {
              return this->_borderStyle;
          },
          // set
          [&](const sw::BorderStyle &value) {
              if (this->_borderStyle != value) {
                  this->_borderStyle = value;
                  this->Redraw();
              }
          })
{
    static WNDCLASSEXW wc = {0};

    if (wc.cbSize == 0) {
        wc.cbSize        = sizeof(wc);
        wc.hInstance     = App::Instance;
        wc.lpfnWndProc   = DefWindowProcW;
        wc.lpszClassName = _PanelClassName;
        RegisterClassExW(&wc);
    }

    this->InitControl(_PanelClassName, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, WS_EX_NOACTIVATE);
    this->Rect        = sw::Rect(0, 0, 200, 200);
    this->Transparent = true;
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

    if (this->_borderStyle != sw::BorderStyle::None)
        DrawEdge(hdc, &clientRect, (UINT)this->_borderStyle, BF_RECT);

    DeleteObject(hBrush);
    EndPaint(hwnd, &ps);
    return true;
}

bool sw::Panel::OnSize(Size newClientSize)
{
    InvalidateRect(this->Handle, NULL, TRUE);
    return UIElement::OnSize(newClientSize);
}

bool sw::Panel::OnMouseMove(Point mousePosition, MouseKey keyState)
{
    HWND hwnd = this->Handle;
    this->SendMessageW(WM_SETCURSOR, (WPARAM)hwnd, MAKELONG(HTCLIENT, WM_MOUSEMOVE));
    return UIElement::OnMouseMove(mousePosition, keyState);
}
