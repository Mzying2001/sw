#include "Control.h"

sw::Control::Control()
    : ControlId(
          // get
          [this]() -> int {
              return GetDlgCtrlID(_hwnd);
          }),

      IsInHierarchy(
          // get
          [this]() -> bool {
              if (_hwnd == NULL || _isDestroyed) {
                  return false;
              }
              auto container = WndBase::_GetControlInitContainer();
              return container == nullptr || GetParent(_hwnd) != container->_hwnd;
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

bool sw::Control::ResetHandle(LPVOID lpParam)
{
    DWORD style   = GetStyle();
    DWORD exStyle = GetExtendedStyle();
    return ResetHandle(style, exStyle, lpParam);
}

bool sw::Control::ResetHandle(DWORD style, DWORD exStyle, LPVOID lpParam)
{
    RECT rect = Rect.Get();
    auto text = GetInternalText().c_str();

    HWND oldHwnd = _hwnd;
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

    _hwnd = newHwnd;
    DestroyWindow(oldHwnd);

    SendMessageW(WM_SETFONT, (WPARAM)GetFontHandle(), TRUE);
    UpdateSiblingsZOrder();
    OnHandleChenged(_hwnd);
    return true;
}

bool sw::Control::OnNotified(NMHDR *pNMHDR, LRESULT &result)
{
    switch (pNMHDR->code) {
        case NM_CUSTOMDRAW: {
            return OnCustomDraw(reinterpret_cast<NMCUSTOMDRAW *>(pNMHDR), result);
        }
        default: {
            return UIElement::OnNotified(pNMHDR, result);
        }
    }
}

bool sw::Control::OnKillFocus(HWND hNextFocus)
{
    _drawFocusRect = false;
    return UIElement::OnKillFocus(hNextFocus);
}

void sw::Control::OnTabStop()
{
    _drawFocusRect = true;
    UIElement::OnTabStop();
}

void sw::Control::OnEndPaint()
{
    if (!_hasCustomDraw && _drawFocusRect) {
        HDC hdc = GetDC(_hwnd);
        OnDrawFocusRect(hdc);
        ReleaseDC(_hwnd, hdc);
    }
}

bool sw::Control::OnCustomDraw(NMCUSTOMDRAW *pNMCD, LRESULT &result)
{
    _hasCustomDraw = true;

    switch (pNMCD->dwDrawStage) {
        case CDDS_PREERASE: {
            return OnPreErase(pNMCD->hdc, result);
        }
        case CDDS_POSTERASE: {
            return OnPostErase(pNMCD->hdc, result);
        }
        case CDDS_PREPAINT: {
            return OnPrePaint(pNMCD->hdc, result);
        }
        case CDDS_POSTPAINT: {
            return OnPostPaint(pNMCD->hdc, result);
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
    if (_drawFocusRect) {
        OnDrawFocusRect(hdc);
    }
    return false;
}

void sw::Control::OnDrawFocusRect(HDC hdc)
{
    // RECT rect = ClientRect;
    RECT rect;
    GetClientRect(_hwnd, &rect);
    DrawFocusRect(hdc, &rect);
}

void sw::Control::OnHandleChenged(HWND hwnd)
{
}
