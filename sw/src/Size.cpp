#include "Size.h"
#include "Dip.h"
#include "Utils.h"

sw::Size::Size()
    : Size(0, 0)
{
}

sw::Size::Size(double width, double height)
    : width(width), height(height)
{
}

sw::Size::Size(const SIZE &size)
    : width(Dip::PxToDipX(size.cx)), height(Dip::PxToDipY(size.cy))
{
}

sw::Size::operator SIZE() const
{
    return {Dip::DipToPxX(this->width), Dip::DipToPxY(this->height)};
}

bool sw::Size::Equals(const Size &other) const
{
    return (this->width == other.width) && (this->height == other.height);
}

std::wstring sw::Size::ToString() const
{
    return Utils::FormatStr(L"Size{width=%g, height=%g}", this->width, this->height);
}
