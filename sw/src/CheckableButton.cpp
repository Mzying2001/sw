#include "CheckableButton.h"

sw::CheckableButton::CheckableButton()
    : CheckState(
          Property<sw::CheckState>::Init(this)
              .Getter([](CheckableButton *self) -> sw::CheckState {
                  return (sw::CheckState)self->SendMessageW(BM_GETCHECK, 0, 0);
              })
              .Setter([](CheckableButton *self, sw::CheckState value) {
                  if (self->CheckState != value) {
                      self->SendMessageW(BM_SETCHECK, (WPARAM)value, 0);
                      self->RaisePropertyChanged(&CheckableButton::CheckState);
                      self->RaisePropertyChanged(&CheckableButton::IsChecked);
                  }
              })),

      IsChecked(
          Property<bool>::Init(this)
              .Getter([](CheckableButton *self) -> bool {
                  return self->CheckState == sw::CheckState::Checked;
              })
              .Setter([](CheckableButton *self, bool value) {
                  self->CheckState = value ? sw::CheckState::Checked : sw::CheckState::Unchecked;
              }))
{
}

sw::CheckableButton::~CheckableButton()
{
}

void sw::CheckableButton::OnClicked()
{
    RaisePropertyChanged(&CheckableButton::CheckState);
    RaisePropertyChanged(&CheckableButton::IsChecked);
    ButtonBase::OnClicked();
}

void sw::CheckableButton::OnDoubleClicked()
{
    RaisePropertyChanged(&CheckableButton::CheckState);
    RaisePropertyChanged(&CheckableButton::IsChecked);
    ButtonBase::OnDoubleClicked();
}
