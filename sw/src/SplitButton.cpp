#include "SplitButton.h"

sw::SplitButton::SplitButton()
{
    UpdateButtonStyle(false);
}

void sw::SplitButton::UpdateButtonStyle(bool focused)
{
    auto basestyle = GetStyle() & ~(BS_PUSHBUTTON | BS_DEFPUSHBUTTON | BS_SPLITBUTTON | BS_DEFSPLITBUTTON);
    SetStyle(focused ? (basestyle | BS_DEFSPLITBUTTON) : (basestyle | BS_SPLITBUTTON));
}
