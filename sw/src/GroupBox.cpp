#include "GroupBox.h"

sw::GroupBox::GroupBox()
{
    this->InitControl(L"BUTTON", L"GroupBox", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | BS_GROUPBOX, 0);
    this->Rect = sw::Rect(0, 0, 200, 200);
}
