#include "CanvasLayout.h"
#include "Utils.h"
#include <cmath>

sw::CanvasLayoutTag::CanvasLayoutTag()
    : left(0), top(0)
{
}

sw::CanvasLayoutTag::CanvasLayoutTag(float left, float top)
    : left(left), top(top)
{
}

sw::CanvasLayoutTag::CanvasLayoutTag(uint64_t layoutTag)
    : left(reinterpret_cast<float *>(&layoutTag)[0]),
      top(reinterpret_cast<float *>(&layoutTag)[1])
{
}

sw::CanvasLayoutTag::operator uint64_t() const
{
    uint64_t result;
    reinterpret_cast<float *>(&result)[0] = left;
    reinterpret_cast<float *>(&result)[1] = top;
    return result;
}

void sw::CanvasLayout::MeasureOverride(Size &availableSize)
{
    Size desireSize{};
    Size measureSize{INFINITY, INFINITY};

    int childCount = this->GetChildLayoutCount();
    for (int i = 0; i < childCount; ++i) {
        ILayout &item = this->GetChildLayoutAt(i);

        item.Measure(measureSize);
        Size childDesireSize = item.GetDesireSize();
        CanvasLayoutTag tag  = item.GetLayoutTag();
        desireSize.width     = Utils::Max(tag.left + childDesireSize.width, desireSize.width);
        desireSize.height    = Utils::Max(tag.top + childDesireSize.height, desireSize.height);
    }

    this->SetDesireSize(desireSize);
}

void sw::CanvasLayout::ArrangeOverride(Size &finalSize)
{
    int childCount = this->GetChildLayoutCount();
    for (int i = 0; i < childCount; ++i) {
        ILayout &item        = this->GetChildLayoutAt(i);
        Size childDesireSize = item.GetDesireSize();
        CanvasLayoutTag tag  = item.GetLayoutTag();
        item.Arrange(Rect{tag.left, tag.top, childDesireSize.width, childDesireSize.height});
    }
}
