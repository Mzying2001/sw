#include "DockPanel.h"

sw::DockPanel::DockPanel()
    : LastChildFill(
          // get
          [&]() -> const bool & {
              return this->_dockLayout.lastChildFill;
          },
          // set
          [&](const bool &value) {
              this->_dockLayout.lastChildFill = value;
              this->NotifyLayoutUpdated();
          })
{
    this->Layout = &this->_dockLayout;
}
