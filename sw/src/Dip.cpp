#include "Dip.h"

static sw::Dip::DpiScaleInfo _GetCurrentScaleInfo();
static sw::Dip::DpiScaleInfo _dpiScaleInfo = _GetCurrentScaleInfo();

void sw::Dip::Update()
{
    _dpiScaleInfo = _GetCurrentScaleInfo();
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

static sw::Dip::DpiScaleInfo _GetCurrentScaleInfo()
{
    sw::Dip::DpiScaleInfo info{};
    HWND hwnd   = GetDesktopWindow();
    HDC hdc     = GetDC(hwnd);
    info.scaleX = 96.0 / GetDeviceCaps(hdc, LOGPIXELSX);
    info.scaleY = 96.0 / GetDeviceCaps(hdc, LOGPIXELSY);
    ReleaseDC(hwnd, hdc);
    return info;
}
