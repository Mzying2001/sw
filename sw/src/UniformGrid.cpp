#include "UniformGrid.h"

sw::UniformGrid::UniformGrid()
    : Rows(
          // get
          [&]() -> const int & {
              return this->_uniformGridLayout.rows;
          },
          // set
          [&](const int &value) {
              this->_uniformGridLayout.rows = value;
              this->NotifyLayoutUpdated();
          }),

      Columns(
          // get
          [&]() -> const int & {
              return this->_uniformGridLayout.columns;
          },
          // set
          [&](const int &value) {
              this->_uniformGridLayout.columns = value;
              this->NotifyLayoutUpdated();
          }),

      FirstColumn(
          // get
          [&]() -> const int & {
              return this->_uniformGridLayout.firstColumn;
          },
          // set
          [&](const int &value) {
              this->_uniformGridLayout.firstColumn = value;
              this->NotifyLayoutUpdated();
          })
{
    this->_uniformGridLayout.Associate(this);
    this->HorizontalAlignment = HorizontalAlignment::Stretch;
    this->VerticalAlignment   = VerticalAlignment::Stretch;
}

sw::LayoutHost *sw::UniformGrid::GetDefaultLayout()
{
    return &this->_uniformGridLayout;
}
