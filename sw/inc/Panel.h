#pragma once

#include "PanelBase.h"

namespace sw
{
    /**
     * @brief 边框类型
     */
    enum class BorderStyle {
        None   = 0,           // 无边框
        Bump   = EDGE_BUMP,   // 突出的凸起边框
        Etched = EDGE_ETCHED, // 刻痕式边框
        Raised = EDGE_RAISED, // 凸起边框
        Sunked = EDGE_SUNKEN, // 凹陷边框
    };

    /**
     * @brief 面板
     */
    class Panel : public PanelBase
    {
    private:
        /**
         * @brief 边框类型，默认为无边框
         */
        BorderStyle _borderStyle = sw::BorderStyle::None;

        /**
         * @brief 内边距
         */
        Thickness _padding;

    public:
        /**
         * @brief 边框样式
         */
        const Property<sw::BorderStyle> BorderStyle;

        /**
         * @brief 面板的内边距
         */
        const Property<sw::Thickness> Padding;

    public:
        /**
         * @brief 初始化面板
         */
        Panel();

    protected:
        /**
         * @brief 更新边框
         */
        void UpdateBorder();

        /**
         * @brief 对WndProc的封装
         */
        virtual LRESULT WndProc(const ProcMsg &refMsg) override;

        /**
         * @brief        接收到WM_ERASEBKGND时调用该函数
         * @param hdc    设备上下文句柄
         * @param result 若已处理该消息则设为非零值，默认值为0
         * @return       若返回true则将result作为消息的返回值，否则使用DefaultWndProc的返回值
         */
        virtual bool OnEraseBackground(HDC hdc, LRESULT &result) override;

        /**
         * @brief  接收到WM_PAINT时调用该函数
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnPaint() override;

        /**
         * @brief      接收到WM_NCPAINT时调用该函数
         * @param hRgn 窗口更新区域的句柄，可能为NULL
         * @return     若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnNcPaint(HRGN hRgn) override;

        /**
         * @brief      绘制边框
         * @param hdc  绘制设备句柄，可能为NULL
         * @param rect 绘制边框的矩形区域，该函数会减去边框厚度
         * @note       若hdc为NULL则不进行绘制，只更新rect
         */
        virtual void OnDrawBorder(HDC hdc, RECT &rect);

        /**
         * @brief      绘制内边距
         * @param hdc  绘制设备句柄，可能为NULL
         * @param rect 绘制内边距的矩形区域，该函数会减去内边距
         * @note       若hdc为NULL则不进行绘制，只更新rect
         */
        virtual void OnDrawPadding(HDC hdc, RECT &rect);
    };
}
