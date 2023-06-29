#pragma once

#include "ILayout.h"

namespace sw
{
    class LayoutHost : public ILayout
    {
    private:
        /**
         * @brief 关联的对象
         */
        ILayout *_associatedObj = nullptr;

    public:
        void Associate(ILayout *obj);

    public:
        /**
         * @brief 获取关联对象子控件的数量
         */
        virtual int GetChildLayoutCount();

        /**
         * @brief 获取关联对象对应索引处的子控件
         */
        virtual ILayout &GetChildLayoutAt(int index);

        /**
         * @brief 获取关联对象所需尺寸
         */
        virtual Size GetDesireSize();

        /**
         * @brief 设置关联对象所需的尺寸
         */
        virtual void SetDesireSize(const Size &size);

        /**
         * @brief               测量控件所需尺寸
         * @param availableSize 可用的尺寸
         */
        virtual void Measure(const Size &availableSize);

        /**
         * @brief               安排控件位置
         * @param finalPosition 最终控件所安排的位置
         */
        virtual void Arrange(const Rect &finalPosition);

        /**
         * @brief 重写此函数计算所需尺寸
         */
        virtual void MeasureOverride(Size &availableSize) = 0;

        /**
         * @brief 重写此函数安排控件
         */
        virtual void ArrangeOverride(Size &finalSize) = 0;
    };
}
