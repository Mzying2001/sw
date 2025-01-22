#include "IPAddressControl.h"

sw::IPAddressControl::IPAddressControl()
    : IPAddressControl(sw::Size{150, 24})
{
}

sw::IPAddressControl::IPAddressControl(sw::Size size)
    : IsBlank(
          // get
          [this]() -> bool {
              return ::SendMessageW(this->_hIPAddrCtrl, IPM_ISBLANK, 0, 0);
          }),

      Address(
          // get
          [this]() -> uint32_t {
              uint32_t result;
              ::SendMessageW(this->_hIPAddrCtrl, IPM_GETADDRESS, 0, reinterpret_cast<LPARAM>(&result));
              return result;
          },
          // set
          [this](const uint32_t &value) {
              ::SendMessageW(this->_hIPAddrCtrl, IPM_SETADDRESS, 0, (LPARAM)value);
              this->OnAddressChanged();
          })
{
    this->Rect    = sw::Rect{0, 0, size.width, size.height};
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

bool sw::IPAddressControl::OnNotify(NMHDR *pNMHDR, LRESULT &result)
{
    if (pNMHDR->code == IPN_FIELDCHANGED) {
        this->OnAddressChanged();
    }
    return this->HwndHost::OnNotify(pNMHDR, result);
}

void sw::IPAddressControl::OnAddressChanged()
{
    this->RaiseRoutedEvent(IPAddressControl_AddressChanged);
}
