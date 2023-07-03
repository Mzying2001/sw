#include "StackLayoutV.h"
#include <cmath>

void sw::StackLayoutV::MeasureOverride(Size &availableSize)
{
    Size desireSize;
    int childCount = this->GetChildLayoutCount();

    for (int i = 0; i < childCount; ++i) {
        ILayout &item = GetChildLayoutAt(i);
        item.Measure(Size(std::isinf(availableSize.width) ? INFINITY : availableSize.width, INFINITY));

        Size itemDesireSize = item.GetDesireSize();
        desireSize.height += itemDesireSize.height;
        desireSize.width = max(desireSize.width, itemDesireSize.width);
    }

    this->SetDesireSize(desireSize);
}

void sw::StackLayoutV::ArrangeOverride(Size &finalSize)
{
    double top     = 0;
    int childCount = this->GetChildLayoutCount();

    for (int i = 0; i < childCount; ++i) {
        ILayout &item       = GetChildLayoutAt(i);
        Size itemDesireSize = item.GetDesireSize();
        item.Arrange(Rect(0, top, finalSize.width, itemDesireSize.height));
        top += itemDesireSize.height;
    }
}
