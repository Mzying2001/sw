#include "Point.h"
#include "Dip.h"
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
    static double &scaleX = const_cast<double &>(Dip::ScaleX.Get());
    static double &scaleY = const_cast<double &>(Dip::ScaleY.Get());

    this->x = scaleX * point.x;
    this->y = scaleY * point.y;
}

sw::Point::operator POINT() const
{
    static double &scaleX = const_cast<double &>(Dip::ScaleX.Get());
    static double &scaleY = const_cast<double &>(Dip::ScaleY.Get());

    POINT point{};
    point.x = std::lround(this->x / scaleX);
    point.y = std::lround(this->y / scaleY);
    return point;
}
