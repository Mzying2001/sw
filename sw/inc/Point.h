#pragma once

#include <Windows.h>
#include <iostream>

namespace sw
{
    struct Point {
        double x;
        double y;

        Point();
        Point(double x, double y);
        Point(const POINT &point);
        operator POINT() const;

        friend std::wostream &operator<<(std::wostream &wos, const Point &point);
    };
}
