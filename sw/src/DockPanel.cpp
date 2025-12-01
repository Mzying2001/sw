#include "DockPanel.h"

sw::DockPanel::DockPanel()
    : LastChildFill(
          Property<bool>::Init(this)
              .Getter([](DockPanel *self) -> bool {
                  return self->_dockLayout.lastChildFill;
              })
              .Setter([](DockPanel *self, bool value) {
                  if (self->_dockLayout.lastChildFill != value) {
                      self->_dockLayout.lastChildFill = value;
                      self->InvalidateMeasure();
                  }
              }))
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
