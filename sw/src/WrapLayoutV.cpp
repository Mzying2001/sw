#include "WrapLayoutV.h"
#include "Utils.h"
#include <cmath>
#include <tuple>
#include <vector>

namespace
{
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

sw::Size sw::WrapLayoutV::MeasureOverride(const Size &availableSize)
{
    Size size{};
    int count = this->GetChildLayoutCount();

    if (std::isinf(availableSize.height)) {
        for (int i = 0; i < count; ++i) {
            ILayout &item = this->GetChildLayoutAt(i);
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
            ILayout &item = this->GetChildLayoutAt(i);
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

    int count = this->GetChildLayoutCount();

    std::vector<std::tuple<ILayout *, Size>> col;
    col.reserve(count);

    for (int i = 0; i < count; ++i) {
        ILayout &item       = this->GetChildLayoutAt(i);
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
