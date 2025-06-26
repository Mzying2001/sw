#include "Grid.h"

sw::Grid::Grid()
{
    this->_gridLayout.Associate(this);
    this->HorizontalAlignment = HorizontalAlignment::Stretch;
    this->VerticalAlignment   = VerticalAlignment::Stretch;
}

void sw::Grid::AddRow(const GridRow &row)
{
    this->_gridLayout.rows.Append(row);
    this->InvalidateMeasure();
}

void sw::Grid::SetRows(std::initializer_list<GridRow> rows)
{
    List<GridRow> rowsList = rows;
    this->_gridLayout.rows = rowsList;
    this->InvalidateMeasure();
}

void sw::Grid::AddColumn(const GridColumn &col)
{
    this->_gridLayout.columns.Append(col);
    this->InvalidateMeasure();
}

void sw::Grid::SetColumns(std::initializer_list<GridColumn> cols)
{
    List<GridColumn> colsList = cols;
    this->_gridLayout.columns = colsList;
    this->InvalidateMeasure();
}

void sw::Grid::ClearRows()
{
    this->_gridLayout.rows.Clear();
    this->InvalidateMeasure();
}

void sw::Grid::ClearColumns()
{
    this->_gridLayout.columns.Clear();
    this->InvalidateMeasure();
}

sw::GridLayoutTag sw::Grid::GetGridLayoutTag(UIElement &element)
{
    return element.LayoutTag.Get();
}

void sw::Grid::SetGridLayoutTag(UIElement &element, const GridLayoutTag &tag)
{
    element.LayoutTag.Set(tag);
}

sw::LayoutHost *sw::Grid::GetDefaultLayout()
{
    return &this->_gridLayout;
}
