#pragma once

#include "Control.h"
#include "Layer.h"

namespace sw
{
    /**
     * @brief 面板类型的基类
     */
    class PanelBase : public Control, public Layer
    {
    public:
        /**
         * @brief 初始化PanelBase
         */
        PanelBase();

        /**
         * @brief 析构函数，这里用纯虚函数使该类成为抽象类
         */
        virtual ~PanelBase() = 0;

    protected:
        /**
         * @brief           安排元素位置
         * @param finalSize 最终元素所安排的位置
         */
        virtual void Arrange(const sw::Rect &finalPosition) override;

        /**
         * @brief               测量元素所需尺寸，无需考虑边框和边距
         * @param availableSize 可用的尺寸
         * @return              返回元素需要占用的尺寸
         */
        virtual Size MeasureOverride(const Size &availableSize) override;

        /**
         * @brief           安排子元素的位置，可重写该函数以实现自定义布局
         * @param finalSize 可用于排列子元素的最终尺寸
         */
        virtual void ArrangeOverride(const Size &finalSize) override;

        /**
         * @brief       接收到WM_VSCROLL时调用目标控件的该函数
         * @param event 事件类型，即消息wParam的低字
         * @param pos   当前滚动条的位置，仅当event为SB_THUMBPOSITION或SB_THUMBTRACK时有效
         * @return      若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnVerticalScroll(int event, int pos) override;

        /**
         * @brief       接收到WM_HSCROLL时调用目标控件的该函数
         * @param event 事件类型，即消息wParam的低字
         * @param pos   当前滚动条的位置，仅当event为SB_THUMBPOSITION或SB_THUMBTRACK时有效
         * @return      若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnHorizontalScroll(int event, int pos) override;

        /**
         * @brief        父窗口接收到WM_NOTIFY后且父窗口OnNotify函数返回false时调用发出通知控件的该函数
         * @param pNMHDR 包含有关通知消息的信息
         * @param result 函数返回值为true时将该值作为消息的返回值
         * @return       若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnNotified(NMHDR *pNMHDR, LRESULT &result) override;

        /**
         * @brief            接收到WM_KILLFOCUS时调用该函数
         * @param hNextFocus 接收到焦点的hwnd，可能为NULL
         * @return           若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnKillFocus(HWND hNextFocus) override;

        /**
         * @brief 通过tab键将焦点移动到当前元素时调用该函数
         */
        virtual void OnTabStop() override;

        /**
         * @brief 在OnPaint函数完成之后调用该函数
         */
        virtual void OnEndPaint() override;

        /**
         * @brief            尝试将指定的矩形区域移动到可视区域内
         * @param screenRect 要移动到可视区域的矩形在屏幕中的位置
         * @return           若已处理该请求则返回true，否则返回false以继续向上冒泡
         */
        virtual bool RequestBringIntoView(const sw::Rect &screenRect) override;

        /**
         * @brief           路由事件经过当前元素时调用该函数
         * @param eventArgs 事件参数
         * @param handler   事件处理函数，值为空时表示当前元素没有注册该事件处理函数
         * @return          若已处理该事件则返回true，否则返回false以继调用处理函数
         */
        virtual bool OnRoutedEvent(RoutedEventArgs &eventArgs, const RoutedEventHandler &handler) override;

    public:
        /**
         * @brief  尝试将对象转换成Control
         * @return 若函数成功则返回Control指针，否则返回nullptr
         */
        virtual Control *ToControl() override;
    };
}
