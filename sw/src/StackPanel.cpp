#include "StackPanel.h"

sw::StackPanel::StackPanel()
    : Orientation(
          Property<sw::Orientation>::Init(this)
              .Getter([](StackPanel *self) -> sw::Orientation {
                  return self->_stackLayout.orientation;
              })
              .Setter([](StackPanel *self, sw::Orientation value) {
                  if (self->_stackLayout.orientation != value) {
                      self->_stackLayout.orientation = value;
                      self->InvalidateMeasure();
                  }
              }))

{
    this->_stackLayout.Associate(this);
    this->HorizontalAlignment = HorizontalAlignment::Stretch;
    this->VerticalAlignment   = VerticalAlignment::Stretch;
}

sw::LayoutHost *sw::StackPanel::GetDefaultLayout()
{
    return &this->_stackLayout;
}
