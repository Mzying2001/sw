#include "Thickness.h"

sw::Thickness::Thickness()
    : Thickness(0, 0, 0, 0)
{
}

sw::Thickness::Thickness(double thickness)
    : Thickness(thickness, thickness, thickness, thickness)
{
}

sw::Thickness::Thickness(double left, double top, double right, double bottom)
    : left(left), top(top), right(right), bottom(bottom)
{
}

bool operator==(const sw::Thickness &left, const sw::Thickness &right)
{
    return (left.left   == right.left)  &&
           (left.top    == right.top)   &&
           (left.right  == right.right) &&
           (left.bottom == right.bottom);
}

bool operator!=(const sw::Thickness &left, const sw::Thickness &right)
{
    return (left.left   != right.left)  ||
           (left.top    != right.top)   ||
           (left.right  != right.right) ||
           (left.bottom != right.bottom);
}

std::wostream &operator<<(std::wostream &wos, const sw::Thickness &thickness)
{
    return wos << L"Thickness{left=" << thickness.left << L", top=" << thickness.top << L", right=" << thickness.right << L", bottom=" << thickness.bottom << L"}";
}
