#pragma once

#include "Property.h"
#include <Windows.h>

namespace sw
{
    /**
     * @brief 用于处理设备独立像素（dip）与屏幕像素之间的转换
     */
    class Dip
    {
    private:
        Dip() = delete;

    public:
        /**
         * @brief 水平缩放比例
         */
        static const ReadOnlyProperty<double> ScaleX;

        /**
         * @brief 垂直缩放比例
         */
        static const ReadOnlyProperty<double> ScaleY;

        /**
         * @brief dpi改变时调用该函数更新缩放比例
         */
        static void Update(int dpiX, int dpiY);

        /**
         * @brief 像素转dip（水平方向）
         */
        static double PxToDipX(int px);

        /**
         * @brief 像素转dip（垂直方向）
         */
        static double PxToDipY(int px);

        /**
         * @brief dip转像素（水平方向）
         */
        static int DipToPxX(double dip);

        /**
         * @brief dip转像素（垂直方向）
         */
        static int DipToPxY(double dip);
    };
}
