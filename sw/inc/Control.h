#pragma once

#include "Color.h"
#include "ICtlColor.h"
#include "UIElement.h"

namespace sw
{
    class Control : virtual public UIElement, public ICtlColor
    {
    private:
        Color _backColor;
        Color _textColor;

    public:
        /**
         * @brief 背景颜色
         */
        const Property<Color> BackColor;

        /**
         * @brief 文本颜色
         */
        const Property<Color> TextColor;

    public:
        Control();

        /**
         * @brief      窗口句柄初始化完成
         * @param hwnd 窗口句柄
         */
        virtual void HandleInitialized(HWND hwnd);

        /**
         * @brief 父窗口接收到WM_CTLCOLORxxx的回调
         */
        virtual LRESULT CtlColor(HDC hdc, HWND hwnd);
    };
}
