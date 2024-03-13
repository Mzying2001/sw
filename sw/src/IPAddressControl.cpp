#include "IPAddressControl.h"

sw::IPAddressControl::IPAddressControl()
    : IsBlank(
          // get
          [&]() -> const bool & {
              static bool result;
              result = ::SendMessageW(this->_hIPAddrCtrl, IPM_ISBLANK, 0, 0);
              return result;
          }),

      Address(
          // get
          [&]() -> const uint32_t & {
              static uint32_t result;
              ::SendMessageW(this->_hIPAddrCtrl, IPM_GETADDRESS, 0, reinterpret_cast<LPARAM>(&result));
              return result;
          },
          // set
          [&](const uint32_t &value) {
              ::SendMessageW(this->_hIPAddrCtrl, IPM_SETADDRESS, 0, (LPARAM)value);
          })
{
    this->Rect    = sw::Rect{0, 0, 150, 24};
    this->TabStop = true;

    this->InitHwndHost();
    ::SendMessageW(this->_hIPAddrCtrl, WM_SETFONT, reinterpret_cast<WPARAM>(this->GetFontHandle()), FALSE);
}

void sw::IPAddressControl::Clear()
{
    ::SendMessageW(this->_hIPAddrCtrl, IPM_CLEARADDRESS, 0, 0);
}

bool sw::IPAddressControl::SetRange(int field, uint8_t min, uint8_t max)
{
    return ::SendMessageW(this->_hIPAddrCtrl, IPM_SETRANGE, field, MAKEIPRANGE(min, max));
}

HWND sw::IPAddressControl::BuildWindowCore(HWND hParent)
{
    RECT rect;
    GetClientRect(hParent, &rect);

    this->_hIPAddrCtrl = CreateWindowExW(0, WC_IPADDRESSW, L"", WS_CHILD | WS_VISIBLE,
                                         0, 0, rect.right - rect.left, rect.bottom - rect.top,
                                         hParent, NULL, App::Instance, NULL);

    return this->_hIPAddrCtrl;
}

void sw::IPAddressControl::DestroyWindowCore(HWND hwnd)
{
    DestroyWindow(this->_hIPAddrCtrl);
    this->_hIPAddrCtrl = NULL;
}

void sw::IPAddressControl::FontChanged(HFONT hfont)
{
    if (this->_hIPAddrCtrl != NULL)
        ::SendMessageW(this->_hIPAddrCtrl, WM_SETFONT, reinterpret_cast<WPARAM>(hfont), TRUE);
}

bool sw::IPAddressControl::OnSetFocus(HWND hPrevFocus)
{
    // SetFocus(this->_hIPAddrCtrl);
    ::SendMessageW(this->_hIPAddrCtrl, IPM_SETFOCUS, -1, 0);
    return this->HwndHost::OnSetFocus(hPrevFocus);
}
