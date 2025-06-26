#include "WrapLayout.h"

sw::Size sw::WrapLayout::MeasureOverride(const Size &availableSize)
{
    return this->orientation == Orientation::Horizontal
               ? this->WrapLayoutH::MeasureOverride(availableSize)
               : this->WrapLayoutV::MeasureOverride(availableSize);
}

void sw::WrapLayout::ArrangeOverride(const Size &finalSize)
{
    this->orientation == Orientation::Horizontal
        ? this->WrapLayoutH::ArrangeOverride(finalSize)
        : this->WrapLayoutV::ArrangeOverride(finalSize);
}
