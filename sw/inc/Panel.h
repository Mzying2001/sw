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
         * @brief       重写SetText以防止修改Text属性时调用SetWindowTextW设置窗口文本
         * @param value 要设置的文本
         */
        virtual void SetText(const std::wstring &value) override;

        /**
         * @brief  接收到WM_PAINT时调用该函数
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnPaint() override;

        /**
         * @brief       接收到WM_KEYDOWN时调用该函数
         * @param key   虚拟按键
         * @param flags 附加信息
         * @return      若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnKeyDown(VirtualKey key, KeyFlags flags) override;

        /**
         * @brief       接收到WM_KEYUP时调用该函数
         * @param key   虚拟按键
         * @param flags 附加信息
         * @return      若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnKeyUp(VirtualKey key, KeyFlags flags) override;

        /**
         * @brief               接收到WM_MOUSEMOVE时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState      指示某些按键是否按下
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseMove(Point mousePosition, MouseKey keyState) override;

        /**
         * @brief  接收到WM_MOUSELEAVE时调用该函数
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseLeave() override;

        /**
         * @brief               接收到WM_LBUTTONDOWN时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState      指示某些按键是否按下
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseLeftButtonDown(Point mousePosition, MouseKey keyState) override;

        /**
         * @brief               接收到WM_LBUTTONUP时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState      指示某些按键是否按下
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseLeftButtonUp(Point mousePosition, MouseKey keyState) override;

        /**
         * @brief               接收到WM_LBUTTONDBLCLK时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState      指示某些按键是否按下
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseLeftButtonDoubleClick(Point mousePosition, MouseKey keyState) override;

        /**
         * @brief          接收到WM_ENABLE时调用该函数
         * @param newValue Enabled的新值
         * @return         若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnEnabledChanged(bool newValue) override;
    };
}
