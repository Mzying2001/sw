#include "Control.h"

sw::Control::Control()
    : ControlId(
          // get
          [this]() -> int {
              return GetDlgCtrlID(this->_hwnd);
          }),

      IsInHierarchy(
          // get
          [this]() -> bool {
              if (this->_hwnd == NULL || this->_isDestroyed) {
                  return false;
              }
              auto container = WndBase::_GetControlInitContainer();
              return container == nullptr || GetParent(this->_hwnd) != container->_hwnd;
          })
{
}

sw::Control::~Control()
{
}

sw::Control *sw::Control::ToControl()
{
    return this;
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
    auto text = this->GetInternalText().c_str();

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
    this->OnHandleChenged(this->_hwnd);
}

bool sw::Control::OnNotified(NMHDR *pNMHDR, LRESULT &result)
{
    switch (pNMHDR->code) {
        case NM_CUSTOMDRAW: {
            return this->OnCustomDraw(reinterpret_cast<NMCUSTOMDRAW *>(pNMHDR), result);
        }
        default: {
            return this->UIElement::OnNotified(pNMHDR, result);
        }
    }
}

bool sw::Control::OnKillFocus(HWND hNextFocus)
{
    this->_drawFocusRect = false;
    return this->UIElement::OnKillFocus(hNextFocus);
}

void sw::Control::OnTabStop()
{
    this->_drawFocusRect = true;
    this->UIElement::OnTabStop();
}

void sw::Control::OnEndPaint()
{
    if (!this->_hasCustomDraw && this->_drawFocusRect) {
        HDC hdc = GetDC(this->_hwnd);
        this->OnDrawFocusRect(hdc);
        ReleaseDC(this->_hwnd, hdc);
    }
}

bool sw::Control::OnCustomDraw(NMCUSTOMDRAW *pNMCD, LRESULT &result)
{
    this->_hasCustomDraw = true;

    switch (pNMCD->dwDrawStage) {
        case CDDS_PREERASE: {
            return this->OnPreErase(pNMCD->hdc, result);
        }
        case CDDS_POSTERASE: {
            return this->OnPostErase(pNMCD->hdc, result);
        }
        case CDDS_PREPAINT: {
            return this->OnPrePaint(pNMCD->hdc, result);
        }
        case CDDS_POSTPAINT: {
            return this->OnPostPaint(pNMCD->hdc, result);
        }
        default: {
            return false;
        }
    }
}

bool sw::Control::OnPreErase(HDC hdc, LRESULT &result)
{
    result = CDRF_NOTIFYPOSTERASE;
    return true;
}

bool sw::Control::OnPostErase(HDC hdc, LRESULT &result)
{
    return false;
}

bool sw::Control::OnPrePaint(HDC hdc, LRESULT &result)
{
    result = CDRF_NOTIFYPOSTPAINT;
    return true;
}

bool sw::Control::OnPostPaint(HDC hdc, LRESULT &result)
{
    if (this->_drawFocusRect) {
        this->OnDrawFocusRect(hdc);
    }
    return false;
}

void sw::Control::OnDrawFocusRect(HDC hdc)
{
    // RECT rect = this->ClientRect;
    RECT rect;
    GetClientRect(this->_hwnd, &rect);
    DrawFocusRect(hdc, &rect);
}

void sw::Control::OnHandleChenged(HWND hwnd)
{
}
