#include "Rect.h"
#include "Dip.h"
#include "Utils.h"

sw::Rect::Rect(double left, double top, double width, double height)
    : left(left), top(top), width(width), height(height)
{
}

sw::Rect::Rect(const RECT &rect)
    : left(Dip::PxToDipX(rect.left)),
      top(Dip::PxToDipY(rect.top)),
      width(Dip::PxToDipX(rect.right - rect.left)),
      height(Dip::PxToDipY(rect.bottom - rect.top))
{
}

sw::Rect::operator RECT() const
{
    return {Dip::DipToPxX(this->left),
            Dip::DipToPxY(this->top),
            Dip::DipToPxX(this->left + this->width),
            Dip::DipToPxY(this->top + this->height)};
}

sw::Point sw::Rect::GetPos() const
{
    return Point(this->left, this->top);
}

sw::Size sw::Rect::GetSize() const
{
    return Size(this->width, this->height);
}

bool sw::Rect::Equals(const Rect &other) const
{
    return (this->left == other.left) &&
           (this->top == other.top) &&
           (this->width == other.width) &&
           (this->height == other.height);
}

std::wstring sw::Rect::ToString() const
{
    return Utils::FormatStr(L"Rect{left=%g, top=%g, width=%g, height=%g}", this->left, this->top, this->width, this->height);
}
