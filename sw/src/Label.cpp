#include "Label.h"

sw::Label::Label()
{
    this->InitControl(L"STATIC", L"Label", WS_CHILD | WS_VISIBLE);
    this->Rect = sw::Rect(0, 0, 40, 20);
}
