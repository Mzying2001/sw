#pragma once

#include <iostream>

namespace sw
{
    struct Thickness {
        double left;
        double top;
        double right;
        double bottom;

        Thickness();
        Thickness(double thickness);
        Thickness(double horizontal, double vertical);
        Thickness(double left, double top, double right, double bottom);

        friend bool operator==(const Thickness &left, const Thickness &right)
        {
            return (left.left   == right.left)  &&
                   (left.top    == right.top)   &&
                   (left.right  == right.right) &&
                   (left.bottom == right.bottom);
        }

        friend bool operator!=(const Thickness &left, const Thickness &right)
        {
            return (left.left   != right.left)  ||
                   (left.top    != right.top)   ||
                   (left.right  != right.right) ||
                   (left.bottom != right.bottom);
        }

        friend std::wostream &operator<<(std::wostream &wos, const Thickness &thickness)
        {
            return wos << L"Thickness{left=" << thickness.left << L", top=" << thickness.top << L", right=" << thickness.right << L", bottom=" << thickness.bottom << L"}";
        }
    };
}
