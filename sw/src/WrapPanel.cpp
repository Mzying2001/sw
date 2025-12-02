#include "WrapPanel.h"

sw::WrapPanel::WrapPanel()
    : Orientation(
          Property<sw::Orientation>::Init(this)
              .Getter([](WrapPanel *self) -> sw::Orientation {
                  return self->_wrapLayout.orientation;
              })
              .Setter([](WrapPanel *self, sw::Orientation value) {
                  if (self->_wrapLayout.orientation != value) {
                      self->_wrapLayout.orientation = value;
                      self->InvalidateMeasure();
                  }
              }))
{
    this->_wrapLayout.Associate(this);
    this->HorizontalAlignment = HorizontalAlignment::Stretch;
    this->VerticalAlignment   = VerticalAlignment::Stretch;
}

sw::LayoutHost *sw::WrapPanel::GetDefaultLayout()
{
    return &this->_wrapLayout;
}
