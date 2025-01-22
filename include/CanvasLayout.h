#pragma once

#include "LayoutHost.h"

namespace sw
{
    /**
     * @brief 绝对位置布局方式的布局标记
     */
    struct CanvasLayoutTag {
        /**
         * @brief 左边
         */
        float left;

        /**
         * @brief 顶边
         */
        float top;

        /**
         * @brief 左边顶边均为0
         */
        CanvasLayoutTag();

        /**
         * @brief 指定左边和顶边
         */
        CanvasLayoutTag(float left, float top);

        /**
         * @brief 从LayoutTag创建
         */
        CanvasLayoutTag(uint64_t layoutTag);

        /**
         * @brief 隐式转换LayoutTag
         */
        operator uint64_t() const;
    };

    /**
     * @brief 绝对位置布局方式
     */
    class CanvasLayout : public LayoutHost
    {
    public:
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
