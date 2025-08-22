#pragma once

#include <Windows.h>
#include <string>

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

        /**
         * @brief 构造一个四边都为0的Thickness结构体
         */
        Thickness();

        /**
         * @brief 构造一个四边都相同的Thickness结构体
         */
        Thickness(double thickness);

        /**
         * @brief 指定横向和纵向值构造Thickness结构体
         */
        Thickness(double horizontal, double vertical);

        /**
         * @brief 指定四边的值构造Thickness结构体
         */
        Thickness(double left, double top, double right, double bottom);

        /**
         * @brief 从RECT结构体构造Thickness结构体
         */
        Thickness(const RECT &rect);

        /**
         * @brief 隐式转换为RECT
         */
        operator RECT() const;

        /**
         * @brief 判断两个Thickness是否相同
         */
        bool operator==(const Thickness &other) const;

        /**
         * @brief 判断两个Thickness是否相同
         */
        bool operator!=(const Thickness &other) const;

        /**
         * @brief 获取描述当前对象的字符串
         */
        std::wstring ToString() const;
    };
}
