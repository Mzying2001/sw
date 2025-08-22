#include "Thickness.h"
#include "Dip.h"
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

sw::Thickness::Thickness(const RECT &rect)
    : Thickness(
          Dip::PxToDipX(rect.left),
          Dip::PxToDipY(rect.top),
          Dip::PxToDipX(rect.right),
          Dip::PxToDipY(rect.bottom))
{
}

sw::Thickness::operator RECT() const
{
    return RECT{
        Dip::DipToPxX(this->left),
        Dip::DipToPxY(this->top),
        Dip::DipToPxX(this->right),
        Dip::DipToPxY(this->bottom)};
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
