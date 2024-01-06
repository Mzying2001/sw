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

    public:
        /**
         * @brief 边框样式
         */
        const Property<sw::BorderStyle> BorderStyle;

    public:
        /**
         * @brief 初始化面板
         */
        Panel();

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
        virtual bool OnSize(Size newClientSize) override;

        /**
         * @brief               接收到WM_MOUSEMOVE时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState      指示某些按键是否按下
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseMove(Point mousePosition, MouseKey keyState) override;
    };
}
