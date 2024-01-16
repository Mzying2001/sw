#include "GridLayout.h"
#include "Utils.h"

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

void sw::GridLayout::MeasureOverride(Size &availableSize)
{
    Size desireSize{};
    this->_UpdateInternalData();

    int rowCount   = (int)this->_internalData.rowsInfo.size();
    int colCount   = (int)this->_internalData.colsInfo.size();
    int childCount = (int)this->_internalData.childrenInfo.size();

    // TODO

    // Measure列

    // Measure行

    // 设置DesireSize
    this->SetDesireSize(desireSize);
}

void sw::GridLayout::ArrangeOverride(Size &finalSize)
{
    // TODO
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
            this->_internalData.rowsInfo.emplace_back(info);
        } else {
            for (GridRow &row : this->rows) {
                info.row = row;
                if (info.row.height < 0)
                    info.row.height = 0;
                this->_internalData.rowsInfo.emplace_back(info);
            }
        }
    }

    // colsInfo
    {
        _ColInfo info;

        if (this->columns.IsEmpty()) {
            this->_internalData.colsInfo.emplace_back(info);
        } else {
            for (GridColumn &col : this->columns) {
                info.col = col;
                if (info.col.width < 0)
                    info.col.width = 0;
                this->_internalData.colsInfo.emplace_back(info);
            }
        }
    }

    // childrenInfo
    {
        int rowCount   = (int)this->_internalData.rowsInfo.size();
        int colCount   = (int)this->_internalData.colsInfo.size();
        int childCount = this->GetChildLayoutCount();

        for (int i = 0; i < childCount; ++i) {
            ILayout &item     = this->GetChildLayoutAt(i);
            GridLayoutTag tag = item.GetLayoutTag();

            tag.row    = Utils::Min<uint16_t>(rowCount - 1, tag.row);
            tag.column = Utils::Min<uint16_t>(colCount - 1, tag.column);

            if (tag.row + tag.rowSpan > rowCount) {
                tag.rowSpan = rowCount - tag.row;
            }
            if (tag.column + tag.columnSpan > colCount) {
                tag.columnSpan = colCount - tag.column;
            }

            this->_internalData.childrenInfo.push_back({&item, tag});
        }
    }
}
