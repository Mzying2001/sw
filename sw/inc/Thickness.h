#pragma once

#include <iostream>

namespace sw
{
    /**
     * @brief 表示矩形区域周围边框的厚度
     */
    struct Thickness {
        /**
         * @brief 左边
         */
        double left;

        /**
         * @brief 顶边
         */
        double top;

        /**
         * @brief 右边
         */
        double right;

        /**
         * @brief 底边
         */
        double bottom;

        Thickness();
        Thickness(double thickness);
        Thickness(double horizontal, double vertical);
        Thickness(double left, double top, double right, double bottom);

        friend bool operator==(const Thickness &left, const Thickness &right)
        {
            return (left.left   == right.left)  &&
                   (left.top    == right.top)   &&
                   (left.right  == right.right) &&
                   (left.bottom == right.bottom);
        }

        friend bool operator!=(const Thickness &left, const Thickness &right)
        {
            return (left.left   != right.left)  ||
                   (left.top    != right.top)   ||
                   (left.right  != right.right) ||
                   (left.bottom != right.bottom);
        }

        friend std::wostream &operator<<(std::wostream &wos, const Thickness &thickness)
        {
            return wos << L"Thickness{left=" << thickness.left << L", top=" << thickness.top << L", right=" << thickness.right << L", bottom=" << thickness.bottom << L"}";
        }
    };
}
