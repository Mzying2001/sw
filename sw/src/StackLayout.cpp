#include "StackLayout.h"

sw::Size sw::StackLayout::MeasureOverride(const Size &availableSize)
{
    return this->orientation == Orientation::Horizontal
               ? this->StackLayoutH::MeasureOverride(availableSize)
               : this->StackLayoutV::MeasureOverride(availableSize);
}

void sw::StackLayout::ArrangeOverride(const Size &finalSize)
{
    this->orientation == Orientation::Horizontal
        ? this->StackLayoutH::ArrangeOverride(finalSize)
        : this->StackLayoutV::ArrangeOverride(finalSize);
}
