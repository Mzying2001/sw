#include "CheckableButton.h"

sw::CheckableButton::CheckableButton()
    : CheckState(
          // get
          [&]() -> const sw::CheckState & {
              static sw::CheckState result;
              result = (sw::CheckState)this->SendMessageW(BM_GETCHECK, 0, 0);
              return result;
          },
          // set
          [&](const sw::CheckState &value) {
              this->SendMessageW(BM_SETCHECK, (WPARAM)value, 0);
          }),

      IsChecked(
          // get
          [&]() -> const bool & {
              static bool result;
              result = this->CheckState.Get() == sw::CheckState::Checked;
              return result;
          },
          // set
          [&](const bool &value) {
              this->CheckState = value ? sw::CheckState::Checked : sw::CheckState::Unchecked;
          })
{
}

sw::CheckableButton::~CheckableButton()
{
}
