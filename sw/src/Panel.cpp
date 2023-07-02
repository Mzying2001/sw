#include "Panel.h"

sw::Panel::Panel()
{
    this->InitControl(L"STATIC", NULL, WS_CHILD | WS_VISIBLE);
}

void sw::Panel::Measure(const Size &availableSize)
{
    this->Layer::Measure(availableSize);
}

void sw::Panel::Arrange(const sw::Rect &finalPosition)
{
    this->Layer::Arrange(finalPosition);
}
