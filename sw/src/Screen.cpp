#include "Screen.h"
#include "Dip.h"

const sw::ReadOnlyProperty<double> sw::Screen::Width(
    []() -> const double & {
        static double width;
        width = GetSystemMetrics(SM_CXSCREEN) * Dip::ScaleX;
        return width;
    } //
);

const sw::ReadOnlyProperty<double> sw::Screen::Height(
    []() -> const double & {
        static double height;
        height = GetSystemMetrics(SM_CYSCREEN) * Dip::ScaleY;
        return height;
    } //
);
