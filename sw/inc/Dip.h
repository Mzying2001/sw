#pragma once

#include "Property.h"
#include <Windows.h>

namespace sw
{
    class Dip
    {
    public:
        struct DpiScaleInfo {
            double scaleX;
            double scaleY;
        };

    private:
        Dip();

    public:
        static const ReadOnlyProperty<double> ScaleX;
        static const ReadOnlyProperty<double> ScaleY;
        static void Update();
    };
}
