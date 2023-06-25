#pragma once

#include "Dpi.h"
#include <Windows.h>

namespace sw
{
    struct Point {
        double x;
        double y;

        Point();
        Point(double x, double y);
        Point(const POINT &point);
        operator POINT() const;
    };
}
