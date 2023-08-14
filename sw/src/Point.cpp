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
    this->x = Dip::ScaleX * point.x;
    this->y = Dip::ScaleY * point.y;
}

sw::Point::operator POINT() const
{
    POINT point{};
    point.x = std::lround(this->x / Dip::ScaleX);
    point.y = std::lround(this->y / Dip::ScaleY);
    return point;
}

bool operator==(const sw::Point &left, const sw::Point &right)
{
    return (left.x == right.x) && (left.y == right.y);
}

bool operator!=(const sw::Point &left, const sw::Point &right)
{
    return (left.x != right.x) || (left.y != right.y);
}

std::wostream &operator<<(std::wostream &wos, const sw::Point &point)
{
    return wos << L"(" << point.x << L", " << point.y << L")";
}
