#pragma once

#include "Control.h"
#include <CommCtrl.h>

namespace sw
{
    /**
     * @brief 指定DateTimePicker控件显示的日期和时间格式
     */
    enum class DateTimePickerFormat {
        Short,  // 以短格式显示日期
        Long,   // 以长格式显示日期
        Custom, // 自定义格式
    };

    /**
     * @brief 日期和时间选取器
     */
    class DateTimePicker : public Control
    {
    private:
        /**
         * @brief 日期和时间格式
         */
        DateTimePickerFormat _format{DateTimePickerFormat::Short};

        /**
         * @brief 自定义格式字符串
         */
        std::wstring _customFormat{};

    public:
        /**
         * @brief 是否显示上下调整按钮
         */
        const Property<bool> ShowUpDownButton;

        /**
         * @brief 日期和时间格式
         */
        const Property<DateTimePickerFormat> Format;

        /**
         * @brief 自定义日期和时间格式字符串，空字符串表示默认格式
         */
        const Property<std::wstring> CustomFormat;

    public:
        /**
         * @brief 初始化DateTimePicker
         */
        DateTimePicker();

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
         * @brief 父窗口接收到WM_NOTIFY后调用发出通知控件的该函数
         */
        virtual void OnNotified(NMHDR *pNMHDR) override;

        /**
         * @brief       当前控件表示的时间改变时调用该函数
         * @param pInfo 发生改变的信息
         */
        virtual void OnTimeChanged(NMDATETIMECHANGE *pInfo);

    private:
        /**
         * @brief       设置格式字符串
         * @param value 要设置的值
         */
        void _SetFormat(const std::wstring &value);

        /**
         * @brief       修改当前控件的样式，该函数会调用ResetHandle
         * @param style 新的样式
         */
        void _UpdateStyle(DWORD style);
    };
}
