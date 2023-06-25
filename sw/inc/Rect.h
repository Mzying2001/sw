#pragma once

#include "Dpi.h"
#include <Windows.h>

namespace sw
{
    struct Rect {
        double left;
        double top;
        double width;
        double height;

        Rect();
        Rect(double left, double top, double width, double height);
        Rect(const RECT &rect);
        RECT GetRECT() const;
    };
}
