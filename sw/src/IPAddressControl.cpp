#include "IPAddressControl.h"

sw::IPAddressControl::IPAddressControl()
    : IsBlank(
          // get
          [this]() -> bool {
              return ::SendMessageW(_hIPAddrCtrl, IPM_ISBLANK, 0, 0);
          }),

      Address(
          // get
          [this]() -> uint32_t {
              uint32_t result;
              ::SendMessageW(_hIPAddrCtrl, IPM_GETADDRESS, 0, reinterpret_cast<LPARAM>(&result));
              return result;
          },
          // set
          [this](const uint32_t &value) {
              ::SendMessageW(_hIPAddrCtrl, IPM_SETADDRESS, 0, (LPARAM)value);
              OnAddressChanged();
          })
{
    Rect    = sw::Rect{0, 0, 150, 24};
    TabStop = true;

    InitHwndHost();
}

void sw::IPAddressControl::Clear()
{
    ::SendMessageW(_hIPAddrCtrl, IPM_CLEARADDRESS, 0, 0);
}

bool sw::IPAddressControl::SetRange(int field, uint8_t min, uint8_t max)
{
    return ::SendMessageW(_hIPAddrCtrl, IPM_SETRANGE, field, MAKEIPRANGE(min, max));
}

HWND sw::IPAddressControl::BuildWindowCore(HWND hParent)
{
    RECT rect;
    GetClientRect(hParent, &rect);

    _hIPAddrCtrl = CreateWindowExW(
        0, WC_IPADDRESSW, L"", WS_CHILD | WS_VISIBLE,
        0, 0, rect.right - rect.left, rect.bottom - rect.top,
        hParent, NULL, App::Instance, NULL);

    return _hIPAddrCtrl;
}

void sw::IPAddressControl::DestroyWindowCore(HWND hwnd)
{
    DestroyWindow(_hIPAddrCtrl);
    _hIPAddrCtrl = NULL;
}

bool sw::IPAddressControl::OnSetFocus(HWND hPrevFocus)
{
    ::SendMessageW(_hIPAddrCtrl, IPM_SETFOCUS, -1, 0);
    return TBase::OnSetFocus(hPrevFocus);
}

bool sw::IPAddressControl::OnNotify(NMHDR *pNMHDR, LRESULT &result)
{
    if (pNMHDR->code == IPN_FIELDCHANGED) {
        OnAddressChanged();
    }
    return TBase::OnNotify(pNMHDR, result);
}

void sw::IPAddressControl::OnAddressChanged()
{
    RaiseRoutedEvent(IPAddressControl_AddressChanged);
}
