#pragma once

#include <Windows.h>
#include <string>

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

        /**
         * @brief 构造xy均为0的Point结构体
         */
        Point();

        /**
         * @brief 构造指定xy值的Point结构体
         */
        Point(double x, double y);

        /**
         * @brief 从POINT构造Point结构体
         */
        Point(const POINT &point);

        /**
         * @brief 隐式转换POINT
         */
        operator POINT() const;

        /**
         * @brief 判断两个Point是否相等
         */
        bool operator==(const Point &other) const;

        /**
         * @brief 判断两个Point是否不相等
         */
        bool operator!=(const Point &other) const;

        /**
         * @brief 获取描述当前对象的字符串
         */
        std::wstring ToString() const;
    };
}
