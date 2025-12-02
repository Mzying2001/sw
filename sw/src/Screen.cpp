#include "Screen.h"
#include "Dip.h"

const sw::ReadOnlyProperty<double> sw::Screen::Width(
    Property<double>::Init()
        .Getter([]() -> double {
            return Dip::PxToDipX(GetSystemMetrics(SM_CXSCREEN));
        }) //
);

const sw::ReadOnlyProperty<double> sw::Screen::Height(
    Property<double>::Init()
        .Getter([]() -> double {
            return Dip::PxToDipY(GetSystemMetrics(SM_CYSCREEN));
        }) //
);

const sw::ReadOnlyProperty<sw::Size> sw::Screen::Size(
    Property<sw::Size>::Init()
        .Getter([]() -> sw::Size {
            return sw::Size{
                Dip::PxToDipX(GetSystemMetrics(SM_CXSCREEN)),
                Dip::PxToDipY(GetSystemMetrics(SM_CYSCREEN))};
        }) //
);

const sw::ReadOnlyProperty<sw::Size> sw::Screen::VirtualSize(
    Property<sw::Size>::Init()
        .Getter([]() -> sw::Size {
            return sw::Size{
                Dip::PxToDipX(GetSystemMetrics(SM_CXVIRTUALSCREEN)),
                Dip::PxToDipY(GetSystemMetrics(SM_CYVIRTUALSCREEN))};
        }) //
);

const sw::ReadOnlyProperty<sw::Point> sw::Screen::VirtualOrigin(
    Property<sw::Point>::Init()
        .Getter([]() -> sw::Point {
            return sw::Point{
                Dip::PxToDipX(GetSystemMetrics(SM_XVIRTUALSCREEN)),
                Dip::PxToDipY(GetSystemMetrics(SM_YVIRTUALSCREEN))};
        }) //
);

const sw::ReadOnlyProperty<sw::Point> sw::Screen::CursorPosition(
    Property<sw::Point>::Init()
        .Getter([]() -> sw::Point {
            POINT p;
            GetCursorPos(&p);
            return p;
        }) //
);
