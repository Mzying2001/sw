#include "Canvas.h"

sw::Canvas::Canvas()
{
    this->_canvasLayout.Associate(this);
    this->SetAlignment(HorizontalAlignment::Stretch, VerticalAlignment::Stretch);
}

sw::CanvasLayoutTag sw::Canvas::GetCanvasLayoutTag(UIElement &element)
{
    return element.LayoutTag.Get();
}

void sw::Canvas::SetCanvasLayoutTag(UIElement &element, const CanvasLayoutTag &tag)
{
    element.LayoutTag.Set(tag);
}

sw::LayoutHost *sw::Canvas::GetDefaultLayout()
{
    return &this->_canvasLayout;
}
