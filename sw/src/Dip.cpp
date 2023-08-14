#include "Dip.h"

#if !defined(USER_DEFAULT_SCREEN_DPI)
#define USER_DEFAULT_SCREEN_DPI 96
#endif

static sw::Dip::DipScaleInfo _GetScaleInfo();
static sw::Dip::DipScaleInfo _dipScaleInfo = _GetScaleInfo();

void sw::Dip::Update(int dpiX, int dpiY)
{
    _dipScaleInfo.scaleX = (double)USER_DEFAULT_SCREEN_DPI / dpiX;
    _dipScaleInfo.scaleY = (double)USER_DEFAULT_SCREEN_DPI / dpiY;
}

const sw::ReadOnlyProperty<double> sw::Dip::ScaleX(
    []() -> const double & {
        return _dipScaleInfo.scaleX;
    } //
);

const sw::ReadOnlyProperty<double> sw::Dip::ScaleY(
    []() -> const double & {
        return _dipScaleInfo.scaleY;
    } //
);

static sw::Dip::DipScaleInfo _GetScaleInfo()
{
    sw::Dip::DipScaleInfo info{};
    HDC hdc     = GetDC(NULL);
    info.scaleX = (double)USER_DEFAULT_SCREEN_DPI / GetDeviceCaps(hdc, LOGPIXELSX);
    info.scaleY = (double)USER_DEFAULT_SCREEN_DPI / GetDeviceCaps(hdc, LOGPIXELSY);
    ReleaseDC(NULL, hdc);
    return info;
}
