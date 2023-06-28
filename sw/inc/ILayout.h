#pragma once

#include "Rect.h"
#include "Size.h"

namespace sw
{
    /**
     * @brief 布局接口
     */
    class ILayout
    {
    public:
        /**
         * @brief 获取子控件的数量
         */
        virtual int GetChildLayoutCount() = 0;

        /**
         * @brief 获取对应索引处的子控件
         */
        virtual ILayout &GetChildLayoutAt(int index) = 0;

        /**
         * @brief 获取控件所需尺寸
         */
        virtual Size GetDesireSize() = 0;

        /**
         * @brief 设置当前控件所需的尺寸
         */
        virtual void SetDesireSize(const Size &size) = 0;

        /**
         * @brief               测量控件所需尺寸
         * @param availableSize 可用的尺寸
         */
        virtual void Measure(const Size &availableSize) = 0;

        /**
         * @brief               安排控件位置
         * @param finalPosition 最终控件所安排的位置
         */
        virtual void Arrange(const Rect &finalPosition) = 0;
    };
}
