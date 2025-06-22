#include "CheckBox.h"

namespace
{
    constexpr DWORD _CheckBoxStyle_Normal     = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | BS_NOTIFY | BS_AUTOCHECKBOX;
    constexpr DWORD _CheckBoxStyle_ThreeState = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | BS_NOTIFY | BS_AUTO3STATE;
}

sw::CheckBox::CheckBox()
    : ThreeState(
          // get
          [this]() -> bool {
              return this->GetStyle() == _CheckBoxStyle_ThreeState;
          },
          // set
          [this](const bool &value) {
              this->SetStyle(value ? _CheckBoxStyle_ThreeState : _CheckBoxStyle_Normal);
          })
{
    this->InitButtonBase(L"CheckBox", _CheckBoxStyle_Normal, 0);
    this->Rect = sw::Rect(0, 0, 100, 20);
}
