#pragma once

#include "KnownColor.h"
#include <cstdint>
#include <iostream>

namespace sw
{
    /**
     * @brief 颜色
     */
    struct Color {
        /**
         * @brief R分量
         */
        uint8_t r;

        /**
         * @brief G分量
         */
        uint8_t g;

        /**
         * @brief B分量
         */
        uint8_t b;

        /**
         * @brief 保留字段
         */
        uint8_t _reserved{0};

        Color();
        Color(uint8_t r, uint8_t g, uint8_t b);
        Color(KnownColor knownColor);
        Color(COLORREF color);
        operator COLORREF() const;

        friend bool operator==(const Color &left, const Color &right)
        {
            return (left.r == right.r) && (left.g == right.g) && (left.b == right.b);
        }

        friend bool operator!=(const Color &left, const Color &right)
        {
            return (left.r != right.r) || (left.g != right.g) || (left.b != right.b);
        }

        friend std::wostream &operator<<(std::wostream &wos, const Color &color)
        {
            return wos << L"Color{r=" << (int)color.r << L", g=" << (int)color.g << L", b=" << (int)color.b << L"}";
        }
    };
}
