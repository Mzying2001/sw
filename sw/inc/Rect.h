#pragma once

#include <Windows.h>
#include <iostream>

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

        friend std::wostream &operator<<(std::wostream &wos, const Rect &rect);
    };
}
