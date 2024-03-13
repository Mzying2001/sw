#pragma once

#include "HwndHost.h"
#include <CommCtrl.h>

namespace sw
{
    /**
     * @brief IP地址框
     */
    class IPAddressControl : public HwndHost
    {
    private:
        /**
         * @brief IP地址框的句柄
         */
        HWND _hIPAddrCtrl{NULL};

    public:
        /**
         * @brief 内容是否为空
         */
        const ReadOnlyProperty<bool> IsBlank;

        /**
         * @brief 当前内容所表示的IP地址
         */
        const Property<uint32_t> Address;

    public:
        /**
         * @brief 初始化IP地址框
         */
        IPAddressControl();

        /**
         * @brief 清空输入的内容
         */
        void Clear();

        /**
         * @brief       限制某个IP段的值范围
         * @param field 要限制的IP段的索引
         * @param min   最小值
         * @param max   最大值
         * @return      若函数成功则返回true，否则返回false
         */
        bool SetRange(int field, uint8_t min, uint8_t max);

    protected:
        /**
         * @brief         HwndHost创建时会调用该函数，需在该函数中创建要被托管的窗口句柄，设置其父窗口并返回被托管的句柄
         * @param hParent 需要给被托管窗口句柄设置的父窗口句柄
         * @return        被托管窗口句柄
         */
        virtual HWND BuildWindowCore(HWND hParent) override;

        /**
         * @brief      HwndHost被销毁时会调用该函数来销毁被托管的窗口句柄
         * @param hwnd 被托管窗口句柄
         */
        virtual void DestroyWindowCore(HWND hwnd) override;

        /**
         * @brief       字体改变时调用该函数
         * @param hfont 字体句柄
         */
        virtual void FontChanged(HFONT hfont) override;

        /**
         * @brief            接收到WM_SETFOCUS时调用该函数
         * @param hPrevFocus 丢失焦点的hwnd，可能为NULL
         * @return           若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnSetFocus(HWND hPrevFocus) override;
    };
}
