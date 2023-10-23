#pragma once

#include "Color.h"
#include "Cursor.h"
#include "ICtlColor.h"
#include "UIElement.h"

namespace sw
{
    /**
     * @brief 控件
     */
    class Control : virtual public UIElement, public ICtlColor
    {
    private:
        /**
         * @brief 背景颜色
         */
        Color _backColor = Color::White;

        /**
         * @brief 文本颜色
         */
        Color _textColor = Color::Black;

        /**
         * @brief 是否使用默认的鼠标样式
         */
        bool _useDefaultCursor = true;

        /**
         * @brief 鼠标句柄
         */
        HCURSOR _hCursor = NULL;

    public:
        /**
         * @brief 背景颜色，该属性对部分控件无效
         */
        const Property<Color> BackColor;

        /**
         * @brief 文本颜色，该属性对部分控件无效
         */
        const Property<Color> TextColor;

    public:
        /**
         * @brief 初始化控件
         */
        Control();

    protected:
        /**
         * @brief 销毁控件句柄并重新初始化，该操作会创建新的句柄并设置样式、文本、字体等
         */
        void ResetHandle();

        /**
         * @brief 控件句柄发生改变时调用该函数
         */
        virtual void HandleChenged();

        /**
         * @brief        设置背景颜色
         * @param color  要设置的颜色
         * @param redraw 是否重绘
         */
        virtual void SetBackColor(Color color, bool redraw);

        /**
         * @brief        设置文本颜色
         * @param color  要设置的颜色
         * @param redraw 是否重绘
         */
        virtual void SetTextColor(Color color, bool redraw);

        /**
         * @brief                   接收到WM_SETCURSOR消息时调用该函数
         * @param hwnd              鼠标所在窗口的句柄
         * @param hitTest           hit-test的结果，详见WM_NCHITTEST消息的返回值
         * @param message           触发该事件的鼠标消息，如WM_MOUSEMOVE
         * @param useDefaultWndProc 是否调用DefaultWndProc并将其返回值作为当前消息的返回值，默认为true
         * @return                  当useDefaultWndProc为false时使用该值作为消息的返回值，表示是否已处理该消息
         */
        virtual bool OnSetCursor(HWND hwnd, int hitTest, int message, bool &useDefaultWndProc) override;

    public:
        /**
         * @brief 父窗口接收到WM_CTLCOLORxxx的回调
         */
        virtual LRESULT CtlColor(HDC hdc, HWND hwnd) override;

        /**
         * @brief         设置鼠标样式
         * @param hCursor 鼠标句柄
         */
        void SetCursor(HCURSOR hCursor);

        /**
         * @brief        设置鼠标样式
         * @param cursor 鼠标样式
         */
        void SetCursor(StandardCursor cursor);

        /**
         * @brief 将鼠标样式设置为默认样式
         */
        void ResetCursor();
    };
}
