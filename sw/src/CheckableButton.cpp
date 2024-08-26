#include "CheckableButton.h"

sw::CheckableButton::CheckableButton()
    : CheckState(
          // get
          [this]() -> sw::CheckState {
              return (sw::CheckState)this->SendMessageW(BM_GETCHECK, 0, 0);
          },
          // set
          [this](const sw::CheckState &value) {
              this->SendMessageW(BM_SETCHECK, (WPARAM)value, 0);
          }),

      IsChecked(
          // get
          [this]() -> bool {
              return this->CheckState.Get() == sw::CheckState::Checked;
          },
          // set
          [this](const bool &value) {
              this->CheckState = value ? sw::CheckState::Checked : sw::CheckState::Unchecked;
          })
{
}

sw::CheckableButton::~CheckableButton()
{
}
