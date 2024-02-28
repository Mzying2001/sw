#include "HwndHost.h"

void sw::HwndHost::InitHwndHost()
{
    if (this->_hWindowCore == NULL && !this->IsDestroyed)
        this->_hWindowCore = this->BuildWindowCore(this->Handle);
}

bool sw::HwndHost::OnSize(Size newClientSize)
{
    if (this->_hWindowCore != NULL) {
        SetWindowPos(this->_hWindowCore, NULL, 0, 0,
                     Dip::DipToPxX(newClientSize.width),
                     Dip::DipToPxY(newClientSize.height),
                     SWP_NOACTIVATE | SWP_NOZORDER);
    }
    return this->StaticControl::OnSize(newClientSize);
}

bool sw::HwndHost::OnDestroy()
{
    this->DestroyWindowCore(this->_hWindowCore);
    this->_hWindowCore = NULL;
    return this->StaticControl::OnDestroy();
}
