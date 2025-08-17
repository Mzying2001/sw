#include "PanelBase.h"

sw::PanelBase::PanelBase()
{
}

sw::PanelBase::~PanelBase()
{
}

sw::Size sw::PanelBase::MeasureOverride(const Size &availableSize)
{
    return this->Layer::MeasureOverride(availableSize);
}

void sw::PanelBase::ArrangeOverride(const Size &finalSize)
{
    this->Layer::ArrangeOverride(finalSize);
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

bool sw::PanelBase::RequestBringIntoView(const sw::Rect &screenRect)
{
    return this->Layer::RequestBringIntoView(screenRect);
}

sw::Control *sw::PanelBase::ToControl()
{
    return this->Control::ToControl();
}
