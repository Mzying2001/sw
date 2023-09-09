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

sw::DockLayout::DockLayoutTag sw::DockPanel::GetDock(UIElement &element)
{
    return (DockLayout::DockLayoutTag)element.LayoutTag.Get();
}

void sw::DockPanel::SetDock(UIElement &element, DockLayout::DockLayoutTag dock)
{
    element.LayoutTag = dock;
}
