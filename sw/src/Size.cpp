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
    this->width  = size.cx * Dip::ScaleX;
    this->height = size.cy * Dip::ScaleY;
}

sw::Size::operator SIZE() const
{
    SIZE size{};
    size.cx = std::lround(this->width / Dip::ScaleX);
    size.cy = std::lround(this->height / Dip::ScaleY);
    return size;
}

bool operator==(const sw::Size &left, const sw::Size &right)
{
    return (left.width == right.width) && (left.height == right.height);
}

bool operator!=(const sw::Size &left, const sw::Size &right)
{
    return (left.width != right.width) || (left.height != right.height);
}

std::wostream &operator<<(std::wostream &wos, const sw::Size &size)
{
    return wos << L"Size{width=" << size.width << L", height=" << size.height << L"}";
}
