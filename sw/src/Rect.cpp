#include "Rect.h"
#include "Dip.h"
#include <cmath>

sw::Rect::Rect()
    : Rect(0, 0, 0, 0)
{
}

sw::Rect::Rect(double left, double top, double width, double height)
    : left(left), top(top), width(width), height(height)
{
}

sw::Rect::Rect(const RECT &rect)
{
    double scaleX = Dip::ScaleX;
    double scaleY = Dip::ScaleY;
    this->left    = scaleX * rect.left;
    this->top     = scaleY * rect.top;
    this->width   = scaleX * (rect.right - rect.left);
    this->height  = scaleY * (rect.bottom - rect.top);
}

sw::Rect::operator RECT() const
{
    RECT rect{};
    double scaleX;
    double scaleY;
    scaleX      = Dip::ScaleX;
    scaleY      = Dip::ScaleY;
    rect.left   = std::lround(this->left / scaleX);
    rect.top    = std::lround(this->top / scaleY);
    rect.right  = std::lround((this->left + this->width) / scaleX);
    rect.bottom = std::lround((this->top + this->height) / scaleY);
    return rect;
}

sw::Point sw::Rect::GetPos() const
{
    return Point(this->left, this->top);
}

sw::Size sw::Rect::GetSize() const
{
    return Size(this->width, this->height);
}
