#include "Button.h"

sw::Button::Button()
{
    this->InitControl(L"BUTTON", L"Button", WS_CHILD | WS_VISIBLE | BS_NOTIFY | BS_PUSHBUTTON);
}
