#include "StackLayout.h"

void sw::StackLayout::MeasureOverride(Size &availableSize)
{
    this->orientation == Orientation::Horizontal
        ? this->StackLayoutH::MeasureOverride(availableSize)
        : this->StackLayoutV::MeasureOverride(availableSize);
}

void sw::StackLayout::ArrangeOverride(Size &finalSize)
{
    this->orientation == Orientation::Horizontal
        ? this->StackLayoutH::ArrangeOverride(finalSize)
        : this->StackLayoutV::ArrangeOverride(finalSize);
}
