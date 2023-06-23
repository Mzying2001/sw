#pragma once

#include "Property.h"
#include "Dpi.h"

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
