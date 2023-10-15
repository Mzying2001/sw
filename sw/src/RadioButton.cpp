#include "RadioButton.h"

sw::RadioButton::RadioButton()
{
    this->InitButtonBase(L"RadioButton", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | BS_NOTIFY | BS_AUTORADIOBUTTON, 0);
    this->Rect = sw::Rect(0, 0, 100, 20);
}
