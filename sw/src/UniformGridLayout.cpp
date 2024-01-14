#include "UniformGridLayout.h"
#include "Utils.h"
#include <cmath>

void sw::UniformGridLayout::MeasureOverride(Size &availableSize)
{
    int rowCount = Utils::Max(1, this->rows);
    int colCount = Utils::Max(1, this->columns);

    double itemMaxDesireWidth  = 0;
    double itemMaxDesireHeight = 0;

    int childCount = this->GetChildLayoutCount();
    Size measureSize{availableSize.width / colCount, availableSize.height / rowCount};

    for (int i = 0; i < childCount; ++i) {
        ILayout &item = this->GetChildLayoutAt(i);
        item.Measure(measureSize);
        Size itemDesireSize = item.GetDesireSize();
        itemMaxDesireWidth  = Utils::Max(itemDesireSize.width, itemMaxDesireWidth);
        itemMaxDesireHeight = Utils::Max(itemDesireSize.height, itemMaxDesireHeight);
    }

    this->SetDesireSize(Size(
        std::isinf(availableSize.width) ? (itemMaxDesireWidth * colCount) : (availableSize.width),
        std::isinf(availableSize.height) ? (itemMaxDesireHeight * rowCount) : (availableSize.height)));
}

void sw::UniformGridLayout::ArrangeOverride(Size &finalSize)
{
    int rowCount = Utils::Max(1, this->rows);
    int colCount = Utils::Max(1, this->columns);
    int beginCol = Utils::Max(0, Utils::Min(rowCount - 1, this->firstColumn));

    double arrangeWidth  = finalSize.width / colCount;
    double arrangeHeight = finalSize.height / rowCount;

    int childCount = this->GetChildLayoutCount();

    for (int i = 0; i < childCount; ++i) {
        int arrangeRow = (i + beginCol) / colCount;
        int arrangeCol = (i + beginCol) % colCount;

        if (arrangeRow >= rowCount) {
            arrangeRow = rowCount - 1;
            arrangeCol = colCount - 1;
        }

        ILayout &item = this->GetChildLayoutAt(i);
        item.Arrange(Rect(arrangeCol * arrangeWidth, arrangeRow * arrangeHeight, arrangeWidth, arrangeHeight));
    }
}
