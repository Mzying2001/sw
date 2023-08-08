#include "CheckBox.h"

static constexpr DWORD _CheckBoxStyle_Normal     = WS_CHILD | WS_VISIBLE | BS_NOTIFY | BS_AUTOCHECKBOX;
static constexpr DWORD _CheckBoxStyle_ThreeState = WS_CHILD | WS_VISIBLE | BS_NOTIFY | BS_AUTO3STATE;

sw::CheckBox::CheckBox()
    : ThreeState(
          // get
          [&]() -> const bool & {
              static bool result;
              result = this->GetStyle() == _CheckBoxStyle_ThreeState;
              return result;
          },
          // set
          [&](const bool &value) {
              this->SetStyle(value ? _CheckBoxStyle_ThreeState : _CheckBoxStyle_Normal);
          })
{
    this->InitButtonBase(L"CheckBox", _CheckBoxStyle_Normal, 0);
    this->Rect = sw::Rect(0, 0, 100, 20);
}
