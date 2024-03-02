#pragma once

#include "StaticControl.h"

namespace sw
{
    /**
     * @brief 将Win32 window托管为SimpleWindow控件
     */
    class HwndHost : public StaticControl
    {
    private:
        /**
         * @brief 托管的句柄
         */
        HWND _hWindowCore{NULL};

    public:
        /**
         * @brief 创建HwndHost对象
         */
        HwndHost();

    protected:
        /**
         * @brief 子类需要调用该函数以初始化HwndHost，该函数会调用BuildWindowCore
         */
        void InitHwndHost();

        /**
         * @brief               接收到WM_SIZE时调用该函数
         * @param newClientSize 改变后的用户区尺寸
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnSize(Size newClientSize) override;

        /**
         * @brief  接收到WM_DESTROY时调用该函数
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnDestroy() override;

        /**
         * @brief         HwndHost创建时会调用该函数，需在该函数中创建要被托管的窗口句柄，设置其父窗口并返回被托管的句柄
         * @param hParent 需要给被托管窗口句柄设置的父窗口句柄
         * @return        被托管窗口句柄
         */
        virtual HWND BuildWindowCore(HWND hParent) = 0;

        /**
         * @brief      HwndHost被销毁时会调用该函数来销毁被托管的窗口句柄
         * @param hwnd 被托管窗口句柄
         */
        virtual void DestroyWindowCore(HWND hwnd) = 0;
    };
}
