#pragma once

#include "Dpi.h"
#include "Property.h"

namespace sw
{
    class Screen
    {
    private:
        Screen();

    public:
        static const ReadOnlyProperty<double> Width;
        static const ReadOnlyProperty<double> Height;
    };
}
