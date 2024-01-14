#include "Grid.h"

sw::Grid::Grid()
{
    this->_gridLayout.Associate(this);
    this->HorizontalAlignment = HorizontalAlignment::Stretch;
    this->VerticalAlignment   = VerticalAlignment::Stretch;
}

sw::LayoutHost *sw::Grid::GetDefaultLayout()
{
    return &this->_gridLayout;
}
