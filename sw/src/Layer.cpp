#include "Layer.h"
#include <cmath>

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

void sw::Layer::Measure(const Size &availableSize)
{
    Size measureSize    = availableSize;
    Thickness margin    = this->Margin;
    sw::Rect windowRect = this->Rect;
    sw::Rect clientRect = this->ClientRect;

    // 考虑边框
    measureSize.width -= (windowRect.width - clientRect.width) + margin.left + margin.right;
    measureSize.height -= (windowRect.height - clientRect.height) + margin.top + margin.bottom;

    this->GetLayoutHost().Measure(measureSize);

    Size desireSize = this->GetDesireSize();

    if (std::isnan(desireSize.width) || std::isnan(desireSize.height)) {
        // AbsoluteLayout特殊处理：用nan表示按照普通控件处理
        this->UIElement::Measure(availableSize);
    } else {
        // 考虑边框
        desireSize.width += (windowRect.width - clientRect.width) + margin.left + margin.right;
        desireSize.height += (windowRect.width - clientRect.width) + margin.top + margin.bottom;
        this->SetDesireSize(desireSize);
    }
}

void sw::Layer::Arrange(const sw::Rect &finalPosition)
{
    this->UIElement::Arrange(finalPosition);
    this->GetLayoutHost().Arrange(this->ClientRect);
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
