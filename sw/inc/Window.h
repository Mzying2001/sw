#pragma once

#include "Color.h"
#include "Screen.h"
#include "UIElement.h"

namespace sw
{
    class Window : public UIElement
    {
    public:
        enum WindowState {
            Normal,    // 普通窗口
            Minimized, // 最小化窗口
            Maximized, // 最大化窗口
        };

        enum WindowStartupLocation {
            Manual,       // 使用系统默认或手动设置
            CenterScreen, // 屏幕中心
        };

    private:
        bool _isFirstShow = true;
        Color _background = Color::White;

    public:
        /**
         * @brief 是否在关闭所有窗口后退出程序
         */
        static bool PostQuitWhenAllClosed;

        /**
         * @brief 窗口状态
         */
        const Property<WindowState> State;

        /**
         * @brief 窗口是否可调整大小
         */
        const Property<bool> SizeBox;

        /**
         * @brief 最大化按钮是否可用
         */
        const Property<bool> MaximizeBox;

        /**
         * @brief 最小化按钮是否可用
         */
        const Property<bool> MinimizeBox;

        /**
         * @brief 窗口是否置顶
         */
        const Property<bool> Topmost;

        /**
         * @brief 是否显示为ToolWindow (窄边框)
         */
        const Property<bool> ToolWindow;

        /**
         * @brief 背景颜色
         */
        const Property<Color> Background;

        /**
         * @brief 最大宽度，当值不大于0时表示不限制
         */
        double MaxWidth = -1;

        /**
         * @brief 最大高度，当值不大于0时表示不限制
         */
        double MaxHeight = -1;

        /**
         * @brief 最小宽度，当值不大于0时表示不限制
         */
        double MinWidth = -1;

        /**
         * @brief 最小高度，当值不大于0时表示不限制
         */
        double MinHeight = -1;

        /**
         * @brief 窗口初次启动的位置
         */
        WindowStartupLocation StartupLocation = Manual;

    public:
        Window();

    private:
        bool GetStyle(LONG_PTR style);
        void SetStyle(LONG_PTR style, bool value);
        bool GetExtendedStyle(LONG_PTR style);
        void SetExtendedStyle(LONG_PTR style, bool value);

    protected:
        /**
         * @brief 对WndProc的封装
         */
        virtual LRESULT WndProc(const ProcMsg &refMsg);

        /**
         * @brief  接收到WM_CLOSE时调用该函数
         * @return 若已处理该消息则返回true，否则返回false以调用DefWindowProcW
         */
        virtual bool OnClose();

        /**
         * @brief  接收到WM_CREATE时调用该函数
         * @return 若已处理该消息则返回true，否则返回false以调用DefWindowProcW
         */
        virtual bool OnCreate();

        /**
         * @brief  接收到WM_DESTROY时调用该函数
         * @return 若已处理该消息则返回true，否则返回false以调用DefWindowProcW
         */
        virtual bool OnDestroy();

        /**
         * @brief  接收到WM_PAINT时调用该函数
         * @return 若已处理该消息则返回true，否则返回false以调用DefWindowProcW
         */
        virtual bool OnPaint();

    public:
        /**
         * @brief 显示窗口
         */
        void Show();
    };
}
