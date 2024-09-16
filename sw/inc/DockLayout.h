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
        enum : uint64_t {
            Left,   // 左边
            Top,    // 顶边
            Right,  // 右边
            Bottom, // 底边
        };

    private:
        /**
         * @brief Tag值
         */
        uint64_t _value;

    public:
        /**
         * @brief 创建DockLayoutTag
         */
        DockLayoutTag(uint64_t value = Left);

        /**
         * @brief 隐式转换uint64_t
         */
        operator uint64_t() const;

        /**
         * @brief 判断值是否相等
         */
        bool operator==(const DockLayoutTag &other) const;

        /**
         * @brief 判断值是否不相等
         */
        bool operator!=(const DockLayoutTag &other) const;

        /**
         * @brief 判断值是否相等
         */
        bool operator==(uint64_t value) const;

        /**
         * @brief 判断值是否不相等
         */
        bool operator!=(uint64_t value) const;
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
