#include "HwndHost.h"

sw::HwndHost::HwndHost()
    : FillContent(
          // get
          [this]() -> bool {
              return this->_fillContent;
          },
          // set
          [this](const bool &value) {
              this->_fillContent = value;
          })
{
    this->Rect = sw::Rect{0, 0, 100, 100};
}

void sw::HwndHost::InitHwndHost()
{
    if (this->_hWindowCore == NULL && !this->IsDestroyed)
        this->_hWindowCore = this->BuildWindowCore(this->Handle);
}

bool sw::HwndHost::OnSize(const Size &newClientSize)
{
    if (this->_hWindowCore != NULL && this->_fillContent) {
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
