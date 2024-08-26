#include "UniformGrid.h"

sw::UniformGrid::UniformGrid()
    : Rows(
          // get
          [this]() -> int {
              return this->_uniformGridLayout.rows;
          },
          // set
          [this](const int &value) {
              this->_uniformGridLayout.rows = value;
              this->NotifyLayoutUpdated();
          }),

      Columns(
          // get
          [this]() -> int {
              return this->_uniformGridLayout.columns;
          },
          // set
          [this](const int &value) {
              this->_uniformGridLayout.columns = value;
              this->NotifyLayoutUpdated();
          }),

      FirstColumn(
          // get
          [this]() -> int {
              return this->_uniformGridLayout.firstColumn;
          },
          // set
          [this](const int &value) {
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
