#pragma once

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
    };
}
