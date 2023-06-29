#include "Dip.h"

static sw::Dip::DipScaleInfo _GetScaleInfo();
static sw::Dip::DipScaleInfo _dpiScaleInfo = _GetScaleInfo();

void sw::Dip::Update(int dpiX, int dpiY)
{
    _dpiScaleInfo.scaleX = (double)USER_DEFAULT_SCREEN_DPI / dpiX;
    _dpiScaleInfo.scaleY = (double)USER_DEFAULT_SCREEN_DPI / dpiY;
}

const sw::ReadOnlyProperty<double> sw::Dip::ScaleX(
    []() -> const double & {
        return _dpiScaleInfo.scaleX;
    } //
);

const sw::ReadOnlyProperty<double> sw::Dip::ScaleY(
    []() -> const double & {
        return _dpiScaleInfo.scaleY;
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
