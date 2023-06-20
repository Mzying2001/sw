#pragma once

#include <Windows.h>

namespace sw
{
    struct Rect {
        int left;
        int top;
        unsigned int width;
        unsigned int height;

        Rect();
        Rect(int left, int top, unsigned int width, unsigned int height);
        Rect(const RECT &rect);
        operator RECT();
    };
}
