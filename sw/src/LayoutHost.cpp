#include "LayoutHost.h"

void sw::LayoutHost::Associate(ILayout *obj)
{
    this->_associatedObj = obj;
}

int sw::LayoutHost::GetChildLayoutCount()
{
    return this->_associatedObj->GetChildLayoutCount();
}

sw::ILayout &sw::LayoutHost::GetChildLayoutAt(int index)
{
    return this->_associatedObj->GetChildLayoutAt(index);
}

sw::Size sw::LayoutHost::GetDesireSize()
{
    return this->_associatedObj->GetDesireSize();
}

void sw::LayoutHost::SetDesireSize(const Size &size)
{
    this->_associatedObj->SetDesireSize(size);
}

void sw::LayoutHost::Measure(const Size &availableSize)
{
    Size size = availableSize;
    this->MeasureOverride(size);
}

void sw::LayoutHost::Arrange(const Rect &finalPosition)
{
    Size size(finalPosition.width, finalPosition.height);
    this->ArrangeOverride(size);
}
