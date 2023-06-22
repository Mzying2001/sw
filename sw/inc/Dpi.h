#pragma once

#include <Windows.h>
#include "Property.h"

namespace sw
{
    class Dpi
    {
    public:
        struct DpiScaleInfo {
            double scaleX;
            double scaleY;
        };

    private:
        Dpi();
        static DpiScaleInfo _info;

    public:
        static const ReadOnlyProperty<double> ScaleX;
        static const ReadOnlyProperty<double> ScaleY;
        static void Update();
    };
}
