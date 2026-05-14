#pragma once

#include "IComparable.h"
#include "IToString.h"
#include "Point.h"
#include "Size.h"
#include <windows.h>
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
        Rect(double left, double top, double width, double height) noexcept;

        /**
         * @brief 从RECT构造Rect
         */
        Rect(const RECT &rect) noexcept;

        /**
         * @brief 隐式转换RECT
         * @note 由于DIP↔像素之间存在非线性舍入，对从RECT构造的Rect再转回RECT
         *       不保证与原始RECT逐字段相等：right/bottom由(left+width)/(top+height)
         *       重新换算，在非整数倍DPI缩放下可能存在±1像素偏差
         */
        operator RECT() const noexcept;

        /**
         * @brief 获取Rect左上角的位置
         */
        Point GetPos() const noexcept;

        /**
         * @brief 获取Rect的尺寸
         */
        Size GetSize() const noexcept;

        /**
         * @brief 判断两个Rect是否相等
         */
        bool Equals(const Rect &other) const noexcept;

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
