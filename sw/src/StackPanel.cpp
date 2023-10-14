#include "StackPanel.h"

sw::StackPanel::StackPanel()
    : Orientation(
          // get
          [&]() -> const sw::Orientation & {
              return this->_stackLayout.orientation;
          },
          // set
          [&](const sw::Orientation &value) {
              this->_stackLayout.orientation = value;
              this->NotifyLayoutUpdated();
          })
{
    this->_stackLayout.Associate(this);
}

sw::LayoutHost *sw::StackPanel::GetDefaultLayout()
{
    return &this->_stackLayout;
}
