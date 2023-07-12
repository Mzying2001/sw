#include "Panel.h"

sw::Panel::Panel()
{
    this->InitControl(L"STATIC", NULL, WS_CHILD | WS_VISIBLE);
    this->HorizontalAlignment = HorizontalAlignment::Stretch;
    this->VerticalAlignment   = VerticalAlignment::Stretch;
}

void sw::Panel::HandleInitialized(HWND hwnd)
{
    this->Control::HandleInitialized(hwnd);
}

void sw::Panel::Measure(const Size &availableSize)
{
    this->Layer::Measure(availableSize);
}

void sw::Panel::Arrange(const sw::Rect &finalPosition)
{
    this->Layer::Arrange(finalPosition);
}
