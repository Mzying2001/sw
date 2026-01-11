#pragma once

#include <windows.h>

namespace sw
{
    /**
     * @brief NcHitTest（WM_NCHITTEST）的返回值
     */
    enum class HitTestResult {
        HitBorder      = HTBORDER,      // 在没有大小边界的窗口边框中。
        HitBottom      = HTBOTTOM,      // 在可调整大小的窗口的下水平边框中（用户可以单击鼠标以垂直调整窗口大小）。
        HitBottomLeft  = HTBOTTOMLEFT,  // 在可调整大小的窗口的边框左下角（用户可以单击鼠标以对角线调整窗口大小）。
        HitBottomRight = HTBOTTOMRIGHT, // 在可调整大小的窗口的边框右下角（用户可以单击鼠标以对角线调整窗口大小）。
        HitCaption     = HTCAPTION,     // 在标题栏中。
        HitClient      = HTCLIENT,      // 在客户端区中。
        HitClose       = HTCLOSE,       // 在关闭按钮中。
        HitError       = HTERROR,       // 在屏幕背景上或窗口之间的分割线上（与 HTNOWHERE 相同，只是 DefWindowProc 函数会生成系统蜂鸣音以指示错误）。
        HitGrowBox     = HTGROWBOX,     // 在大小框中（与 HTSIZE 相同）。
        HitHelp        = HTHELP,        // 在帮助按钮中。
        HitHScroll     = HTHSCROLL,     // 在水平滚动条中。
        HitLeft        = HTLEFT,        // 在可调整大小的窗口的左边框中（用户可以单击鼠标以水平调整窗口大小）。
        HitMenu        = HTMENU,        // 在菜单中。
        HitMaxButton   = HTMAXBUTTON,   // 在最大化按钮中。
        HitMinButton   = HTMINBUTTON,   // 在最小化按钮中。
        HitNoWhere     = HTNOWHERE,     // 在屏幕背景上，或在窗口之间的分隔线上。
        HitReduce      = HTREDUCE,      // 在最小化按钮中。
        HitRight       = HTRIGHT,       // 在可调整大小的窗口的右左边框中（用户可以单击鼠标以水平调整窗口大小）。
        HitSize        = HTSIZE,        // 在大小框中（与 HTGROWBOX 相同）。
        HitSysMenu     = HTSYSMENU,     // 在窗口菜单或子窗口的关闭按钮中。
        HitTop         = HTTOP,         // 在窗口的上水平边框中。
        HitTopLeft     = HTTOPLEFT,     // 在窗口边框的左上角。
        HitTopRight    = HTTOPRIGHT,    // 在窗口边框的右上角。
        HitTransparent = HTTRANSPARENT, // 在同一线程当前由另一个窗口覆盖的窗口中（消息将发送到同一线程中的基础窗口，直到其中一个窗口返回不是 HTTRANSPARENT 的代码）。
        HitVScroll     = HTVSCROLL,     // 在垂直滚动条中。
        HitZoom        = HTZOOM,        // 在最大化按钮中。
    };
}
