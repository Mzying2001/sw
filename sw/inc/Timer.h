#pragma once

#include "WndBase.h"

namespace sw
{
    class Timer; // 向前声明

    /**
     * @brief 计时器触发事件类型
     */
    using TimerTickHandler = Action<Timer &>;

    /**
     * @brief 计时器
     */
    class Timer : public WndBase
    {
    private:
        /**
         * @brief 是否已启动
         */
        bool _started = false;

        /**
         * @brief 触发间隔
         */
        uint32_t _interval = 1000;

    public:
        /**
         * @brief 相对于上一次触发的Tick事件引发下一次Tick事件之间的时间（以毫秒为单位）
         */
        Property<uint32_t> Interval;

        /**
         * @brief 计时器触发事件
         */
        TimerTickHandler Tick;

    public:
        /**
         * @brief 初始化计时器
         */
        Timer();

        /**
         * @brief 开始计时器
         */
        void Start();

        /**
         * @brief 停止计时器
         */
        void Stop();

    protected:
        /**
         * @brief 计时器已启动并且达到间隔时间时调用该函数
         */
        virtual void OnTick();

    private:
        /**
         * @brief      通过窗口句柄获取Timer指针
         * @param hwnd 窗口句柄
         * @return     若函数成功则返回对象的指针，否则返回nullptr
         */
        static Timer *_GetTimerPtr(HWND hwnd);

        /**
         * @brief       关联窗口句柄与Timer对象
         * @param hwnd  窗口句柄
         * @param timer 与句柄关联的对象
         */
        static void _SetTimerPtr(HWND hwnd, Timer &timer);

        /**
         * @brief TimerProc回调函数
         */
        static void CALLBACK _TimerProc(HWND hwnd, UINT msg, UINT_PTR idTimer, DWORD time);
    };
}
