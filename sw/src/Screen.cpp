#include "Screen.h"
#include "Dip.h"

const sw::ReadOnlyProperty<double> sw::Screen::Width(
    []() -> double {
        return Dip::PxToDipX(GetSystemMetrics(SM_CXSCREEN));
    } //
);

const sw::ReadOnlyProperty<double> sw::Screen::Height(
    []() -> double {
        return Dip::PxToDipY(GetSystemMetrics(SM_CYSCREEN));
    } //
);

const sw::ReadOnlyProperty<sw::Size> sw::Screen::Size(
    []() -> sw::Size {
        return sw::Size{
            Dip::PxToDipX(GetSystemMetrics(SM_CXSCREEN)),
            Dip::PxToDipY(GetSystemMetrics(SM_CYSCREEN))};
    } //
);

const sw::ReadOnlyProperty<sw::Size> sw::Screen::VirtualSize(
    []() -> sw::Size {
        return sw::Size{
            Dip::PxToDipX(GetSystemMetrics(SM_CXVIRTUALSCREEN)),
            Dip::PxToDipY(GetSystemMetrics(SM_CYVIRTUALSCREEN))};
    } //
);

const sw::ReadOnlyProperty<sw::Point> sw::Screen::VirtualOrigin(
    []() -> sw::Point {
        return sw::Point{
            Dip::PxToDipX(GetSystemMetrics(SM_XVIRTUALSCREEN)),
            Dip::PxToDipY(GetSystemMetrics(SM_YVIRTUALSCREEN))};
    } //
);

const sw::ReadOnlyProperty<sw::Point> sw::Screen::CursorPosition(
    []() -> sw::Point {
        POINT p;
        GetCursorPos(&p);
        return p;
    } //
);
