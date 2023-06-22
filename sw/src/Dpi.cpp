#include "Dpi.h"

static sw::Dpi::DpiScaleInfo _GetCurrentScaleInfo();
static sw::Dpi::DpiScaleInfo _dpiScaleInfo = _GetCurrentScaleInfo();

void sw::Dpi::Update()
{
    _dpiScaleInfo = _GetCurrentScaleInfo();
}

const sw::ReadOnlyProperty<double> sw::Dpi::ScaleX(
    []() -> const double & {
        return _dpiScaleInfo.scaleX;
    } //
);

const sw::ReadOnlyProperty<double> sw::Dpi::ScaleY(
    []() -> const double & {
        return _dpiScaleInfo.scaleY;
    } //
);

static sw::Dpi::DpiScaleInfo _GetCurrentScaleInfo()
{
    sw::Dpi::DpiScaleInfo info{};
    HDC hdc     = GetDC(NULL);
    info.scaleX = (double)GetDeviceCaps(hdc, LOGPIXELSX) / USER_DEFAULT_SCREEN_DPI;
    info.scaleY = (double)GetDeviceCaps(hdc, LOGPIXELSY) / USER_DEFAULT_SCREEN_DPI;
    ReleaseDC(NULL, hdc);
    return info;
}
