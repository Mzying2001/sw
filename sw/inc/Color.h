#pragma once

#include "IComparable.h"
#include "IToString.h"
#include "KnownColor.h"
#include <cstdint>
#include <string>

namespace sw
{
    /**
     * @brief 颜色
     */
    struct Color : public IToString<Color>,
                   public IEqualityComparable<Color> {
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

        /**
         * @brief 构造一个rgb分量均为0的Color结构体
         */
        Color();

        /**
         * @brief 通过rgb构造Color结构体
         */
        Color(uint8_t r, uint8_t g, uint8_t b);

        /**
         * @brief 通过KnownColor构造Color结构体
         */
        Color(KnownColor knownColor);

        /**
         * @brief 通过COLORREF构造Color结构体
         */
        Color(COLORREF color);

        /**
         * @brief 隐式转换COLORREF
         */
        operator COLORREF() const;

        /**
         * @brief 判断两个Color是否相等
         */
        bool Equals(const Color &other) const;

        /**
         * @brief 获取描述当前对象的字符串
         */
        std::wstring ToString() const;
    };
}
