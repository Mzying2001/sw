#include "Grid.h"

sw::Grid::Grid()
    : Rows(
          Property<ObservableCollection<GridRow> *>::Init(this)
              .Getter([](Grid *self) -> ObservableCollection<GridRow> * {
                  return &self->_gridLayout.rows;
              })),

      Columns(
          Property<ObservableCollection<GridColumn> *>::Init(this)
              .Getter([](Grid *self) -> ObservableCollection<GridColumn> * {
                  return &self->_gridLayout.columns;
              }))
{
    _gridLayout.Associate(this);

    _gridLayout.rows.CollectionChanged +=
        NotifyCollectionChangedEventHandler(*this, &Grid::_RowsColumnsCollectionChangedHandler);

    _gridLayout.columns.CollectionChanged +=
        NotifyCollectionChangedEventHandler(*this, &Grid::_RowsColumnsCollectionChangedHandler);

    HorizontalAlignment = HorizontalAlignment::Stretch;
    VerticalAlignment   = VerticalAlignment::Stretch;
}

void sw::Grid::SetRows(std::initializer_list<GridRow> rows)
{
    _gridLayout.rows.GetInternalVector().assign(rows);
    InvalidateMeasure();
}

void sw::Grid::SetColumns(std::initializer_list<GridColumn> cols)
{
    _gridLayout.columns.GetInternalVector().assign(cols);
    InvalidateMeasure();
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
    return &_gridLayout;
}

void sw::Grid::_RowsColumnsCollectionChangedHandler(
    INotifyCollectionChanged &sender, NotifyCollectionChangedEventArgs &args)
{
    InvalidateMeasure();
}
