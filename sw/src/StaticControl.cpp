#include "StaticControl.h"

sw::StaticControl::StaticControl()
    : Notify(
          Property<bool>::Init(this)
              .Getter([](StaticControl *self) -> bool {
                  return self->GetStyle(SS_NOTIFY);
              })
              .Setter([](StaticControl *self, bool value) {
                  self->SetStyle(SS_NOTIFY, value);
              }))
{
    this->InitControl(L"STATIC", L"", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, 0);
}
