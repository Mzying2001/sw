#include "StackLayoutV.h"
#include "Utils.h"
#include <cmath>

void sw::StackLayoutV::MeasureOverride(Size &availableSize)
{
    Size desireSize;
    int childCount = this->GetChildLayoutCount();

    for (int i = 0; i < childCount; ++i) {
        ILayout &item = this->GetChildLayoutAt(i);
        item.Measure(Size(std::isinf(availableSize.width) ? INFINITY : availableSize.width, INFINITY));

        Size itemDesireSize = item.GetDesireSize();
        desireSize.height += itemDesireSize.height;
        desireSize.width = Utils::Max(desireSize.width, itemDesireSize.width);
    }

    this->SetDesireSize(desireSize);
}

void sw::StackLayoutV::ArrangeOverride(Size &finalSize)
{
    double top     = 0;
    int childCount = this->GetChildLayoutCount();

    for (int i = 0; i < childCount; ++i) {
        ILayout &item = this->GetChildLayoutAt(i);

        Size itemDesireSize = item.GetDesireSize();
        item.Arrange(Rect(0, top, finalSize.width, itemDesireSize.height));
        top += itemDesireSize.height;
    }
}
