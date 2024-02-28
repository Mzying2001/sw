#pragma once

#include "LayoutHost.h"
#include <cstdint>

namespace sw
{
    /**
     * @brief 停靠布局标记
     */
    class DockLayoutTag
    {
    public:
        static constexpr uint64_t Left   = 0; // 左边
        static constexpr uint64_t Top    = 1; // 顶边
        static constexpr uint64_t Right  = 2; // 右边
        static constexpr uint64_t Bottom = 3; // 底边

    private:
        uint64_t _value; // 值

    public:
        /**
         * @brief 创建DockLayoutTag
         */
        inline DockLayoutTag(uint64_t value)
            : _value(value)
        {
        }

        /**
         * @brief 隐式转换uint64_t
         */
        inline operator uint64_t() const
        {
            return _value;
        }

        /**
         * @brief 判断值是否相等
         */
        friend inline bool operator==(const DockLayoutTag &left, const DockLayoutTag &right)
        {
            return left._value == right._value;
        }
    };

    /**
     * @brief 停靠布局
     */
    class DockLayout : public LayoutHost
    {
    public:
        /**
         * @brief 最后一个子元素是否填充剩余空间
         */
        bool lastChildFill = true;

        /**
         * @brief 计算所需尺寸
         */
        virtual void MeasureOverride(Size &availableSize) override;

        /**
         * @brief 安排控件
         */
        virtual void ArrangeOverride(Size &finalSize) override;
    };
}
