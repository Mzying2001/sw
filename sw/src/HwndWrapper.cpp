#include "HwndWrapper.h"

void sw::HwndWrapper::InitHwndWrapper()
{
    if (this->_hwnd != NULL) {
        return;
    }

    bool isControl = false;

    this->_hwnd = this->BuildWindowCore(
        isControl, WndBase::_NextControlId());

    this->_isControl = isControl;
    WndBase::_SetWndBase(this->_hwnd, *this);

    this->_originalWndProc = reinterpret_cast<WNDPROC>(
        SetWindowLongPtrW(this->_hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndBase::_WndProc)));

    RECT rect;
    GetWindowRect(this->_hwnd, &rect);
    this->_rect = rect;

    this->UpdateText();
    this->HandleInitialized(this->_hwnd);
    this->UpdateFont();

    if (this->_isControl) {
        WndBase::_InitControlContainer();
        this->SetParent(nullptr);
    }
}
