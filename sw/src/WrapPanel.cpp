#include "WrapPanel.h"

sw::WrapPanel::WrapPanel()
    : Orientation(
          // get
          [this]() -> sw::Orientation {
              return this->_wrapLayout.orientation;
          },
          // set
          [this](const sw::Orientation &value) {
              this->_wrapLayout.orientation = value;
              this->NotifyLayoutUpdated();
          })
{
    this->_wrapLayout.Associate(this);
    this->HorizontalAlignment = HorizontalAlignment::Stretch;
    this->VerticalAlignment   = VerticalAlignment::Stretch;
}

sw::LayoutHost *sw::WrapPanel::GetDefaultLayout()
{
    return &this->_wrapLayout;
}
