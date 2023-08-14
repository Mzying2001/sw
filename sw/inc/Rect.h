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

        friend bool operator==(const Rect& left, const Rect& right)
        {
            return (left.left   == right.left)  &&
                   (left.top    == right.top)   &&
                   (left.width  == right.width) &&
                   (left.height == right.height);
        }

        friend bool operator!=(const Rect& left, const Rect& right)
        {
            return (left.left   != right.left)  ||
                   (left.top    != right.top)   ||
                   (left.width  != right.width) ||
                   (left.height != right.height);
        }

        friend std::wostream& operator<<(std::wostream& wos, const Rect& rect)
        {
            return wos << L"Rect{left=" << rect.left << L", top=" << rect.top << L", width=" << rect.width << L", height=" << rect.height << L"}";
        }
    };
}
