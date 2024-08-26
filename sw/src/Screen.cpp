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

const sw::ReadOnlyProperty<sw::Point> sw::Screen::CursorPosition(
    []() -> sw::Point {
        POINT p;
        GetCursorPos(&p);
        return p;
    } //
);
