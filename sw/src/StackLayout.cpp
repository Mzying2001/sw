#include "StackLayout.h"
#include "Utils.h"
#include <cmath>

sw::Size sw::StackLayoutH::MeasureOverride(const Size &availableSize)
{
    Size desireSize{};
    int childCount = GetChildLayoutCount();

    for (int i = 0; i < childCount; ++i) {
        ILayout &item = GetChildLayoutAt(i);
        item.Measure(Size{INFINITY, std::isinf(availableSize.height) ? INFINITY : availableSize.height});

        Size itemDesireSize = item.GetDesireSize();
        desireSize.width += itemDesireSize.width;
        desireSize.height = Utils::Max(desireSize.height, itemDesireSize.height);
    }

    return desireSize;
}

void sw::StackLayoutH::ArrangeOverride(const Size &finalSize)
{
    double width   = 0;
    int childCount = GetChildLayoutCount();

    for (int i = 0; i < childCount; ++i) {
        ILayout &item       = GetChildLayoutAt(i);
        Size itemDesireSize = item.GetDesireSize();
        item.Arrange(Rect{width, 0, itemDesireSize.width, finalSize.height});
        width += itemDesireSize.width;
    }
}

sw::Size sw::StackLayoutV::MeasureOverride(const Size &availableSize)
{
    Size desireSize{};
    int childCount = GetChildLayoutCount();

    for (int i = 0; i < childCount; ++i) {
        ILayout &item = GetChildLayoutAt(i);
        item.Measure(Size{std::isinf(availableSize.width) ? INFINITY : availableSize.width, INFINITY});

        Size itemDesireSize = item.GetDesireSize();
        desireSize.height += itemDesireSize.height;
        desireSize.width = Utils::Max(desireSize.width, itemDesireSize.width);
    }

    return desireSize;
}

void sw::StackLayoutV::ArrangeOverride(const Size &finalSize)
{
    double top     = 0;
    int childCount = GetChildLayoutCount();

    for (int i = 0; i < childCount; ++i) {
        ILayout &item       = GetChildLayoutAt(i);
        Size itemDesireSize = item.GetDesireSize();
        item.Arrange(Rect{0, top, finalSize.width, itemDesireSize.height});
        top += itemDesireSize.height;
    }
}

sw::Size sw::StackLayout::MeasureOverride(const Size &availableSize)
{
    return orientation == Orientation::Horizontal
               ? StackLayoutH::MeasureOverride(availableSize)
               : StackLayoutV::MeasureOverride(availableSize);
}

void sw::StackLayout::ArrangeOverride(const Size &finalSize)
{
    orientation == Orientation::Horizontal
        ? StackLayoutH::ArrangeOverride(finalSize)
        : StackLayoutV::ArrangeOverride(finalSize);
}
