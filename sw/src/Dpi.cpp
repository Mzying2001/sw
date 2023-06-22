#include "Dpi.h"

static sw::Dpi::DpiScaleInfo GetCurrentScaleInfo()
{
    sw::Dpi::DpiScaleInfo info{};
    HDC hdc     = GetDC(NULL);
    info.scaleX = (double)GetDeviceCaps(hdc, LOGPIXELSX) / USER_DEFAULT_SCREEN_DPI;
    info.scaleY = (double)GetDeviceCaps(hdc, LOGPIXELSY) / USER_DEFAULT_SCREEN_DPI;
    ReleaseDC(NULL, hdc);
    return info;
}

void sw::Dpi::Update()
{
    _info = GetCurrentScaleInfo();
}

sw::Dpi::DpiScaleInfo sw::Dpi::_info =
    GetCurrentScaleInfo();

const sw::ReadOnlyProperty<double> sw::Dpi::ScaleX(
    []() -> double {
        return _info.scaleX;
    } //
);

const sw::ReadOnlyProperty<double> sw::Dpi::ScaleY(
    []() -> double {
        return _info.scaleY;
    } //
);
