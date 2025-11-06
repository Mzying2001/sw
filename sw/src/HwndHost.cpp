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
          }),

      SyncFont(
          // get
          [this]() -> bool {
              return this->_syncFont;
          },
          // set
          [this](const bool &value) {
              this->_syncFont = value;
          })
{
    this->Rect = sw::Rect{0, 0, 100, 100};
}

sw::HwndHost::~HwndHost()
{
    if (!this->IsDestroyed) {
        DestroyWindow(this->Handle);
    }
}

void sw::HwndHost::InitHwndHost()
{
    if (this->_hWindowCore == NULL && !this->IsDestroyed) //
    {
        this->_hWindowCore = this->BuildWindowCore(this->Handle);

        if (this->_hWindowCore != NULL) {
            this->_SyncSize(this->ClientRect->GetSize());
            this->_SyncFont(this->GetFontHandle());
        }
    }
}

void sw::HwndHost::FontChanged(HFONT hfont)
{
    if (this->_hWindowCore != NULL) {
        this->_SyncFont(hfont);
    }
    this->StaticControl::FontChanged(hfont);
}

bool sw::HwndHost::OnSize(const Size &newClientSize)
{
    if (this->_hWindowCore != NULL) {
        this->_SyncSize(newClientSize);
    }
    return this->StaticControl::OnSize(newClientSize);
}

bool sw::HwndHost::OnDestroy()
{
    this->DestroyWindowCore(this->_hWindowCore);
    this->_hWindowCore = NULL;
    return this->StaticControl::OnDestroy();
}

void sw::HwndHost::_SyncSize(const SIZE &newSize)
{
    if (this->_hWindowCore != NULL && this->_fillContent) {
        SetWindowPos(
            this->_hWindowCore, NULL,
            0, 0, newSize.cx, newSize.cy,
            SWP_NOACTIVATE | SWP_NOZORDER);
    }
}

void sw::HwndHost::_SyncFont(HFONT hfont)
{
    if (this->_hWindowCore != NULL && this->_syncFont) {
        ::SendMessageW(this->_hWindowCore, WM_SETFONT, reinterpret_cast<WPARAM>(hfont), TRUE);
    }
}
