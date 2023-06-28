#include "WrapLayout.h"
#include <cmath>

void sw::WrapLayout::Measure(const Size &availableSize)
{
    Size size;
    int count = this->GetChildLayoutCount();

    if (std::isinf(availableSize.width)) {
        for (int i = 0; i < count; ++i) {
            ILayout &item = this->GetChildLayoutAt(i);
            item.Measure(Size(INFINITY, INFINITY));

            Size itemDesireSize = item.GetDesireSize();
            size.width += itemDesireSize.width;
            size.height = max(size.height, itemDesireSize.height);
        }
    } else {
        double top       = 0;
        double right     = 0;
        double rowHeight = 0;
        for (int i = 0; i < count; ++i) {
            ILayout &item = this->GetChildLayoutAt(i);
            item.Measure(Size(availableSize.width, INFINITY));

            Size itemDesireSize = item.GetDesireSize();
            if (right + itemDesireSize.width <= availableSize.width) {
                right     = right + itemDesireSize.width;
                rowHeight = max(rowHeight, itemDesireSize.height);
            } else {
                top += rowHeight;
                right     = itemDesireSize.width;
                rowHeight = itemDesireSize.height;
            }
        }
        size.width  = top == 0 ? availableSize.width : right;
        size.height = top + rowHeight;
    }

    this->SetDesireSize(size);
}

void sw::WrapLayout::Arrange(const Rect &finalPosition)
{
    int count        = this->GetChildLayoutCount();
    double top       = 0;
    double right     = 0;
    double rowHeight = 0;
    for (int i = 0; i < count; ++i) {
        ILayout &item       = this->GetChildLayoutAt(i);
        Size itemDesireSize = item.GetDesireSize();
        if (right + itemDesireSize.width <= finalPosition.width) {
            item.Arrange(Rect(right, top, itemDesireSize.width, itemDesireSize.height));
            right     = right + itemDesireSize.width;
            rowHeight = max(rowHeight, itemDesireSize.height);
        } else {
            top += rowHeight;
            item.Arrange(Rect(0, top, itemDesireSize.width, itemDesireSize.height));
            right     = itemDesireSize.width;
            rowHeight = itemDesireSize.height;
        }
    }
}
