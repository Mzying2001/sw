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
                      self->RaisePropertyChanged(&StackPanel::Orientation);
                      self->InvalidateMeasure();
                  }
              })),

      Spacing(
          Property<double>::Init(this)
              .Getter([](StackPanel *self) -> double {
                  return self->_stackLayout.spacing;
              })
              .Setter([](StackPanel *self, double value) {
                  if (self->_stackLayout.spacing != value) {
                      self->_stackLayout.spacing = value;
                      self->RaisePropertyChanged(&StackPanel::Spacing);
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
