#include "PanelBase.h"

sw::PanelBase::PanelBase()
{
}

bool sw::PanelBase::OnSetCursor(HWND hwnd, HitTestResult hitTest, int message, bool &result)
{
    return this->Control::OnSetCursor(hwnd, hitTest, message, result);
}

bool sw::PanelBase::OnVerticalScroll(int event, int pos)
{
    return this->Layer::OnVerticalScroll(event, pos);
}

bool sw::PanelBase::OnHorizontalScroll(int event, int pos)
{
    return this->Layer::OnHorizontalScroll(event, pos);
}

void sw::PanelBase::Measure(const Size &availableSize)
{
    this->Layer::Measure(availableSize);
}

void sw::PanelBase::Arrange(const sw::Rect &finalPosition)
{
    this->Layer::Arrange(finalPosition);
}
