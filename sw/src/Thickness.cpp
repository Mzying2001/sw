#include "Thickness.h"
#include "Utils.h"

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

std::wstring sw::Thickness::ToString() const
{
    return Utils::FormatStr(L"Thickness{left=%g, top=%g, right=%g, bottom=%g}", this->left, this->top, this->right, this->bottom);
}
