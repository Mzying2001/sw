#pragma once

#include "Control.h"
#include "Layer.h"

namespace sw
{
    class Panel : public Control, public Layer
    {
    public:
        Panel();

        /**
         * @brief      窗口句柄初始化完成
         * @param hwnd 窗口句柄
         */
        virtual void HandleInitialized(HWND hwnd) override;

        /**
         * @brief               测量控件所需尺寸
         * @param availableSize 可用的尺寸
         */
        virtual void Measure(const Size &availableSize) override;

        /**
         * @brief           安排控件位置
         * @param finalSize 最终控件所安排的位置
         */
        virtual void Arrange(const sw::Rect &finalPosition) override;

    protected:
        /**
         * @brief                   接收到WM_SETCURSOR消息时调用该函数
         * @param hwnd              鼠标所在窗口的句柄
         * @param hitTest           hit-test的结果，详见WM_NCHITTEST消息的返回值
         * @param message           触发该事件的鼠标消息，如WM_MOUSEMOVE
         * @param useDefaultWndProc 是否调用DefaultWndProc并将其返回值作为当前消息的返回值，默认为true
         * @return                  当useDefaultWndProc为false时使用该值作为消息的返回值，表示是否已处理该消息
         */
        virtual bool OnSetCursor(HWND hwnd, int hitTest, int message, bool &useDefaultWndProc) override;
    };
}
