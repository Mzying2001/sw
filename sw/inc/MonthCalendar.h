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
         * @brief 绘制虚线框时调用该函数
         */
        virtual void OnDrawFocusRect() override;

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
    };
}
