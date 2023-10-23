#include "Control.h"

sw::Control::Control()
    : BackColor(
          // get
          [&]() -> const Color & {
              return this->_backColor;
          },
          // set
          [&](const Color &value) {
              this->SetBackColor(value, true);
          }),

      TextColor(
          // get
          [&]() -> const Color & {
              return this->_textColor;
          },
          // set
          [&](const Color &value) {
              this->SetTextColor(value, true);
          })
{
}

void sw::Control::ResetHandle()
{
    HWND &refHwnd = const_cast<HWND &>(this->Handle.Get());

    HWND oldHwnd       = refHwnd;
    HWND parent        = GetParent(oldHwnd);
    DWORD style        = (DWORD)this->GetStyle();
    DWORD exStyle      = (DWORD)this->GetExtendedStyle();
    RECT rect          = this->Rect.Get();
    std::wstring &text = this->GetText();

    wchar_t className[256];
    GetClassNameW(oldHwnd, className, 256);

    refHwnd = CreateWindowExW(
        exStyle,      // Optional window styles
        className,    // Window class
        text.c_str(), // Window text
        style,        // Window style

        // Size and position
        rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,

        parent,        // Parent window
        NULL,          // Menu
        App::Instance, // Instance handle
        this           // Additional application data
    );

    SetWindowLongPtrW(oldHwnd, GWLP_USERDATA, (LONG_PTR)NULL);
    SetWindowLongPtrW(refHwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>((WndBase *)this));

    LONG_PTR wndProc = GetWindowLongPtrW(oldHwnd, GWLP_WNDPROC);
    SetWindowLongPtrW(refHwnd, GWLP_WNDPROC, wndProc);

    this->SendMessageW(WM_SETFONT, (WPARAM)this->GetFontHandle(), TRUE);
    this->HandleChenged();
    this->UpdateSiblingsZOrder();

    DestroyWindow(oldHwnd);
}

void sw::Control::HandleChenged()
{
}

void sw::Control::SetBackColor(Color color, bool redraw)
{
    this->_backColor = color;
    if (redraw) this->Redraw();
}

void sw::Control::SetTextColor(Color color, bool redraw)
{
    this->_textColor = color;
    if (redraw) this->Redraw();
}

bool sw::Control::OnSetCursor(HWND hwnd, int hitTest, int message, bool &useDefaultWndProc)
{
    if (this->_useDefaultCursor) {
        return false;
    }
    ::SetCursor(this->_hCursor);
    useDefaultWndProc = false;
    return true;
}

LRESULT sw::Control::CtlColor(HDC hdc, HWND hwnd)
{
    static HBRUSH hBrush = NULL;

    if (hBrush != NULL) {
        DeleteObject(hBrush);
    }

    hBrush = CreateSolidBrush(this->_backColor);

    ::SetTextColor(hdc, this->_textColor);
    ::SetBkColor(hdc, this->_backColor);
    return (LRESULT)hBrush;
}

void sw::Control::SetCursor(HCURSOR hCursor)
{
    this->_hCursor          = hCursor;
    this->_useDefaultCursor = false;
}

void sw::Control::SetCursor(StandardCursor cursor)
{
    this->SetCursor(CursorHelper::GetCursorHandle(cursor));
}

void sw::Control::ResetCursor()
{
    this->_hCursor          = NULL;
    this->_useDefaultCursor = true;
}
