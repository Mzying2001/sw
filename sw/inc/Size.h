#pragma once

#include <Windows.h>
#include <iostream>

namespace sw
{
    struct Size {
        double width;
        double height;

        Size();
        Size(double width, double height);
        Size(const SIZE &size);
        operator SIZE() const;

        friend std::wostream &operator<<(std::wostream &wos, const Size &size);
    };
}
