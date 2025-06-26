#include "DockPanel.h"

sw::DockPanel::DockPanel()
    : LastChildFill(
          // get
          [this]() -> bool {
              return this->_dockLayout.lastChildFill;
          },
          // set
          [this](const bool &value) {
              this->_dockLayout.lastChildFill = value;
              this->InvalidateMeasure();
          })
{
    this->_dockLayout.Associate(this);
    this->HorizontalAlignment = HorizontalAlignment::Stretch;
    this->VerticalAlignment   = VerticalAlignment::Stretch;
}

sw::DockLayoutTag sw::DockPanel::GetDock(UIElement &element)
{
    return DockLayoutTag{element.LayoutTag.Get()};
}

void sw::DockPanel::SetDock(UIElement &element, DockLayoutTag dock)
{
    element.LayoutTag = dock;
}

sw::LayoutHost *sw::DockPanel::GetDefaultLayout()
{
    return &this->_dockLayout;
}
