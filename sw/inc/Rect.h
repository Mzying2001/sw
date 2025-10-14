#pragma once

#include "IComparable.h"
#include "IToString.h"
#include "Point.h"
#include "Size.h"
#include <Windows.h>
#include <string>
#include <type_traits>

namespace sw
{
    /**
     * @brief 表示一个矩形区域
     */
    struct Rect : public IToString<Rect>,
                  public IEqualityComparable<Rect> {
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
         * @brief 默认构造函数
         */
        Rect() = default;

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
        bool Equals(const Rect &other) const;

        /**
         * @brief 获取描述当前对象的字符串
         */
        std::wstring ToString() const;
    };

    // Rect应为POD类型
    static_assert(
        std::is_trivial<Rect>::value && std::is_standard_layout<Rect>::value,
        "Rect should be a POD type.");
}
