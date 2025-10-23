#include "FillLayout.h"
#include "Utils.h"

sw::Size sw::FillLayout::MeasureOverride(const Size &availableSize)
{
    Size desireSize{};
    int count = this->GetChildLayoutCount();

    for (int i = 0; i < count; ++i) {
        ILayout &item = this->GetChildLayoutAt(i);
        item.Measure(availableSize);
        Size itemDesireSize = item.GetDesireSize();
        desireSize.width    = Utils::Max(desireSize.width, itemDesireSize.width);
        desireSize.height   = Utils::Max(desireSize.height, itemDesireSize.height);
    }
    return desireSize;
}

void sw::FillLayout::ArrangeOverride(const Size &finalSize)
{
    int count = this->GetChildLayoutCount();

    for (int i = 0; i < count; ++i) {
        ILayout &item = this->GetChildLayoutAt(i);
        item.Arrange(Rect{0, 0, finalSize.width, finalSize.height});
    }
}
