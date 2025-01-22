#pragma once

#include "WndBase.h"

namespace sw
{
    class Timer; // 向前声明

    /**
     * @brief 计时器触发事件类型
     */
    using TimerTickHandler = std::function<void(Timer &)>;

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

        /**
         * @brief 处理函数
         */
        TimerTickHandler _handler{nullptr};

    public:
        /**
         * @brief 相对于上一次触发的Tick事件引发下一次Tick事件之间的时间（以毫秒为单位）
         */
        Property<uint32_t> Interval;

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

        /**
         * @brief         设置计时器事件处理函数
         * @param handler 处理函数
         */
        void SetTickHandler(const TimerTickHandler &handler);

        /**
         * @brief           设置成员函数为计时器事件处理函数
         * @tparam T        成员函数所在的类
         * @param obj       成员函数所在的对象
         * @param handler   处理函数
         */
        template <typename T>
        void SetTickHandler(T &obj, void (T::*handler)(Timer &))
        {
            T *p = &obj;
            this->SetTickHandler([p, handler](Timer &timer) { (p->*handler)(timer); });
        }

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
