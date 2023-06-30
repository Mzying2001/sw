#include "FillLayout.h"

void sw::FillLayout::MeasureOverride(Size &availableSize)
{
    int count = this->GetChildLayoutCount();
    for (int i = 0; i < count; ++i) {
        ILayout &item = this->GetChildLayoutAt(i);
        item.Measure(availableSize);
    }
    this->SetDesireSize(availableSize);
}

void sw::FillLayout::ArrangeOverride(Size &finalSize)
{
    int count = this->GetChildLayoutCount();
    for (int i = 0; i < count; ++i) {
        ILayout &item = this->GetChildLayoutAt(i);
        item.Arrange(Rect(0, 0, finalSize.width, finalSize.height));
    }
}
