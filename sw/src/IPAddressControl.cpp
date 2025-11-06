#include "IPAddressControl.h"

namespace
{
    /**
     * @brief 内部编辑框子类化时保存原始窗口过程函数的属性名称
     */
    constexpr wchar_t _FieldsEditOriginalProc[] = L"SWPROP_FieldsEditOriginalProc";

    /**
     * @brief 保存IPAddressControl指针的属性名称
     */
    constexpr wchar_t _IPAddressControlPtr[] = L"SWPROP_IPAddressControlPtr";
}

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

    HWND hChild = GetWindow(_hIPAddrCtrl, GW_CHILD);

    while (hChild != NULL) {
        SetPropW(hChild, _IPAddressControlPtr, reinterpret_cast<HANDLE>(this));
        SetPropW(hChild, _FieldsEditOriginalProc, reinterpret_cast<HANDLE>(GetWindowLongPtrW(hChild, GWLP_WNDPROC)));
        SetWindowLongPtrW(hChild, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(_FieldsEditSubclassProc));
        hChild = GetWindow(hChild, GW_HWNDNEXT);
    }

    return _hIPAddrCtrl;
}

void sw::IPAddressControl::DestroyWindowCore(HWND hwnd)
{
    DestroyWindow(_hIPAddrCtrl);
    _hIPAddrCtrl = NULL;
}

bool sw::IPAddressControl::OnSize(const Size &newClientSize)
{
    auto result = TBase::OnSize(newClientSize);

    // SysIPAddress32尺寸改变时不会自动调整内部编辑框的位置和尺寸，
    // 但在字体改变时会进行调整，因此发送WM_SETFONT以调整内部编辑框
    ::SendMessageW(_hIPAddrCtrl, WM_SETFONT, reinterpret_cast<WPARAM>(GetFontHandle()), TRUE);
    return result;
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

void sw::IPAddressControl::_OnTabKeyDown()
{
    bool shiftDown = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
    OnTabMove(!shiftDown);
}

LRESULT sw::IPAddressControl::_FieldsEditSubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    auto originalProc = reinterpret_cast<WNDPROC>(GetPropW(hwnd, _FieldsEditOriginalProc));
    auto pAddressCtrl = reinterpret_cast<IPAddressControl *>(GetPropW(hwnd, _IPAddressControlPtr));

    switch (uMsg) {
        case WM_CHAR: {
            if (wParam == L'\t') {
                if (WndBase::IsPtrValid(pAddressCtrl))
                    pAddressCtrl->_OnTabKeyDown();
                return 0;
            } else {
                // fallthrough
            }
        }
        default: {
            if (originalProc == NULL) {
                return DefWindowProcW(hwnd, uMsg, wParam, lParam);
            } else {
                return CallWindowProcW(originalProc, hwnd, uMsg, wParam, lParam);
            }
        }
    }
}
