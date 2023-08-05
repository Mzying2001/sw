#include "Panel.h"

sw::Panel::Panel()
{
    this->InitControl(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | SS_NOTIFY, 0);
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

bool sw::Panel::OnSetCursor(HWND hwnd, int hitTest, int message, bool &useDefaultWndProc)
{
    return this->Control::OnSetCursor(hwnd, hitTest, message, useDefaultWndProc);
}
