#include "Control.h"

sw::Control::Control()
{
}

sw::Control::~Control()
{
}

void sw::Control::ResetHandle()
{
    DWORD style   = this->GetStyle();
    DWORD exStyle = this->GetExtendedStyle();
    this->ResetHandle(style, exStyle);
}

void sw::Control::ResetHandle(DWORD style, DWORD exStyle)
{
    HWND &refHwnd = this->_hwnd;

    RECT rect = this->Rect.Get();
    auto text = this->GetText().c_str();

    HWND oldHwnd = refHwnd;
    HWND hParent = GetParent(oldHwnd);

    wchar_t className[256];
    GetClassNameW(oldHwnd, className, 256);

    HWND newHwnd = CreateWindowExW(
        exStyle,   // Optional window styles
        className, // Window class
        text,      // Window text
        style,     // Window style

        // Size and position
        rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,

        hParent,        // Parent window
        NULL,           // Menu
        App::Instance,  // Instance handle
        (WndBase *)this // Additional application data
    );

    LONG_PTR wndproc =
        SetWindowLongPtrW(oldHwnd, GWLP_WNDPROC, GetWindowLongPtrW(newHwnd, GWLP_WNDPROC));
    SetWindowLongPtrW(oldHwnd, GWLP_USERDATA, (LONG_PTR)NULL);

    SetWindowLongPtrW(newHwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>((WndBase *)this));
    SetWindowLongPtrW(newHwnd, GWLP_WNDPROC, wndproc);

    refHwnd = newHwnd;
    DestroyWindow(oldHwnd);

    this->SendMessageW(WM_SETFONT, (WPARAM)this->GetFontHandle(), TRUE);
    this->UpdateSiblingsZOrder();
    this->HandleChenged();
}

void sw::Control::HandleChenged()
{
}
