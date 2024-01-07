#include "Control.h"

sw::Control::Control()
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

bool sw::Control::OnSetCursor(HWND hwnd, HitTestResult hitTest, int message, bool &result)
{
    if (this->_useDefaultCursor) {
        return false;
    }
    ::SetCursor(this->_hCursor);
    result = true;
    return true;
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
