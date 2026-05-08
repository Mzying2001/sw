#include "Point.h"
#include "Dip.h"
#include "Utils.h"

sw::Point::Point(double x, double y) noexcept
    : x(x), y(y)
{
}

sw::Point::Point(const POINT &point) noexcept
    : x(Dip::PxToDipX(point.x)), y(Dip::PxToDipY(point.y))
{
}

sw::Point::operator POINT() const noexcept
{
    return {Dip::DipToPxX(this->x), Dip::DipToPxY(this->y)};
}

bool sw::Point::Equals(const Point &other) const noexcept
{
    return (this->x == other.x) && (this->y == other.y);
}

std::wstring sw::Point::ToString() const
{
    return Utils::FormatStr(L"(%g, %g)", this->x, this->y);
}
