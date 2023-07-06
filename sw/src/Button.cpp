#include "Button.h"

sw::Button::Button()
{
    this->InitButtonBase(L"Button", WS_CHILD | WS_VISIBLE | BS_NOTIFY | BS_PUSHBUTTON);
    this->Rect = sw::Rect(0, 0, 70, 30);
}
