#pragma once

#include <Windows.h>
#include <cstdint>

namespace sw
{
    struct Color {
        uint8_t r;
        uint8_t g;
        uint8_t b;

        Color();
        Color(uint8_t r, uint8_t g, uint8_t b);
        Color(COLORREF color);
        operator COLORREF() const;
    };
}
