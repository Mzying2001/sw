#include "Panel.h"

sw::Panel::Panel()
{
    this->InitControl(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | SS_NOTIFY, 0);
    this->HorizontalAlignment = HorizontalAlignment::Stretch;
    this->VerticalAlignment   = VerticalAlignment::Stretch;
}
