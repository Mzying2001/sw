#include "StaticControl.h"

sw::StaticControl::StaticControl()
    : Notify(
          Property<bool>::Init(this)
              .Getter([](StaticControl *self) -> bool {
                  return self->GetStyle(SS_NOTIFY);
              })
              .Setter([](StaticControl *self, bool value) {
                  if (self->Notify != value) {
                      self->SetStyle(SS_NOTIFY, value);
                      self->RaisePropertyChanged(&StaticControl::Notify);
                  }
              }))
{
    this->InitControl(L"STATIC", L"", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, 0);
}
