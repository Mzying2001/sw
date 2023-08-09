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
        operator RECT() const;

        friend bool operator==(const Rect &left, const Rect &right);
        friend bool operator!=(const Rect &left, const Rect &right);
        friend std::wostream &operator<<(std::wostream &wos, const Rect &rect);
    };
}
