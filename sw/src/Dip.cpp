#include "Dip.h"
#include <cmath>

#if !defined(USER_DEFAULT_SCREEN_DPI)
#define USER_DEFAULT_SCREEN_DPI 96
#endif

static struct _ScaleInfo {
    double scaleX;
    double scaleY;
    _ScaleInfo();
} _scaleInfo;

_ScaleInfo::_ScaleInfo()
{
    HDC hdc      = GetDC(NULL);
    this->scaleX = (double)USER_DEFAULT_SCREEN_DPI / GetDeviceCaps(hdc, LOGPIXELSX);
    this->scaleY = (double)USER_DEFAULT_SCREEN_DPI / GetDeviceCaps(hdc, LOGPIXELSY);
    ReleaseDC(NULL, hdc);
}

/*================================================================================*/

const sw::ReadOnlyProperty<double> sw::Dip::ScaleX(
    []() -> const double & {
        return _scaleInfo.scaleX;
    } //
);

const sw::ReadOnlyProperty<double> sw::Dip::ScaleY(
    []() -> const double & {
        return _scaleInfo.scaleY;
    } //
);

void sw::Dip::Update(int dpiX, int dpiY)
{
    _scaleInfo.scaleX = (double)USER_DEFAULT_SCREEN_DPI / dpiX;
    _scaleInfo.scaleY = (double)USER_DEFAULT_SCREEN_DPI / dpiY;
}

double sw::Dip::PxToDipX(int px)
{
    return px * _scaleInfo.scaleX;
}

double sw::Dip::PxToDipY(int px)
{
    return px * _scaleInfo.scaleY;
}

int sw::Dip::DipToPxX(double dip)
{
    return (int)std::lround(dip / _scaleInfo.scaleX);
}

int sw::Dip::DipToPxY(double dip)
{
    return (int)std::lround(dip / _scaleInfo.scaleY);
}
