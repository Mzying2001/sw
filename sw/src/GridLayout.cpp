#include "GridLayout.h"
#include "Utils.h"
#include <algorithm>
#include <cmath>

sw::GridLayoutTag::GridLayoutTag()
    : row(0), column(0), rowSpan(1), columnSpan(1)
{
}

sw::GridLayoutTag::GridLayoutTag(uint16_t row, uint16_t column, uint16_t rowSpan, uint16_t columnSpan)
    : row(row), column(column), rowSpan(rowSpan), columnSpan(columnSpan)
{
}

sw::GridLayoutTag::GridLayoutTag(uint64_t layoutTag)
    : row((layoutTag >> 0) & 0xffff), column((layoutTag >> 16) & 0xffff),
      rowSpan((layoutTag >> 32) & 0xffff), columnSpan((layoutTag >> 48) & 0xffff)
{
}

sw::GridLayoutTag::operator uint64_t() const
{
    return ((uint64_t)(this->row) << 0) |
           ((uint64_t)(this->column) << 16) |
           ((uint64_t)(this->rowSpan) << 32) |
           ((uint64_t)(this->columnSpan) << 48);
}

sw::GridRow::GridRow()
    : type(GridRCType::FillRemain), height(1)
{
}

sw::GridRow::GridRow(GridRCType type, double height)
    : type(type), height(height)
{
}

sw::GridRow::GridRow(double height)
    : type(GridRCType::FixSize), height(height)
{
}

sw::FixSizeGridRow::FixSizeGridRow(double height)
    : GridRow(GridRCType::FixSize, height)
{
}

sw::AutoSizeGridRow::AutoSizeGridRow()
    : GridRow(GridRCType::AutoSize, 0)
{
}

sw::FillRemainGridRow::FillRemainGridRow(double proportion)
    : GridRow(GridRCType::FillRemain, proportion)
{
}

sw::GridColumn::GridColumn()
    : type(GridRCType::FillRemain), width(1)
{
}

sw::GridColumn::GridColumn(GridRCType type, double width)
    : type(type), width(width)
{
}

sw::GridColumn::GridColumn(double width)
    : type(GridRCType::FixSize), width(width)
{
}

sw::FixSizeGridColumn::FixSizeGridColumn(double width)
    : GridColumn(GridRCType::FixSize, width)
{
}

sw::AutoSizeGridColumn::AutoSizeGridColumn()
    : GridColumn(GridRCType::AutoSize, 0)
{
}

sw::FillRemainGridColumn::FillRemainGridColumn(double proportion)
    : GridColumn(GridRCType::FillRemain, proportion)
{
}

void sw::GridLayout::MeasureOverride(Size &availableSize)
{
    Size desireSize{};
    this->_UpdateInternalData();

    int rowCount   = (int)this->_internalData.rowsInfo.size();
    int colCount   = (int)this->_internalData.colsInfo.size();
    int childCount = (int)this->_internalData.childrenInfo.size();

    bool widthSizeToContent  = std::isinf(availableSize.width);
    bool heightSizeToContent = std::isinf(availableSize.height);

    // 子元素为空，DesireSize只受类型为FixSize的行列影响
    if (childCount == 0) {
        for (_ColInfo &colInfo : this->_internalData.colsInfo) {
            if (colInfo.col.type == GridRCType::FixSize)
                desireSize.width += colInfo.size;
        }
        for (_RowInfo &rowInfo : this->_internalData.rowsInfo) {
            if (rowInfo.row.type == GridRCType::FixSize)
                desireSize.height += rowInfo.size;
        }
        this->SetDesireSize(desireSize);
        return;
    }

    // Measure列

    // 对childrenInfo进行排序
    // 优先级 FixSize > AutoSize > FillRemain
    // 对于优先级相同的则按照columnSpan从小到大排序
    std::sort(
        this->_internalData.childrenInfo.begin(),
        this->_internalData.childrenInfo.end(),
        [](const _ChildInfo &a, const _ChildInfo &b) -> bool {
            if (a.colMeasureType != b.colMeasureType) {
                // 先FixSize，后AutoSize，最后FillRemain
                return a.colMeasureType < b.colMeasureType;
            } else {
                // 若类型相同，则按照跨列数从小到大measure
                return a.layoutTag.columnSpan < b.layoutTag.columnSpan;
            }
        });

    for (_ChildInfo &childInfo : this->_internalData.childrenInfo) {
        bool breakFlag = false; // 标记是否退出循环

        switch (childInfo.colMeasureType) {
            case GridRCType::FixSize: {
                // 第一次measure目的是确定列宽度，因此FixSize列的元素没必要在此时measure
                break;
            }

            case GridRCType::AutoSize: {
                childInfo.instance->Measure(Size(INFINITY, INFINITY));
                Size childDesireSize = childInfo.instance->GetDesireSize();

                if (childInfo.layoutTag.columnSpan <= 1) {
                    // 若ColumnSpan为1，直接更新对应列的尺寸
                    this->_internalData.colsInfo[childInfo.layoutTag.column].size =
                        Utils::Max(this->_internalData.colsInfo[childInfo.layoutTag.column].size, childDesireSize.width);
                } else {
                    // 若ColumnSpan不为1，则对所跨类型为AutoSize的列均匀贡献
                    int count  = 0;
                    double sum = 0;

                    for (int i = 0; i < childInfo.layoutTag.columnSpan; ++i) {
                        _ColInfo &colInfo =
                            this->_internalData.colsInfo[childInfo.layoutTag.column + i];
                        count += colInfo.col.type == GridRCType::AutoSize;
                        sum += colInfo.size;
                    }

                    if (sum < childDesireSize.width) {
                        double tmp =
                            (childDesireSize.width - sum) / count;
                        for (int i = 0; i < childInfo.layoutTag.columnSpan; ++i) {
                            _ColInfo &colInfo =
                                this->_internalData.colsInfo[childInfo.layoutTag.column + i];
                            if (colInfo.col.type == GridRCType::AutoSize) {
                                colInfo.size += tmp;
                            }
                        }
                    }
                }
                break;
            }

            case GridRCType::FillRemain: {
                if (!widthSizeToContent) {
                    // 若宽度不是由内容确定（availableSize.width为inf），则类型为FillRemain的列宽与内容无关
                    // 此时可以直接根据剩余的大小计算，直接退出循环
                    breakFlag = true;
                    break;
                }

                // 宽度由内容决定，对元素进行measure，以占空间最大的元素为基准分配大小
                childInfo.instance->Measure(Size(INFINITY, INFINITY));
                Size childDesireSize = childInfo.instance->GetDesireSize();

                if (childInfo.layoutTag.columnSpan <= 1) {
                    // 若ColumnSpan为1，直接更新对应列的尺寸
                    this->_internalData.colsInfo[childInfo.layoutTag.column].size =
                        Utils::Max(this->_internalData.colsInfo[childInfo.layoutTag.column].size, childDesireSize.width);
                } else {
                    // 若ColumnSpan不为1，则对所跨类型为FillRemain的列均匀贡献
                    int count  = 0;
                    double sum = 0;

                    for (int i = 0; i < childInfo.layoutTag.columnSpan; ++i) {
                        _ColInfo &colInfo =
                            this->_internalData.colsInfo[childInfo.layoutTag.column + i];
                        count += colInfo.col.type == GridRCType::FillRemain;
                        sum += colInfo.size;
                    }

                    if (sum < childDesireSize.width) {
                        double tmp =
                            (childDesireSize.width - sum) / count;
                        for (int i = 0; i < childInfo.layoutTag.columnSpan; ++i) {
                            _ColInfo &colInfo =
                                this->_internalData.colsInfo[childInfo.layoutTag.column + i];
                            if (colInfo.col.type == GridRCType::FillRemain) {
                                colInfo.size += tmp;
                            }
                        }
                    }
                }
                break;
            }
        }

        if (breakFlag) {
            break;
        }
    }

    if (!widthSizeToContent) {
        // 宽度不是由内容决定时计算类型为FillRemain的列的宽度
        double remainWidth = availableSize.width;
        for (_ColInfo &colInfo : this->_internalData.colsInfo) {
            if (colInfo.col.type != GridRCType::FillRemain) {
                remainWidth -= colInfo.size;
            }
        }
        if (remainWidth < 0) {
            remainWidth = 0;
        }
        for (_ColInfo &colInfo : this->_internalData.colsInfo) {
            if (colInfo.col.type == GridRCType::FillRemain) {
                colInfo.size = remainWidth * colInfo.proportion;
            }
        }
    } else {
        // 宽度由内容决定，依据所占宽度最大元素为基准计算列宽度
        _ColInfo *maxWidthCol = nullptr;
        for (_ColInfo &colInfo : this->_internalData.colsInfo) {
            if (colInfo.col.type == GridRCType::FillRemain &&
                (maxWidthCol == nullptr || colInfo.size > maxWidthCol->size)) {
                maxWidthCol = &colInfo;
            }
        }
        if (maxWidthCol != nullptr) {
            double tmp =
                maxWidthCol->size / maxWidthCol->proportion;
            for (_ColInfo &colInfo : this->_internalData.colsInfo) {
                if (colInfo.col.type == GridRCType::FillRemain) {
                    colInfo.size = tmp * colInfo.proportion;
                }
            }
        }
    }

    // Measure行

    // 对childrenInfo进行排序
    // 优先级 FixSize > AutoSize > FillRemain
    // 对于优先级相同的则按照rowSpan从小到大排序
    std::sort(
        this->_internalData.childrenInfo.begin(),
        this->_internalData.childrenInfo.end(),
        [](const _ChildInfo &a, const _ChildInfo &b) -> bool {
            if (a.rowMeasureType != b.rowMeasureType) {
                // 先FixSize，后AutoSize，最后FillRemain
                return a.rowMeasureType < b.rowMeasureType;
            } else {
                // 若类型相同，则按照跨列数从小到大measure
                return a.layoutTag.rowSpan < b.layoutTag.rowSpan;
            }
        });

    int measureIndex = 0;

    // Measure行类型为FixSize的元素
    while (measureIndex < childCount &&
           this->_internalData.childrenInfo[measureIndex].rowMeasureType == GridRCType::FixSize) {
        _ChildInfo &childInfo = this->_internalData.childrenInfo[measureIndex++];

        Size measureSize{};
        for (int i = 0; i < childInfo.layoutTag.columnSpan; ++i)
            measureSize.width += this->_internalData.colsInfo[childInfo.layoutTag.column + i].size;
        for (int i = 0; i < childInfo.layoutTag.rowSpan; ++i)
            measureSize.height += this->_internalData.rowsInfo[childInfo.layoutTag.row + i].size;
        childInfo.instance->Measure(measureSize);
    }

    // Measure行类型为AutoSize的元素
    while (measureIndex < childCount &&
           this->_internalData.childrenInfo[measureIndex].rowMeasureType == GridRCType::AutoSize) {
        _ChildInfo &childInfo = this->_internalData.childrenInfo[measureIndex++];

        Size measureSize{0, INFINITY};
        for (int i = 0; i < childInfo.layoutTag.columnSpan; ++i) {
            measureSize.width += this->_internalData.colsInfo[childInfo.layoutTag.column + i].size;
        }

        childInfo.instance->Measure(measureSize);
        Size childDesireSize = childInfo.instance->GetDesireSize();

        if (childInfo.layoutTag.rowSpan <= 1) {
            // 若RowSpan为1，直接更新对应行的尺寸
            this->_internalData.rowsInfo[childInfo.layoutTag.row].size =
                Utils::Max(this->_internalData.rowsInfo[childInfo.layoutTag.row].size, childDesireSize.height);
        } else {
            // 若RowSpan不为1，则对所跨类型为AutoSize的行均匀贡献
            int count  = 0;
            double sum = 0;

            for (int i = 0; i < childInfo.layoutTag.rowSpan; ++i) {
                _RowInfo &rowInfo =
                    this->_internalData.rowsInfo[childInfo.layoutTag.row + i];
                count += rowInfo.row.type == GridRCType::AutoSize;
                sum += rowInfo.size;
            }

            if (sum < childDesireSize.height) {
                double tmp =
                    (childDesireSize.height - sum) / count;
                for (int i = 0; i < childInfo.layoutTag.rowSpan; ++i) {
                    _RowInfo &rowInfo =
                        this->_internalData.rowsInfo[childInfo.layoutTag.row + i];
                    if (rowInfo.row.type == GridRCType::AutoSize) {
                        rowInfo.size += tmp;
                    }
                }
            }
        }
    }

    // Measure行类型为FillRemain的元素
    if (measureIndex < childCount) {
        if (heightSizeToContent) {
            // 高度由内容决定，依据所占宽度最大元素为基准计算列宽度
            while (measureIndex < childCount) {
                _ChildInfo &childInfo = this->_internalData.childrenInfo[measureIndex++];

                Size measureSize{0, INFINITY};
                for (int i = 0; i < childInfo.layoutTag.columnSpan; ++i)
                    measureSize.width += this->_internalData.colsInfo[childInfo.layoutTag.column + i].size;

                childInfo.instance->Measure(measureSize);
                Size childDesireSize = childInfo.instance->GetDesireSize();

                if (childInfo.layoutTag.rowSpan <= 1) {
                    // 若RowSpan为1，直接更新对应列的尺寸
                    this->_internalData.rowsInfo[childInfo.layoutTag.row].size =
                        Utils::Max(this->_internalData.rowsInfo[childInfo.layoutTag.row].size, childDesireSize.height);
                } else {
                    // 若RowSpan不为1，则对所跨类型为FillRemain的列均匀贡献
                    int count  = 0;
                    double sum = 0;

                    for (int i = 0; i < childInfo.layoutTag.rowSpan; ++i) {
                        _RowInfo &rowInfo =
                            this->_internalData.rowsInfo[childInfo.layoutTag.row + i];
                        count += rowInfo.row.type == GridRCType::FillRemain;
                        sum += rowInfo.size;
                    }

                    if (sum < childDesireSize.height) {
                        double tmp =
                            (childDesireSize.height - sum) / count;
                        for (int i = 0; i < childInfo.layoutTag.rowSpan; ++i) {
                            _RowInfo &rowInfo =
                                this->_internalData.rowsInfo[childInfo.layoutTag.row + i];
                            if (rowInfo.row.type == GridRCType::FillRemain) {
                                rowInfo.size += tmp;
                            }
                        }
                    }
                }
            }

            // 高度由内容决定，依据所占高度最大元素为基准计算行高度
            _RowInfo *maxHeightRow = nullptr;
            for (_RowInfo &rowInfo : this->_internalData.rowsInfo) {
                if (rowInfo.row.type == GridRCType::FillRemain &&
                    (maxHeightRow == nullptr || rowInfo.size > maxHeightRow->size)) {
                    maxHeightRow = &rowInfo;
                }
            }
            if (maxHeightRow != nullptr) {
                double tmp =
                    maxHeightRow->size / maxHeightRow->proportion;
                for (_RowInfo &rowInfo : this->_internalData.rowsInfo) {
                    if (rowInfo.row.type == GridRCType::FillRemain) {
                        rowInfo.size = tmp * rowInfo.proportion;
                    }
                }
            }
        } else {
            // 高度不是由内容决定时计算类型为FillRemain的列的宽度
            double remainHeight = availableSize.height;
            for (_RowInfo &rowInfo : this->_internalData.rowsInfo) {
                if (rowInfo.row.type != GridRCType::FillRemain) {
                    remainHeight -= rowInfo.size;
                }
            }
            if (remainHeight < 0) {
                remainHeight = 0;
            }
            for (_RowInfo &rowInfo : this->_internalData.rowsInfo) {
                if (rowInfo.row.type == GridRCType::FillRemain) {
                    rowInfo.size = remainHeight * rowInfo.proportion;
                }
            }
            // Measure
            while (measureIndex < childCount) {
                _ChildInfo &childInfo = this->_internalData.childrenInfo[measureIndex++];

                Size measureSize{};
                for (int i = 0; i < childInfo.layoutTag.columnSpan; ++i)
                    measureSize.width += this->_internalData.colsInfo[childInfo.layoutTag.column + i].size;
                for (int i = 0; i < childInfo.layoutTag.rowSpan; ++i)
                    measureSize.height += this->_internalData.rowsInfo[childInfo.layoutTag.row + i].size;
                childInfo.instance->Measure(measureSize);
            }
        }
    }

    // 更新网格信息
    {
        double left = 0;
        double top  = 0;

        for (int i = 0; i < rowCount; ++i) {
            _RowInfo &rowInfo = this->_internalData.rowsInfo[i];
            for (int j = 0; j < colCount; ++j) {
                _ColInfo &colInfo = this->_internalData.colsInfo[j];
                Rect &cell        = this->_GetCell(i, j);
                cell.left         = left;
                cell.top          = top;
                cell.width        = colInfo.size;
                cell.height       = rowInfo.size;
                left += colInfo.size;
            }
            left = 0;
            top += rowInfo.size;
        }
    }

    // 设置DesireSize
    for (_ColInfo &colInfo : this->_internalData.colsInfo)
        desireSize.width += colInfo.size;
    for (_RowInfo &rowInfo : this->_internalData.rowsInfo)
        desireSize.height += rowInfo.size;
    this->SetDesireSize(desireSize);
}

void sw::GridLayout::ArrangeOverride(Size &finalSize)
{
    int childCount = (int)this->_internalData.childrenInfo.size();
    if (childCount == 0) return;

    for (_ChildInfo &childInfo : this->_internalData.childrenInfo) {
        int row = childInfo.layoutTag.row;
        int col = childInfo.layoutTag.column;

        Rect arrangeRect = this->_GetCell(row, col);
        for (int i = 1; i < childInfo.layoutTag.columnSpan; ++i)
            arrangeRect.width += this->_GetCell(row, col + i).width;
        for (int i = 1; i < childInfo.layoutTag.rowSpan; ++i)
            arrangeRect.height += this->_GetCell(row + i, col).height;

        childInfo.instance->Arrange(arrangeRect);
    }
}

void sw::GridLayout::_UpdateInternalData()
{
    this->_internalData.rowsInfo.clear();
    this->_internalData.colsInfo.clear();
    this->_internalData.childrenInfo.clear();

    // rowsInfo
    {
        _RowInfo info;

        if (this->rows.IsEmpty()) {
            info.proportion = 1;
            this->_internalData.rowsInfo.emplace_back(info);
        } else {
            int count  = 0; // 类型为FillRemain的行数
            double sum = 0; // FillRemain行的高度求和

            for (GridRow &row : this->rows) {
                info.row = row;
                // 防止值小于0
                if (info.row.height < 0) {
                    info.row.height = 0;
                }
                if (info.row.type == GridRCType::FixSize) {
                    // 若类型为FixSize，此时即可确定行高
                    info.size = info.row.height;
                } else {
                    // 其他类型此时无法确定行高
                    info.size = 0;
                }
                // 记录FillRemain类型行的信息，用于计算percentage
                if (info.row.type == GridRCType::FillRemain) {
                    ++count;
                    sum += info.row.height;
                }
                this->_internalData.rowsInfo.emplace_back(info);
            }

            // 设置percentage字段
            if (count && sum > 0) {
                for (_RowInfo &rowInfo : this->_internalData.rowsInfo) {
                    if (rowInfo.row.type == GridRCType::FillRemain) {
                        rowInfo.proportion = rowInfo.row.height / sum;
                    }
                }
            }
        }
    }

    // colsInfo
    {
        _ColInfo info;

        if (this->columns.IsEmpty()) {
            info.proportion = 1;
            this->_internalData.colsInfo.emplace_back(info);
        } else {
            int count  = 0; // 类型为FillRemain的列数
            double sum = 0; // FillRemain列的高度求和

            for (GridColumn &col : this->columns) {
                info.col = col;
                // 防止值小于0
                if (info.col.width < 0) {
                    info.col.width = 0;
                }
                if (info.col.type == GridRCType::FixSize) {
                    // 若类型为FixSize，此时即可确定列宽
                    info.size = info.col.width;
                } else {
                    // 其他类型此时无法确定列宽
                    info.size = 0;
                }
                // 记录FillRemain类型列的信息，用于计算percentage
                if (info.col.type == GridRCType::FillRemain) {
                    ++count;
                    sum += info.col.width;
                }
                this->_internalData.colsInfo.emplace_back(info);
            }

            // 设置percentage字段
            if (count && sum > 0) {
                for (_ColInfo &colInfo : this->_internalData.colsInfo) {
                    if (colInfo.col.type == GridRCType::FillRemain) {
                        colInfo.proportion = colInfo.col.width / sum;
                    }
                }
            }
        }
    }

    // childrenInfo
    {
        int rowCount   = (int)this->_internalData.rowsInfo.size();
        int colCount   = (int)this->_internalData.colsInfo.size();
        int childCount = this->GetChildLayoutCount();

        _ChildInfo info;

        for (int i = 0; i < childCount; ++i) {
            ILayout &item     = this->GetChildLayoutAt(i);
            GridLayoutTag tag = item.GetLayoutTag();

            // 确保row和column的值不会超过网格的大小
            // 由于类型是uint16_t，不存在值小于0的情况
            tag.row    = Utils::Min<uint16_t>(rowCount - 1, tag.row);
            tag.column = Utils::Min<uint16_t>(colCount - 1, tag.column);

            // 确保rowSpan的值合理
            if (tag.rowSpan == 0) {
                tag.rowSpan = 1;
            } else if (tag.row + tag.rowSpan > rowCount) {
                tag.rowSpan = rowCount - tag.row;
            }
            // 确保colSpan的值合理
            if (tag.columnSpan == 0) {
                tag.columnSpan = 1;
            } else if (tag.column + tag.columnSpan > colCount) {
                tag.columnSpan = colCount - tag.column;
            }

            // 子元素在measure时的行列类型，这个值受其所在的位置影响
            // 若span为1，则该值与其所在的行/列的类型相同，否则由所跨所有的行/列共同决定：
            // - 若所有行/列均为FixSize，则为FixSize
            // - 若行/列中存在AutoSize，且没有FillRemain，则为AutoSize
            // - 若行/列中存在FillRemain，则为FillRemain
            GridRCType rowMeasureType = this->_internalData.rowsInfo[tag.row].row.type;
            GridRCType colMeasureType = this->_internalData.colsInfo[tag.column].col.type;

            // 处理跨多行情况下的rowMeasureType
            for (int i = 1; i < tag.rowSpan && rowMeasureType != GridRCType::FillRemain; ++i) {
                GridRCType type =
                    this->_internalData.rowsInfo[tag.row + i].row.type;
                switch (type) {
                    case GridRCType::FixSize: {
                        break; // nothing
                    }
                    case GridRCType::AutoSize:
                    case GridRCType::FillRemain: {
                        rowMeasureType = type;
                    }
                }
            }
            // 处理跨多列情况下的colMeasureType
            for (int i = 1; i < tag.columnSpan && colMeasureType != GridRCType::FillRemain; ++i) {
                GridRCType type =
                    this->_internalData.colsInfo[tag.column + i].col.type;
                switch (type) {
                    case GridRCType::FixSize: {
                        break; // nothing
                    }
                    case GridRCType::AutoSize:
                    case GridRCType::FillRemain: {
                        colMeasureType = type;
                    }
                }
            }

            info.instance       = &item;
            info.layoutTag      = tag;
            info.rowMeasureType = rowMeasureType;
            info.colMeasureType = colMeasureType;

            this->_internalData.childrenInfo.emplace_back(info);
        }
    }

    // cells
    {
        this->_internalData.cells.resize(this->_internalData.rowsInfo.size() * this->_internalData.colsInfo.size());
    }
}

sw::Rect &sw::GridLayout::_GetCell(int row, int col)
{
    return this->_internalData.cells[this->_internalData.colsInfo.size() * row + col];
}
