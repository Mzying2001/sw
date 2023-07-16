#pragma once

#include "LayoutHost.h"
#include <cstdint>

namespace sw
{
    class DockLayout : public LayoutHost
    {
    public:
        /**
         * @brief Dock布局标记
         */
        enum DockLayoutTag : uint32_t {
            Left,   // 左边
            Top,    // 顶边
            Right,  // 右边
            Bottom, // 底边
        };

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
