#pragma once

#include "ILayout.h"

namespace sw
{
    /**
     * @brief 用于托管元素的布局方式的对象类型，是所有布局方式类型的基类
     */
    class LayoutHost
    {
    private:
        /**
         * @brief 关联的对象
         */
        ILayout *_associatedObj = nullptr;

    public:
        /**
         * @brief 默认虚析构函数
         */
        virtual ~LayoutHost() = default;

        /**
         * @brief     设置关联的对象，每个LayoutHost只能关联一个对象
         * @param obj 要关联的对象
         */
        void Associate(ILayout *obj);

        /**
         * @brief     判断当前LayoutHost是否关联了对象
         * @param obj 若传入值为nullptr，则判断是否有任何对象关联，否则判断是否关联了指定对象
         */
        bool IsAssociated(ILayout *obj = nullptr);

        /**
         * @brief 获取关联对象子控件的数量
         */
        int GetChildLayoutCount();

        /**
         * @brief 获取关联对象对应索引处的子控件
         */
        ILayout &GetChildLayoutAt(int index);

    public:
        /**
         * @brief               测量元素所需尺寸，无需考虑边框和边距
         * @param availableSize 可用的尺寸
         * @return              返回元素需要占用的尺寸
         */
        virtual Size MeasureOverride(const Size &availableSize) = 0;

        /**
         * @brief           安排子元素的位置，可重写该函数以实现自定义布局
         * @param finalSize 可用于排列子元素的最终尺寸
         */
        virtual void ArrangeOverride(const Size &finalSize) = 0;
    };
}
