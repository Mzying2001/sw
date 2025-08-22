#include "CheckBox.h"

sw::CheckBox::CheckBox()
    : ThreeState(
          // get
          [this]() -> bool {
              return (GetStyle() & BS_AUTO3STATE) == BS_AUTO3STATE;
          },
          // set
          [this](const bool &value) {
              auto style = GetStyle() & ~(BS_AUTOCHECKBOX | BS_AUTO3STATE);
              SetStyle(value ? (style | BS_AUTO3STATE) : (style | BS_AUTOCHECKBOX));
          })
{
    InitButtonBase(L"CheckBox", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | BS_NOTIFY | BS_AUTOCHECKBOX, 0);
    Rect     = sw::Rect{0, 0, 100, 20};
    AutoSize = true;
}
