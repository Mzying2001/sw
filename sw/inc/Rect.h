#pragma once

#include "IToString.h"
#include "Point.h"
#include "Size.h"
#include <Windows.h>
#include <string>

namespace sw
{
    /**
     * @brief 表示一个矩形区域
     */
    struct Rect : public IToString<Rect> {
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

        /**
         * @brief 构造Rect
         */
        Rect();

        /**
         * @brief 构造Rect
         */
        Rect(double left, double top, double width, double height);

        /**
         * @brief 从RECT构造Rect
         */
        Rect(const RECT &rect);

        /**
         * @brief 隐式转换RECT
         */
        operator RECT() const;

        /**
         * @brief 获取Rect左上角的位置
         */
        Point GetPos() const;

        /**
         * @brief 获取Rect的尺寸
         */
        Size GetSize() const;

        /**
         * @brief 判断两个Rect是否相等
         */
        bool operator==(const Rect &other) const;

        /**
         * @brief 判断两个Rect是否不相等
         */
        bool operator!=(const Rect &other) const;

        /**
         * @brief 获取描述当前对象的字符串
         */
        std::wstring ToString() const;
    };
}
