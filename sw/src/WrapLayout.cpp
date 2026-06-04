#include "WrapLayout.h"
#include "Utils.h"
#include <cmath>
#include <tuple>
#include <vector>

namespace
{
    /**
     * @brief 按行高统一安排一行中的所有子元素，使行内元素占用相同的垂直布局空间
     */
    void _WrapLayoutArrangeRow(
        double top, double height, const std::vector<std::tuple<sw::ILayout *, sw::Size>> &row)
    {
        double left = 0;

        for (auto &tuple : row) {
            sw::ILayout *item = std::get<0>(tuple);
            sw::Size itemSize = std::get<1>(tuple);
            item->Arrange(sw::Rect{left, top, itemSize.width, height});
            left += itemSize.width;
        }
    }

    /**
     * @brief 按列宽统一安排一列中的所有子元素，使列内元素占用相同的水平布局空间
     */
    void _WrapLayoutArrangeCol(
        double left, double width, const std::vector<std::tuple<sw::ILayout *, sw::Size>> &col)
    {
        double top = 0;

        for (auto &tuple : col) {
            sw::ILayout *item = std::get<0>(tuple);
            sw::Size itemSize = std::get<1>(tuple);
            item->Arrange(sw::Rect{left, top, width, itemSize.height});
            top += itemSize.height;
        }
    }

    /**
     * @brief 水平自动换行布局的测量逻辑
     */
    sw::Size _WrapLayoutMeasureOverrideHorz(sw::LayoutHost *self, const sw::Size &availableSize)
    {
        sw::Size size{};
        int count = self->GetChildLayoutCount();

        if (std::isinf(availableSize.width)) {
            for (int i = 0; i < count; ++i) {
                sw::ILayout &item = self->GetChildLayoutAt(i);
                item.Measure(sw::Size{INFINITY, INFINITY});

                sw::Size itemDesireSize = item.GetDesireSize();
                size.width += itemDesireSize.width;
                size.height = sw::Utils::Max(size.height, itemDesireSize.height);
            }
        } else {
            double top       = 0;
            double rowWidth  = 0;
            double rowHeight = 0;

            for (int i = 0; i < count; ++i) {
                sw::ILayout &item = self->GetChildLayoutAt(i);
                item.Measure(sw::Size{availableSize.width, INFINITY});

                sw::Size itemDesireSize = item.GetDesireSize();
                if (rowWidth + itemDesireSize.width <= availableSize.width) {
                    rowWidth += itemDesireSize.width;
                    rowHeight = sw::Utils::Max(rowHeight, itemDesireSize.height);
                } else {
                    top += rowHeight;
                    rowWidth  = itemDesireSize.width;
                    rowHeight = itemDesireSize.height;
                }
                size.width = sw::Utils::Max(size.width, rowWidth);
            }
            size.height = top + rowHeight;
        }

        return size;
    }

    /**
     * @brief 水平自动换行布局的排列逻辑
     */
    void _WrapLayoutArrangeOverrideHorz(sw::LayoutHost *self, const sw::Size &finalSize)
    {
        double top       = 0;
        double rowWidth  = 0;
        double rowHeight = 0;

        int count = self->GetChildLayoutCount();

        std::vector<std::tuple<sw::ILayout *, sw::Size>> row;
        row.reserve(count);

        for (int i = 0; i < count; ++i) {
            sw::ILayout &item       = self->GetChildLayoutAt(i);
            sw::Size itemDesireSize = item.GetDesireSize();

            if (rowWidth + itemDesireSize.width <= finalSize.width) {
                row.emplace_back(&item, itemDesireSize);
                rowWidth += itemDesireSize.width;
                rowHeight = sw::Utils::Max(rowHeight, itemDesireSize.height);
            } else {
                if (!row.empty()) {
                    _WrapLayoutArrangeRow(top, rowHeight, row);
                }
                row.clear();
                row.emplace_back(&item, itemDesireSize);
                top += rowHeight;
                rowWidth  = itemDesireSize.width;
                rowHeight = itemDesireSize.height;
            }
        }

        if (!row.empty()) {
            _WrapLayoutArrangeRow(top, rowHeight, row);
        }
    }

    /**
     * @brief 垂直自动换行布局的测量逻辑
     */
    sw::Size _WrapLayoutMeasureOverrideVert(sw::LayoutHost *self, const sw::Size &availableSize)
    {
        sw::Size size{};
        int count = self->GetChildLayoutCount();

        if (std::isinf(availableSize.height)) {
            for (int i = 0; i < count; ++i) {
                sw::ILayout &item = self->GetChildLayoutAt(i);
                item.Measure(sw::Size{INFINITY, INFINITY});

                sw::Size itemDesireSize = item.GetDesireSize();
                size.height += itemDesireSize.height;
                size.width = sw::Utils::Max(size.width, itemDesireSize.width);
            }
        } else {
            double left      = 0;
            double colHeight = 0;
            double colWidth  = 0;

            for (int i = 0; i < count; ++i) {
                sw::ILayout &item = self->GetChildLayoutAt(i);
                item.Measure(sw::Size{INFINITY, availableSize.height});

                sw::Size itemDesireSize = item.GetDesireSize();
                if (colHeight + itemDesireSize.height <= availableSize.height) {
                    colHeight += itemDesireSize.height;
                    colWidth = sw::Utils::Max(colWidth, itemDesireSize.width);
                } else {
                    left += colWidth;
                    colHeight = itemDesireSize.height;
                    colWidth  = itemDesireSize.width;
                }
                size.height = sw::Utils::Max(size.height, colHeight);
            }
            size.width = left + colWidth;
        }

        return size;
    }

    /**
     * @brief 垂直自动换行布局的排列逻辑
     */
    void _WrapLayoutArrangeOverrideVert(sw::LayoutHost *self, const sw::Size &finalSize)
    {
        double left      = 0;
        double colHeight = 0;
        double colWidth  = 0;

        int count = self->GetChildLayoutCount();

        std::vector<std::tuple<sw::ILayout *, sw::Size>> col;
        col.reserve(count);

        for (int i = 0; i < count; ++i) {
            sw::ILayout &item       = self->GetChildLayoutAt(i);
            sw::Size itemDesireSize = item.GetDesireSize();

            if (colHeight + itemDesireSize.height <= finalSize.height) {
                col.emplace_back(&item, itemDesireSize);
                colHeight += itemDesireSize.height;
                colWidth = sw::Utils::Max(colWidth, itemDesireSize.width);
            } else {
                if (!col.empty()) {
                    _WrapLayoutArrangeCol(left, colWidth, col);
                }
                col.clear();
                col.emplace_back(&item, itemDesireSize);
                left += colWidth;
                colHeight = itemDesireSize.height;
                colWidth  = itemDesireSize.width;
            }
        }

        if (!col.empty()) {
            _WrapLayoutArrangeCol(left, colWidth, col);
        }
    }
}

sw::Size sw::WrapLayoutH::MeasureOverride(const Size &availableSize)
{
    return _WrapLayoutMeasureOverrideHorz(this, availableSize);
}

void sw::WrapLayoutH::ArrangeOverride(const Size &finalSize)
{
    _WrapLayoutArrangeOverrideHorz(this, finalSize);
}

sw::Size sw::WrapLayoutV::MeasureOverride(const Size &availableSize)
{
    return _WrapLayoutMeasureOverrideVert(this, availableSize);
}

void sw::WrapLayoutV::ArrangeOverride(const Size &finalSize)
{
    _WrapLayoutArrangeOverrideVert(this, finalSize);
}

sw::Size sw::WrapLayout::MeasureOverride(const Size &availableSize)
{
    if (orientation == Orientation::Horizontal) {
        return _WrapLayoutMeasureOverrideHorz(this, availableSize);
    } else {
        return _WrapLayoutMeasureOverrideVert(this, availableSize);
    }
}

void sw::WrapLayout::ArrangeOverride(const Size &finalSize)
{
    if (orientation == Orientation::Horizontal) {
        _WrapLayoutArrangeOverrideHorz(this, finalSize);
    } else {
        _WrapLayoutArrangeOverrideVert(this, finalSize);
    }
}
