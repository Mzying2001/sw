#pragma once

#include "UIElement.h"

namespace sw
{
    /**
     * @brief 将Win32 window包装为SimpleWindow对象
     * @note  与HwndHost类似，不同的是HwndWrapper不会创建容器窗口并且会接管句柄的窗口过程函数
     */
    class HwndWrapper : public UIElement
    {
    protected:
        /**
         * @brief 子类需要调用该函数以初始化HwndWrapper，该函数会调用BuildWindowCore
         */
        void InitHwndWrapper();

        /**
         * @brief           初始化HwndWrapper时会调用该函数，需在该函数中创建并返回要被托管的窗口句柄
         * @param isControl 若被托管的窗口句柄是一个控件需要将该值置为true
         * @param controlId 当isControl被设为true时，该值提供一个可用的控件id，确保不会与其他控件的id相重复
         * @return          被托管窗口句柄
         */
        virtual HWND BuildWindowCore(bool &isControl, int controlId) = 0;
    };
}
