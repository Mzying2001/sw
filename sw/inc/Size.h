#pragma once

#include <Windows.h>

namespace sw
{
    struct Size {
        double width;
        double height;

        Size();
        Size(double width, double height);
        Size(const SIZE &size);
        operator SIZE() const;
    };
}
