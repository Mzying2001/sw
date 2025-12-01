#include "UniformGrid.h"

sw::UniformGrid::UniformGrid()
    : Rows(
          Property<int>::Init(this)
              .Getter([](UniformGrid *self) -> int {
                  return self->_uniformGridLayout.rows;
              })
              .Setter([](UniformGrid *self, int value) {
                  if (self->_uniformGridLayout.rows != value) {
                      self->_uniformGridLayout.rows = value;
                      self->InvalidateMeasure();
                  }
              })),

      Columns(
          Property<int>::Init(this)
              .Getter([](UniformGrid *self) -> int {
                  return self->_uniformGridLayout.columns;
              })
              .Setter([](UniformGrid *self, int value) {
                  if (self->_uniformGridLayout.columns != value) {
                      self->_uniformGridLayout.columns = value;
                      self->InvalidateMeasure();
                  }
              })),

      FirstColumn(
          Property<int>::Init(this)
              .Getter([](UniformGrid *self) -> int {
                  return self->_uniformGridLayout.firstColumn;
              })
              .Setter([](UniformGrid *self, int value) {
                  if (self->_uniformGridLayout.firstColumn != value) {
                      self->_uniformGridLayout.firstColumn = value;
                      self->InvalidateMeasure();
                  }
              }))
{
    this->_uniformGridLayout.Associate(this);
    this->HorizontalAlignment = HorizontalAlignment::Stretch;
    this->VerticalAlignment   = VerticalAlignment::Stretch;
}

sw::LayoutHost *sw::UniformGrid::GetDefaultLayout()
{
    return &this->_uniformGridLayout;
}
