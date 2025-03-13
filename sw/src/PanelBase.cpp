#include "PanelBase.h"

sw::PanelBase::PanelBase()
{
}

sw::PanelBase::~PanelBase()
{
}

bool sw::PanelBase::OnVerticalScroll(int event, int pos)
{
    return this->Layer::OnVerticalScroll(event, pos);
}

bool sw::PanelBase::OnHorizontalScroll(int event, int pos)
{
    return this->Layer::OnHorizontalScroll(event, pos);
}

bool sw::PanelBase::OnNotified(NMHDR *pNMHDR, LRESULT &result)
{
    return this->Control::OnNotified(pNMHDR, result);
}

bool sw::PanelBase::OnKillFocus(HWND hNextFocus)
{
    return this->Control::OnKillFocus(hNextFocus);
}

void sw::PanelBase::OnTabStop()
{
    this->Control::OnTabStop();
}

void sw::PanelBase::OnEndPaint()
{
    this->Control::OnEndPaint();
}

void sw::PanelBase::Measure(const Size &availableSize)
{
    this->Layer::Measure(availableSize);
}

void sw::PanelBase::Arrange(const sw::Rect &finalPosition)
{
    this->Layer::Arrange(finalPosition);
}
