#include "Point.h"
#include "Dip.h"

sw::Point::Point()
    : Point(0, 0)
{
}

sw::Point::Point(double x, double y)
    : x(x), y(y)
{
}

sw::Point::Point(const POINT &point)
    : x(Dip::PxToDipX(point.x)), y(Dip::PxToDipY(point.y))
{
}

sw::Point::operator POINT() const
{
    return {Dip::DipToPxX(this->x), Dip::DipToPxY(this->y)};
}

bool sw::Point::operator==(const Point &other) const
{
    return (this->x == other.x) && (this->y == other.y);
}

bool sw::Point::operator!=(const Point &other) const
{
    return (this->x != other.x) || (this->y != other.y);
}

std::wostream &sw::operator<<(std::wostream &wos, const Point &point)
{
    return wos << L"(" << point.x << L", " << point.y << L")";
}
