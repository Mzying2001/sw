#pragma once

#include "Control.h"
#include <CommCtrl.h>

namespace sw
{
    /**
     * @brief 滑块控件
     */
    class Slider : public Control
    {
    public:
        /**
         * @brief 最小值
         */
        const Property<int> Minimum;

        /**
         * @brief 最大值
         */
        const Property<int> Maximum;

        /**
         * @brief 当前滑块的值
         */
        const Property<int> Value;

        /**
         * @brief 是否显示为垂直滑块
         */
        const Property<bool> Vertical;

        /**
         * @brief 是否在滑动时显示数值提示
         */
        const Property<bool> ValueTooltips;

    public:
        /**
         * @brief 初始化滑块控件
         */
        Slider();

    protected:
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
         * @brief 滑块的值改变时调用该函数
         */
        virtual void OnValueChanged();

        /**
         * @brief 释放滑块时调用该函数
         */
        virtual void OnEndTrack();
    };
}
