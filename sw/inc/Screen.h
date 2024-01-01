#pragma once

#include "Point.h"
#include "Property.h"

namespace sw
{
    /**
     * @brief 屏幕相关
     */
    class Screen
    {
    private:
        Screen() = delete;

    public:
        /**
         * @brief 屏幕宽度
         */
        static const ReadOnlyProperty<double> Width;

        /**
         * @brief 屏幕高度
         */
        static const ReadOnlyProperty<double> Height;

        /**
         * @brief 鼠标在屏幕中的位置
         */
        static const ReadOnlyProperty<Point> CursorPosition;
    };
}
