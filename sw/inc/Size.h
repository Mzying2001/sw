#pragma once

#include <Windows.h>
#include <iostream>

namespace sw
{
    /**
     * @brief 尺寸
     */
    struct Size {
        /**
         * @brief 宽度
         */
        double width;

        /**
         * @brief 高度
         */
        double height;

        Size();
        Size(double width, double height);
        Size(const SIZE &size);
        operator SIZE() const;

        friend bool operator==(const Size &left, const Size &right)
        {
            return (left.width == right.width) && (left.height == right.height);
        }

        friend bool operator!=(const Size &left, const Size &right)
        {
            return (left.width != right.width) || (left.height != right.height);
        }

        friend std::wostream &operator<<(std::wostream &wos, const Size &size)
        {
            return wos << L"Size{width=" << size.width << L", height=" << size.height << L"}";
        }
    };
}
