#include "Control.h"

sw::Control::Control()
    : ControlId(
          // get
          [this]() -> int {
              return GetDlgCtrlID(this->_hwnd);
          })
{
}

sw::Control::~Control()
{
}

void sw::Control::ResetHandle(LPVOID lpParam)
{
    DWORD style   = this->GetStyle();
    DWORD exStyle = this->GetExtendedStyle();
    this->ResetHandle(style, exStyle, lpParam);
}

void sw::Control::ResetHandle(DWORD style, DWORD exStyle, LPVOID lpParam)
{
    RECT rect = this->Rect.Get();
    auto text = this->GetText().c_str();

    HWND oldHwnd = this->_hwnd;
    HWND hParent = GetParent(oldHwnd);

    wchar_t className[256];
    GetClassNameW(oldHwnd, className, 256);

    HMENU id = reinterpret_cast<HMENU>(
        static_cast<uintptr_t>(GetDlgCtrlID(oldHwnd)));

    HWND newHwnd = CreateWindowExW(
        exStyle,   // Optional window styles
        className, // Window class
        text,      // Window text
        style,     // Window style

        // Size and position
        rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,

        hParent,       // Parent window
        id,            // Control id
        App::Instance, // Instance handle
        lpParam        // Additional application data
    );

    LONG_PTR wndproc =
        SetWindowLongPtrW(oldHwnd, GWLP_WNDPROC, GetWindowLongPtrW(newHwnd, GWLP_WNDPROC));

    WndBase::_SetWndBase(newHwnd, *this);
    SetWindowLongPtrW(newHwnd, GWLP_WNDPROC, wndproc);

    this->_hwnd = newHwnd;
    DestroyWindow(oldHwnd);

    this->SendMessageW(WM_SETFONT, (WPARAM)this->GetFontHandle(), TRUE);
    this->UpdateSiblingsZOrder();
    this->HandleChenged();
}

void sw::Control::HandleChenged()
{
}
