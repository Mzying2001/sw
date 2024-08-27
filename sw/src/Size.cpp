#include "Size.h"
#include "Dip.h"
#include <cmath>

sw::Size::Size()
    : Size(0, 0)
{
}

sw::Size::Size(double width, double height)
    : width(width), height(height)
{
}

sw::Size::Size(const SIZE &size)
{
    double scaleX = Dip::ScaleX.Get();
    double scaleY = Dip::ScaleY.Get();

    this->width  = size.cx * scaleX;
    this->height = size.cy * scaleY;
}

sw::Size::operator SIZE() const
{
    double scaleX = Dip::ScaleX.Get();
    double scaleY = Dip::ScaleY.Get();

    SIZE size{};
    size.cx = std::lround(this->width / scaleX);
    size.cy = std::lround(this->height / scaleY);
    return size;
}
