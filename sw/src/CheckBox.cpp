#include "CheckBox.h"

sw::CheckBox::CheckBox()
    : ThreeState(
          Property<bool>::Init(this)
              .Getter([](CheckBox *self) -> bool {
                  return (self->GetStyle() & BS_AUTO3STATE) == BS_AUTO3STATE;
              })
              .Setter([](CheckBox *self, bool value) {
                  auto style = self->GetStyle() & ~(BS_AUTOCHECKBOX | BS_AUTO3STATE);
                  self->SetStyle(value ? (style | BS_AUTO3STATE) : (style | BS_AUTOCHECKBOX));
              }))
{
    InitButtonBase(L"CheckBox", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | BS_NOTIFY | BS_AUTOCHECKBOX, 0);
    Rect     = sw::Rect{0, 0, 100, 20};
    AutoSize = true;
}
