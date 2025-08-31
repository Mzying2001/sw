#pragma once

#include "DockLayout.h"
#include "Splitter.h"

namespace sw
{
    /**
     * @brief 用于在DockLayout布局中调整停靠元素大小的分隔条
     */
    class DockSplitter : public Splitter
    {
    private:
        /**
         * @brief 基类别名，方便调用基类函数实现
         */
        using TBase = Splitter;

        /**
         * @brief 水平分隔条的鼠标样式
         */
        HCURSOR _hCurHorz = NULL;

        /**
         * @brief 垂直分隔条的鼠标样式
         */
        HCURSOR _hCurVert = NULL;

        /**
         * @brief 相关联的停靠元素
         */
        UIElement *_relatedElement = nullptr;

        /**
         * @brief 拖动开始时鼠标的位置
         */
        sw::Point _initialMousePos{};

        /**
         * @brief 拖动开始时相关联的停靠元素的位置和尺寸
         */
        sw::Size _initialRelatedElementSize{};

    public:
        /**
         * @brief 初始化DockSplitter
         */
        DockSplitter();

        /**
         * @brief             取消拖动分隔条
         * @param restoreSize 是否将相关联的元素恢复到拖动开始时的大小
         */
        void CancelDrag(bool restoreSize = false);

    protected:
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
         * @brief               接收到WM_MOUSEMOVE时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState      指示某些按键是否按下
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseMove(Point mousePosition, MouseKey keyState) override;

        /**
         * @brief            接收到WM_KILLFOCUS时调用该函数
         * @param hNextFocus 接收到焦点的hwnd，可能为NULL
         * @return           若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnKillFocus(HWND hNextFocus) override;

        /**
         * @brief       接收到WM_KEYDOWN时调用该函数
         * @param key   虚拟按键
         * @param flags 附加信息
         * @return      若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnKeyDown(VirtualKey key, KeyFlags flags) override;

        /**
         * @brief         接收到WM_SETCURSOR消息时调用该函数
         * @param hwnd    鼠标所在窗口的句柄
         * @param hitTest hit-test的结果，详见WM_NCHITTEST消息的返回值
         * @param message 触发该事件的鼠标消息，如WM_MOUSEMOVE
         * @param result  消息的返回值，默认为false
         * @return        若返回true则将result作为消息的返回值，否则使用DefaultWndProc的返回值
         */
        virtual bool OnSetCursor(HWND hwnd, HitTestResult hitTest, int message, bool &result) override;

    private:
        /**
         * @brief 当开始拖动分隔条时调用该函数
         */
        void _OnStartDrag();

        /**
         * @brief 当结束拖动分隔条时调用该函数
         */
        void _OnEndDrag(bool restoreSize);

        /**
         * @brief 当拖动分隔条时调用该函数
         */
        void _OnDragMove();

        /**
         * @brief 更新相关联的停靠元素
         */
        void _UpdateRelatedElement();

        /**
         * @brief 在指定父元素中查找指定布局标记的前一个元素
         */
        UIElement *_FindPreviousElement(UIElement *parent, int startIndex, sw::DockLayoutTag tag);
    };
}
