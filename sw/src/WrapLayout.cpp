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
    void _ArrangeRow(double top, double height,
                     const std::vector<std::tuple<sw::ILayout *, sw::Size>> &row)
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
    void _ArrangeCol(double left, double width,
                     const std::vector<std::tuple<sw::ILayout *, sw::Size>> &col)
    {
        double top = 0;

        for (auto &tuple : col) {
            sw::ILayout *item = std::get<0>(tuple);
            sw::Size itemSize = std::get<1>(tuple);
            item->Arrange(sw::Rect{left, top, width, itemSize.height});
            top += itemSize.height;
        }
    }
}

sw::Size sw::WrapLayoutH::MeasureOverride(const Size &availableSize)
{
    Size size{};
    int count = GetChildLayoutCount();

    if (std::isinf(availableSize.width)) {
        for (int i = 0; i < count; ++i) {
            ILayout &item = GetChildLayoutAt(i);
            item.Measure(Size{INFINITY, INFINITY});

            Size itemDesireSize = item.GetDesireSize();
            size.width += itemDesireSize.width;
            size.height = Utils::Max(size.height, itemDesireSize.height);
        }
    } else {
        double top       = 0;
        double rowWidth  = 0;
        double rowHeight = 0;

        for (int i = 0; i < count; ++i) {
            ILayout &item = GetChildLayoutAt(i);
            item.Measure(Size{availableSize.width, INFINITY});

            Size itemDesireSize = item.GetDesireSize();
            if (rowWidth + itemDesireSize.width <= availableSize.width) {
                rowWidth += itemDesireSize.width;
                rowHeight = Utils::Max(rowHeight, itemDesireSize.height);
            } else {
                top += rowHeight;
                rowWidth  = itemDesireSize.width;
                rowHeight = itemDesireSize.height;
            }
            size.width = Utils::Max(size.width, rowWidth);
        }
        size.height = top + rowHeight;
    }

    return size;
}

void sw::WrapLayoutH::ArrangeOverride(const Size &finalSize)
{
    double top       = 0;
    double rowWidth  = 0;
    double rowHeight = 0;

    int count = GetChildLayoutCount();

    std::vector<std::tuple<ILayout *, Size>> row;
    row.reserve(count);

    for (int i = 0; i < count; ++i) {
        ILayout &item       = GetChildLayoutAt(i);
        Size itemDesireSize = item.GetDesireSize();

        if (rowWidth + itemDesireSize.width <= finalSize.width) {
            row.emplace_back(&item, itemDesireSize);
            rowWidth += itemDesireSize.width;
            rowHeight = Utils::Max(rowHeight, itemDesireSize.height);
        } else {
            if (!row.empty()) {
                _ArrangeRow(top, rowHeight, row);
            }
            row.clear();
            row.emplace_back(&item, itemDesireSize);
            top += rowHeight;
            rowWidth  = itemDesireSize.width;
            rowHeight = itemDesireSize.height;
        }
    }

    if (!row.empty()) {
        _ArrangeRow(top, rowHeight, row);
    }
}

sw::Size sw::WrapLayoutV::MeasureOverride(const Size &availableSize)
{
    Size size{};
    int count = GetChildLayoutCount();

    if (std::isinf(availableSize.height)) {
        for (int i = 0; i < count; ++i) {
            ILayout &item = GetChildLayoutAt(i);
            item.Measure(Size{INFINITY, INFINITY});

            Size itemDesireSize = item.GetDesireSize();
            size.height += itemDesireSize.height;
            size.width = Utils::Max(size.width, itemDesireSize.width);
        }
    } else {
        double left      = 0;
        double colHeight = 0;
        double colWidth  = 0;

        for (int i = 0; i < count; ++i) {
            ILayout &item = GetChildLayoutAt(i);
            item.Measure(Size{INFINITY, availableSize.height});

            Size itemDesireSize = item.GetDesireSize();
            if (colHeight + itemDesireSize.height <= availableSize.height) {
                colHeight += itemDesireSize.height;
                colWidth = Utils::Max(colWidth, itemDesireSize.width);
            } else {
                left += colWidth;
                colHeight = itemDesireSize.height;
                colWidth  = itemDesireSize.width;
            }
            size.height = Utils::Max(size.height, colHeight);
        }
        size.width = left + colWidth;
    }

    return size;
}

void sw::WrapLayoutV::ArrangeOverride(const Size &finalSize)
{
    double left      = 0;
    double colHeight = 0;
    double colWidth  = 0;

    int count = GetChildLayoutCount();

    std::vector<std::tuple<ILayout *, Size>> col;
    col.reserve(count);

    for (int i = 0; i < count; ++i) {
        ILayout &item       = GetChildLayoutAt(i);
        Size itemDesireSize = item.GetDesireSize();

        if (colHeight + itemDesireSize.height <= finalSize.height) {
            col.emplace_back(&item, itemDesireSize);
            colHeight += itemDesireSize.height;
            colWidth = Utils::Max(colWidth, itemDesireSize.width);
        } else {
            if (!col.empty()) {
                _ArrangeCol(left, colWidth, col);
            }
            col.clear();
            col.emplace_back(&item, itemDesireSize);
            left += colWidth;
            colHeight = itemDesireSize.height;
            colWidth  = itemDesireSize.width;
        }
    }

    if (!col.empty()) {
        _ArrangeCol(left, colWidth, col);
    }
}

sw::Size sw::WrapLayout::MeasureOverride(const Size &availableSize)
{
    return orientation == Orientation::Horizontal
               ? WrapLayoutH::MeasureOverride(availableSize)
               : WrapLayoutV::MeasureOverride(availableSize);
}

void sw::WrapLayout::ArrangeOverride(const Size &finalSize)
{
    orientation == Orientation::Horizontal
        ? WrapLayoutH::ArrangeOverride(finalSize)
        : WrapLayoutV::ArrangeOverride(finalSize);
}
