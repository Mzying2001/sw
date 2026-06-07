#include "HwndHost.h"

sw::HwndHost::HwndHost()
    : FillContent(
          Property<bool>::Init(this)
              .Getter([](HwndHost *self) -> bool {
                  return self->_fillContent;
              })
              .Setter([](HwndHost *self, bool value) {
                  if (self->_fillContent != value) {
                      self->_fillContent = value;
                      self->RaisePropertyChanged(&HwndHost::FillContent);
                  }
              })),

      SyncFont(
          Property<bool>::Init(this)
              .Getter([](HwndHost *self) -> bool {
                  return self->_syncFont;
              })
              .Setter([](HwndHost *self, bool value) {
                  if (self->_syncFont != value) {
                      self->_syncFont = value;
                      self->RaisePropertyChanged(&HwndHost::SyncFont);
                  }
              }))
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
    this->TBase::FontChanged(hfont);
}

bool sw::HwndHost::OnSize(const Size &newClientSize)
{
    if (this->_hWindowCore != NULL) {
        this->_SyncSize(newClientSize);
    }
    return this->TBase::OnSize(newClientSize);
}

bool sw::HwndHost::OnDestroy()
{
    this->DestroyWindowCore(this->_hWindowCore);
    this->_hWindowCore = NULL;
    return this->TBase::OnDestroy();
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
