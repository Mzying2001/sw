#include "WrapLayoutH.h"
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
}

sw::Size sw::WrapLayoutH::MeasureOverride(const Size &availableSize)
{
    Size size{};
    int count = this->GetChildLayoutCount();

    if (std::isinf(availableSize.width)) {
        for (int i = 0; i < count; ++i) {
            ILayout &item = this->GetChildLayoutAt(i);
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
            ILayout &item = this->GetChildLayoutAt(i);
            item.Measure(Size{availableSize.width - rowWidth, INFINITY});

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

    int count = this->GetChildLayoutCount();

    std::vector<std::tuple<ILayout *, Size>> row;
    row.reserve(count);

    for (int i = 0; i < count; ++i) {
        ILayout &item       = this->GetChildLayoutAt(i);
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
