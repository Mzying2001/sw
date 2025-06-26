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
         * @brief               测量元素所需尺寸，无需考虑边框和边距
         * @param availableSize 可用的尺寸
         * @return              返回元素需要占用的尺寸
         */
        virtual Size MeasureOverride(const Size &availableSize) override;

        /**
         * @brief           安排子元素的位置，可重写该函数以实现自定义布局
         * @param finalSize 可用于排列子元素的最终尺寸
         */
        virtual void ArrangeOverride(const Size &finalSize) override;
    };
}
