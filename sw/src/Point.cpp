#include "Point.h"
#include <cmath>

sw::Point::Point()
    : Point(0, 0)
{
}

sw::Point::Point(double x, double y)
    : x(x), y(y)
{
}

sw::Point::Point(const POINT &point)
{
    this->x = Dpi::ScaleX * point.x;
    this->y = Dpi::ScaleY * point.y;
}

sw::Point::operator POINT() const
{
    POINT point{};
    point.x = std::lround(this->x / Dpi::ScaleX);
    point.y = std::lround(this->y / Dpi::ScaleY);
    return point;
}
