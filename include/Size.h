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

        /**
         * @brief 构造宽高均为0的Size结构体
         */
        Size();

        /**
         * @brief 构造指定宽高的Size结构体
         */
        Size(double width, double height);

        /**
         * @brief 从SIZE构造Size结构体
         */
        Size(const SIZE &size);

        /**
         * @brief 隐式转换SIZE
         */
        operator SIZE() const;

        /**
         * @brief 判断两个Size是否相等
         */
        bool operator==(const Size &other) const;

        /**
         * @brief 判断两个Size是否不相等
         */
        bool operator!=(const Size &other) const;

        /**
         * @brief 支持Utils::BuildStr
         */
        friend std::wostream &operator<<(std::wostream &wos, const Size &size);
    };
}
