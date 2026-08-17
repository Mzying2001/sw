#include "StackLayout.h"
#include "Utils.h"
#include <cmath>

namespace
{
    /**
     * @brief 水平堆叠布局的测量逻辑
     */
    sw::Size _StackLayoutMeasureOverrideHorz(
        sw::LayoutHost *self, const sw::Size &availableSize, double spacing)
    {
        sw::Size desireSize{};
        int childCount = self->GetChildLayoutCount();

        for (int i = 0; i < childCount; ++i) {
            sw::ILayout &item = self->GetChildLayoutAt(i);
            item.Measure(sw::Size{INFINITY, std::isinf(availableSize.height) ? INFINITY : availableSize.height});

            sw::Size itemDesireSize = item.GetDesireSize();
            if (i > 0) {
                desireSize.width += spacing;
            }
            desireSize.width += itemDesireSize.width;
            desireSize.height = sw::Utils::Max(desireSize.height, itemDesireSize.height);
        }
        return desireSize;
    }

    /**
     * @brief 水平堆叠布局的排列逻辑
     */
    void _StackLayoutArrangeOverrideHorz(
        sw::LayoutHost *self, const sw::Size &finalSize, double spacing)
    {
        double width   = 0;
        int childCount = self->GetChildLayoutCount();

        for (int i = 0; i < childCount; ++i) {
            sw::ILayout &item       = self->GetChildLayoutAt(i);
            sw::Size itemDesireSize = item.GetDesireSize();
            if (i > 0) {
                width += spacing;
            }
            item.Arrange(sw::Rect{width, 0, itemDesireSize.width, finalSize.height});
            width += itemDesireSize.width;
        }
    }

    /**
     * @brief 垂直堆叠布局的测量逻辑
     */
    sw::Size _StackLayoutMeasureOverrideVert(
        sw::LayoutHost *self, const sw::Size &availableSize, double spacing)
    {
        sw::Size desireSize{};
        int childCount = self->GetChildLayoutCount();

        for (int i = 0; i < childCount; ++i) {
            sw::ILayout &item = self->GetChildLayoutAt(i);
            item.Measure(sw::Size{std::isinf(availableSize.width) ? INFINITY : availableSize.width, INFINITY});

            sw::Size itemDesireSize = item.GetDesireSize();
            if (i > 0) {
                desireSize.height += spacing;
            }
            desireSize.height += itemDesireSize.height;
            desireSize.width = sw::Utils::Max(desireSize.width, itemDesireSize.width);
        }
        return desireSize;
    }

    /**
     * @brief 垂直堆叠布局的排列逻辑
     */
    void _StackLayoutArrangeOverrideVert(
        sw::LayoutHost *self, const sw::Size &finalSize, double spacing)
    {
        double top     = 0;
        int childCount = self->GetChildLayoutCount();

        for (int i = 0; i < childCount; ++i) {
            sw::ILayout &item       = self->GetChildLayoutAt(i);
            sw::Size itemDesireSize = item.GetDesireSize();
            if (i > 0) {
                top += spacing;
            }
            item.Arrange(sw::Rect{0, top, finalSize.width, itemDesireSize.height});
            top += itemDesireSize.height;
        }
    }
}

sw::Size sw::StackLayoutH::MeasureOverride(const Size &availableSize)
{
    return _StackLayoutMeasureOverrideHorz(this, availableSize, 0);
}

void sw::StackLayoutH::ArrangeOverride(const Size &finalSize)
{
    _StackLayoutArrangeOverrideHorz(this, finalSize, 0);
}

sw::Size sw::StackLayoutV::MeasureOverride(const Size &availableSize)
{
    return _StackLayoutMeasureOverrideVert(this, availableSize, 0);
}

void sw::StackLayoutV::ArrangeOverride(const Size &finalSize)
{
    _StackLayoutArrangeOverrideVert(this, finalSize, 0);
}

sw::Size sw::StackLayout::MeasureOverride(const Size &availableSize)
{
    if (orientation == Orientation::Horizontal) {
        return _StackLayoutMeasureOverrideHorz(this, availableSize, spacing);
    } else {
        return _StackLayoutMeasureOverrideVert(this, availableSize, spacing);
    }
}

void sw::StackLayout::ArrangeOverride(const Size &finalSize)
{
    if (orientation == Orientation::Horizontal) {
        _StackLayoutArrangeOverrideHorz(this, finalSize, spacing);
    } else {
        _StackLayoutArrangeOverrideVert(this, finalSize, spacing);
    }
}
