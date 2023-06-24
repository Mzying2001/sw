#pragma once

#include <Windows.h>
#include "Dpi.h"

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
