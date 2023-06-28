#include "AbsoluteLayout.h"

void sw::AbsoluteLayout::Measure(const Size &availableSize)
{
    int count = this->GetChildLayoutCount();
    for (int i = 0; i < count; ++i) {
        ILayout &item = this->GetChildLayoutAt(i);
        item.Measure(Size(INFINITY, INFINITY));
    }
}

void sw::AbsoluteLayout::Arrange(const Rect &finalPosition)
{
    int count = this->GetChildLayoutCount();
    for (int i = 0; i < count; ++i) {
        ILayout &item = this->GetChildLayoutAt(i);
        item.Arrange(Rect(NAN, NAN, NAN, NAN));
    }
    // 注：UIElement对AbsoluteLayout有特殊处理，当Arrange参数结构体的第一个字段（即left）为NAN时，Arrange不执行任何操作
}
