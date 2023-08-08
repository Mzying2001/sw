#include "PanelBase.h"

sw::PanelBase::PanelBase()
{
}

bool sw::PanelBase::OnSetCursor(HWND hwnd, int hitTest, int message, bool &useDefaultWndProc)
{
    return this->Control::OnSetCursor(hwnd, hitTest, message, useDefaultWndProc);
}

void sw::PanelBase::Measure(const Size &availableSize)
{
    this->Layer::Measure(availableSize);
}

void sw::PanelBase::Arrange(const sw::Rect &finalPosition)
{
    this->Layer::Arrange(finalPosition);
}
