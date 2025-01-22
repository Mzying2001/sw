#include "StackPanel.h"

sw::StackPanel::StackPanel()
    : Orientation(
          // get
          [this]() -> sw::Orientation {
              return this->_stackLayout.orientation;
          },
          // set
          [this](const sw::Orientation &value) {
              this->_stackLayout.orientation = value;
              this->NotifyLayoutUpdated();
          })
{
    this->_stackLayout.Associate(this);
    this->HorizontalAlignment = HorizontalAlignment::Stretch;
    this->VerticalAlignment   = VerticalAlignment::Stretch;
}

sw::LayoutHost *sw::StackPanel::GetDefaultLayout()
{
    return &this->_stackLayout;
}
