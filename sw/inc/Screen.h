#pragma once

#include "Point.h"
#include "Property.h"
#include "Size.h"

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
         * @brief 主屏幕宽度
         */
        static const ReadOnlyProperty<double> Width;

        /**
         * @brief 主屏幕高度
         */
        static const ReadOnlyProperty<double> Height;

        /**
         * @brief 主屏幕尺寸
         */
        static const ReadOnlyProperty<sw::Size> Size;

        /**
         * @brief 虚拟屏幕尺寸
         */
        static const ReadOnlyProperty<sw::Size> VirtualSize;

        /**
         * @brief 虚拟屏幕原点坐标
         */
        static const ReadOnlyProperty<Point> VirtualOrigin;

        /**
         * @brief 鼠标在屏幕中的位置
         */
        static const ReadOnlyProperty<Point> CursorPosition;
    };
}
