#include "StackLayout.h"
#include "Utils.h"
#include <cmath>

namespace
{
    /**
     * @brief 水平堆叠布局的测量逻辑
     */
    sw::Size _StackLayoutMeasureOverrideHorz(sw::LayoutHost *self, const sw::Size &availableSize)
    {
        sw::Size desireSize{};
        int childCount = self->GetChildLayoutCount();

        for (int i = 0; i < childCount; ++i) {
            sw::ILayout &item = self->GetChildLayoutAt(i);
            item.Measure(sw::Size{INFINITY, std::isinf(availableSize.height) ? INFINITY : availableSize.height});

            sw::Size itemDesireSize = item.GetDesireSize();
            desireSize.width += itemDesireSize.width;
            desireSize.height = sw::Utils::Max(desireSize.height, itemDesireSize.height);
        }
        return desireSize;
    }

    /**
     * @brief 水平堆叠布局的排列逻辑
     */
    void _StackLayoutArrangeOverrideHorz(sw::LayoutHost *self, const sw::Size &finalSize)
    {
        double width   = 0;
        int childCount = self->GetChildLayoutCount();

        for (int i = 0; i < childCount; ++i) {
            sw::ILayout &item       = self->GetChildLayoutAt(i);
            sw::Size itemDesireSize = item.GetDesireSize();
            item.Arrange(sw::Rect{width, 0, itemDesireSize.width, finalSize.height});
            width += itemDesireSize.width;
        }
    }

    /**
     * @brief 垂直堆叠布局的测量逻辑
     */
    sw::Size _StackLayoutMeasureOverrideVert(sw::LayoutHost *self, const sw::Size &availableSize)
    {
        sw::Size desireSize{};
        int childCount = self->GetChildLayoutCount();

        for (int i = 0; i < childCount; ++i) {
            sw::ILayout &item = self->GetChildLayoutAt(i);
            item.Measure(sw::Size{std::isinf(availableSize.width) ? INFINITY : availableSize.width, INFINITY});

            sw::Size itemDesireSize = item.GetDesireSize();
            desireSize.height += itemDesireSize.height;
            desireSize.width = sw::Utils::Max(desireSize.width, itemDesireSize.width);
        }
        return desireSize;
    }

    /**
     * @brief 垂直堆叠布局的排列逻辑
     */
    void _StackLayoutArrangeOverrideVert(sw::LayoutHost *self, const sw::Size &finalSize)
    {
        double top     = 0;
        int childCount = self->GetChildLayoutCount();

        for (int i = 0; i < childCount; ++i) {
            sw::ILayout &item       = self->GetChildLayoutAt(i);
            sw::Size itemDesireSize = item.GetDesireSize();
            item.Arrange(sw::Rect{0, top, finalSize.width, itemDesireSize.height});
            top += itemDesireSize.height;
        }
    }
}

sw::Size sw::StackLayoutH::MeasureOverride(const Size &availableSize)
{
    return _StackLayoutMeasureOverrideHorz(this, availableSize);
}

void sw::StackLayoutH::ArrangeOverride(const Size &finalSize)
{
    _StackLayoutArrangeOverrideHorz(this, finalSize);
}

sw::Size sw::StackLayoutV::MeasureOverride(const Size &availableSize)
{
    return _StackLayoutMeasureOverrideVert(this, availableSize);
}

void sw::StackLayoutV::ArrangeOverride(const Size &finalSize)
{
    _StackLayoutArrangeOverrideVert(this, finalSize);
}

sw::Size sw::StackLayout::MeasureOverride(const Size &availableSize)
{
    if (orientation == Orientation::Horizontal) {
        return _StackLayoutMeasureOverrideHorz(this, availableSize);
    } else {
        return _StackLayoutMeasureOverrideVert(this, availableSize);
    }
}

void sw::StackLayout::ArrangeOverride(const Size &finalSize)
{
    if (orientation == Orientation::Horizontal) {
        _StackLayoutArrangeOverrideHorz(this, finalSize);
    } else {
        _StackLayoutArrangeOverrideVert(this, finalSize);
    }
}
