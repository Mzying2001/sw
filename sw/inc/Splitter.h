#pragma once

#include "Control.h"

namespace sw
{
    /**
     * @brief 分隔条
     */
    class Splitter : public Control
    {
    private:
        /**
         * @brief 记录分隔条方向
         */
        Orientation _orientation = Orientation::Horizontal;

        /**
         * @brief 是否绘制分隔条线条
         */
        bool _drawSplitterLine = true;

    public:
        /**
         * @brief 分隔条的方向，给该属性赋值同时会改变HorizontalAlignment和VerticalAlignment属性的值
         */
        const Property<Orientation> Orientation;

        /**
         * @brief 是否绘制分隔条线条
         */
        const Property<bool> DrawSplitterLine;

    public:
        /**
         * @brief 初始化分隔条
         */
        Splitter();

    protected:
        /**
         * @brief  接收到WM_PAINT时调用该函数
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnPaint() override;

        /**
         * @brief               接收到WM_SIZE时调用该函数
         * @param newClientSize 改变后的用户区尺寸
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnSize(const Size &newClientSize) override;
    };
}
