#pragma once

#include "WndBase.h"
#include <commctrl.h>

namespace sw
{
    /**
     * @brief 提示信息图标类型
     */
    enum class ToolTipIcon {
        None         = TTI_NONE,                // 无图标
        Info         = TTI_INFO,                // 信息图标
        Warning      = TTI_WARNING,             // 警告图标
        Error        = TTI_ERROR,               // 错误图标
        LargeInfo    = 4 /*TTI_INFO_LARGE*/,    // 大错误图标
        LargeWarning = 5 /*TTI_WARNING_LARGE*/, // 大错误图标
        LargeError   = 6 /*TTI_ERROR_LARGE*/,   // 大错误图标
    };

    /**
     * @brief 信息提示，用于用户将指针移过关联控件时显示信息
     */
    class ToolTip : public WndBase
    {
    private:
        /**
         * @brief 图标
         */
        ToolTipIcon _icon{ToolTipIcon::None};

        /**
         * @brief 标题
         */
        std::wstring _title{};

    public:
        /**
         * @brief 触发提示信息的时间，以毫秒为单位，设置负数可恢复默认值
         */
        const Property<int> InitialDelay;

        /**
         * @brief 提示框中显示的图标，标题不为空时图标才会显示
         */
        const Property<ToolTipIcon> ToolTipIcon;

        /**
         * @brief 提示框中显示的标题
         */
        const Property<std::wstring> ToolTipTitle;

        /**
         * @brief 提示框的最大宽度，若未设置则为-1，设置负值可取消限制
         */
        const Property<double> MaxTipWidth;

    public:
        /**
         * @brief 初始化ToolTip
         */
        ToolTip();

        /**
         * @brief 初始化ToolTip，指定窗口样式
         */
        explicit ToolTip(DWORD style);

    public:
        /**
         * @brief         给指定句柄设置提示信息
         * @param hwnd    要设置提示信息的句柄
         * @param tooltip 提示信息
         */
        bool SetToolTip(HWND hwnd, const std::wstring &tooltip);

        /**
         * @brief         给指定窗口或控件设置提示信息
         * @param wnd     要设置提示信息的窗口或控件
         * @param tooltip 提示信息
         */
        bool SetToolTip(const WndBase &wnd, const std::wstring &tooltip);

        /**
         * @brief 移除所有关联的提示信息
         */
        void RemoveAll();

    private:
        /**
         * @brief 更新图标和标题
         */
        void _UpdateIconAndTitle();
    };

    /**
     * @brief 气泡样式的信息提示
     */
    class BallonToolTip : public ToolTip
    {
    public:
        /**
         * @brief 初始化BallonToolTip
         */
        BallonToolTip();
    };
}
