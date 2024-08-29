#include "Rect.h"
#include "Dip.h"

sw::Rect::Rect()
    : Rect(0, 0, 0, 0)
{
}

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
