#include "Layer.h"

sw::Layer::Layer()
    : Layout(
          // get
          [&]() -> LayoutHost *const & {
              return this->_layout;
          },
          // set
          [&](LayoutHost *const &value) {
              if (value != nullptr)
                  value->Associate(this);
              this->_layout = value;
          })
{
    this->_defaultLayout.Associate(this);
}

void sw::Layer::Measure(const Size &availableSize)
{
    Size size           = availableSize;
    sw::Rect windowRect = this->Rect;
    sw::Rect clientRect = this->ClientRect;
    Thickness margin    = this->Margin;

    // 考虑边框
    size.width -= (windowRect.width - clientRect.width) + margin.left + margin.top;
    size.height -= (windowRect.height - clientRect.height) + margin.top + margin.bottom;

    this->UIElement::Measure(availableSize);
    this->GetLayoutHost().Measure(size);
}

void sw::Layer::Arrange(const sw::Rect &finalPosition)
{
    this->UIElement::Arrange(finalPosition);
    this->GetLayoutHost().Arrange(this->ClientRect);
}

sw::LayoutHost &sw::Layer::GetLayoutHost()
{
    return this->_layout == nullptr ? this->_defaultLayout : *this->_layout;
}

void sw::Layer::UpdateLayout()
{
    if (!this->_layoutDisabled) {
        sw::Rect clientRect = this->ClientRect;
        this->GetLayoutHost().Measure(Size(clientRect.width, clientRect.height));
        this->GetLayoutHost().Arrange(clientRect);
    }
}

void sw::Layer::DisableLayout()
{
    this->_layoutDisabled = true;
}

void sw::Layer::EnableLayout()
{
    this->_layoutDisabled = false;
    this->UpdateLayout();
}
