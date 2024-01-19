#pragma once

#include <Windows.h>
#include <iostream>

namespace sw
{
    /**
     * @brief 表示相对于左上角的点坐标
     */
    struct Point {
        /**
         * @brief 横坐标
         */
        double x;

        /**
         * @brief 纵坐标
         */
        double y;

        Point();
        Point(double x, double y);
        Point(const POINT &point);
        operator POINT() const;

        friend bool operator==(const Point &left, const Point &right)
        {
            return (left.x == right.x) && (left.y == right.y);
        }

        friend bool operator!=(const Point &left, const Point &right)
        {
            return (left.x != right.x) || (left.y != right.y);
        }

        friend std::wostream &operator<<(std::wostream &wos, const Point &point)
        {
            return wos << L"(" << point.x << L", " << point.y << L")";
        }
    };
}
