#pragma once

#include "Point.h"
#include "Size.h"
#include <Windows.h>
#include <iostream>

namespace sw
{
    /**
     * @brief 表示一个矩形区域
     */
    struct Rect {
        /**
         * @brief 左边
         */
        double left;

        /**
         * @brief 顶边
         */
        double top;

        /**
         * @brief 宽度
         */
        double width;

        /**
         * @brief 高度
         */
        double height;

        Rect();
        Rect(double left, double top, double width, double height);
        Rect(const RECT &rect);
        operator RECT() const;

        Point GetPos() const;
        Size GetSize() const;

        friend bool operator==(const Rect& left, const Rect& right)
        {
            return (left.left   == right.left)  &&
                   (left.top    == right.top)   &&
                   (left.width  == right.width) &&
                   (left.height == right.height);
        }

        friend bool operator!=(const Rect& left, const Rect& right)
        {
            return (left.left   != right.left)  ||
                   (left.top    != right.top)   ||
                   (left.width  != right.width) ||
                   (left.height != right.height);
        }

        friend std::wostream& operator<<(std::wostream& wos, const Rect& rect)
        {
            return wos << L"Rect{left=" << rect.left << L", top=" << rect.top << L", width=" << rect.width << L", height=" << rect.height << L"}";
        }
    };
}
