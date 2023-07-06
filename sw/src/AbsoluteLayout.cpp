#include "AbsoluteLayout.h"
#include <cmath>

void sw::AbsoluteLayout::MeasureOverride(Size &availableSize)
{
    int count = this->GetChildLayoutCount();
    for (int i = 0; i < count; ++i) {
        ILayout &item = this->GetChildLayoutAt(i);
        item.Measure(Size(INFINITY, INFINITY));
    }
}

void sw::AbsoluteLayout::ArrangeOverride(Size &finalSize)
{
    int count = this->GetChildLayoutCount();
    for (int i = 0; i < count; ++i) {
        ILayout &item       = this->GetChildLayoutAt(i);
        Size itemDesireSize = item.GetDesireSize();
        item.Arrange(Rect(NAN, NAN, itemDesireSize.width, itemDesireSize.height));
    }
    // 注：UIElement对AbsoluteLayout有特殊处理，当Size的左边或顶边为NAN时，Arrange只调整尺寸不改位置
}
