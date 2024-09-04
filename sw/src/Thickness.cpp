#include "Thickness.h"

sw::Thickness::Thickness()
    : Thickness(0, 0, 0, 0)
{
}

sw::Thickness::Thickness(double thickness)
    : Thickness(thickness, thickness, thickness, thickness)
{
}

sw::Thickness::Thickness(double horizontal, double vertical)
    : Thickness(horizontal, vertical, horizontal, vertical)
{
}

sw::Thickness::Thickness(double left, double top, double right, double bottom)
    : left(left), top(top), right(right), bottom(bottom)
{
}

bool sw::Thickness::operator==(const Thickness &other) const
{
    return (this->left == other.left) &&
           (this->top == other.top) &&
           (this->right == other.right) &&
           (this->bottom == other.bottom);
}

bool sw::Thickness::operator!=(const Thickness &other) const
{
    return (this->left != other.left) ||
           (this->top != other.top) ||
           (this->right != other.right) ||
           (this->bottom != other.bottom);
}

std::wostream &sw::operator<<(std::wostream &wos, const Thickness &thickness)
{
    return wos << L"Thickness{left=" << thickness.left << L", top=" << thickness.top << L", right=" << thickness.right << L", bottom=" << thickness.bottom << L"}";
}
