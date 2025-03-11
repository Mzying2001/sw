#pragma once

#include "Control.h"
#include <CommCtrl.h>

namespace sw
{
    /**
     * @brief 月历控件
     */
    class MonthCalendar : public Control
    {
    public:
        /**
         * @brief 是否显示当前日期
         */
        const Property<bool> ShowToday;

    public:
        /**
         * @brief 初始化月历控件
         */
        MonthCalendar();

        /**
         * @brief     获取当前控件的“今天”部分所表示的时间
         * @param out 输出的SYSTEMTIME结构体
         * @return    若获取成功则返回true，否则返回false
         */
        bool GetToday(SYSTEMTIME &out);

        /**
         * @brief       设置当前控件的“今天”部分所表示的时间
         * @param today 要设置的时间
         * @return      若设置成功则返回true，否则返回false
         */
        bool SetToday(const SYSTEMTIME &today);

        /**
         * @brief     获取当前控件表示的时间
         * @param out 输出的SYSTEMTIME结构体
         * @return    若获取成功则返回true，否则返回false
         */
        bool GetTime(SYSTEMTIME &out);

        /**
         * @brief      设置当前控件表示的时间
         * @param time 要设置的时间
         * @return     若设置成功则返回true，否则返回false
         */
        bool SetTime(const SYSTEMTIME &time);

        /**
         * @brief         设置可选的时间段
         * @param minTime 最早时间
         * @param maxTime 最晚时间
         * @return        若设置成功则返回true，否则返回false
         */
        bool SetRange(const SYSTEMTIME &minTime, const SYSTEMTIME &maxTime);

    protected:
        /**
         * @brief     绘制虚线框时调用该函数
         * @param hdc 绘制设备句柄
         */
        virtual void OnDrawFocusRect(HDC hdc) override;

        /**
         * @brief        设置背景颜色
         * @param color  要设置的颜色
         * @param redraw 是否重绘
         */
        virtual void SetBackColor(Color color, bool redraw) override;

        /**
         * @brief        设置文本颜色
         * @param color  要设置的颜色
         * @param redraw 是否重绘
         */
        virtual void SetTextColor(Color color, bool redraw) override;

        /**
         * @brief        父窗口接收到WM_NOTIFY后且父窗口OnNotify函数返回false时调用发出通知控件的该函数
         * @param pNMHDR 包含有关通知消息的信息
         * @param result 函数返回值为true时将该值作为消息的返回值
         * @return       若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnNotified(NMHDR *pNMHDR, LRESULT &result) override;

        /**
         * @brief       当前控件表示的时间改变时调用该函数
         * @param pInfo 发生改变的信息
         */
        virtual void OnTimeChanged(NMSELCHANGE *pInfo);
    };
}
