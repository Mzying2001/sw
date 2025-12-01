#include "CheckableButton.h"

sw::CheckableButton::CheckableButton()
    : CheckState(
          Property<sw::CheckState>::Init(this)
              .Getter([](CheckableButton *self) -> sw::CheckState {
                  return (sw::CheckState)self->SendMessageW(BM_GETCHECK, 0, 0);
              })
              .Setter([](CheckableButton *self, sw::CheckState value) {
                  self->SendMessageW(BM_SETCHECK, (WPARAM)value, 0);
              })),

      IsChecked(
          Property<bool>::Init(this)
              .Getter([](CheckableButton *self) -> bool {
                  return self->CheckState.Get() == sw::CheckState::Checked;
              })
              .Setter([](CheckableButton *self, bool value) {
                  self->CheckState = value ? sw::CheckState::Checked : sw::CheckState::Unchecked;
              }))
{
}

sw::CheckableButton::~CheckableButton()
{
}
