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
        Thickness(double left, double top, double right, double bottom);

        friend bool operator==(const Thickness &left, const Thickness &right);
        friend bool operator!=(const Thickness &left, const Thickness &right);
        friend std::wostream &operator<<(std::wostream &wos, const Thickness &thickness);
    };
}
