#pragma once

#include "Delegate.h"
#include "Property.h"
#include <windows.h>
#include <memory>
#include <string>

namespace sw
{
    /**
     * @brief 线程退出消息循环的方式
     */
    enum class AppQuitMode {
        Auto,   // 线程中所有窗口都销毁时自动退出消息循环
        Manual, // 需手动调用QuitMsgLoop以退出消息循环
    };

    /**
     * @brief App类
     */
    class App
    {
    private:
        App() = delete;

    public:
        /**
         * @brief 应用程序的当前实例的句柄
         */
        static const ReadOnlyProperty<HINSTANCE> Instance;

        /**
         * @brief 当前exe的文件路径
         */
        static const ReadOnlyProperty<std::wstring> ExePath;

        /**
         * @brief 当前exe所在的文件夹路径
         */
        static const ReadOnlyProperty<std::wstring> ExeDirectory;

        /**
         * @brief 当前工作路径
         */
        static const Property<std::wstring> CurrentDirectory;

        /**
         * @brief 当前线程退出消息循环的方式
         * @note  该属性是线程局部的，每个线程有各自独立的值
         */
        static const Property<AppQuitMode> QuitMode;

        /**
         * @brief 当前线程消息循环中处理空句柄消息的回调函数
         * @note  该委托是线程局部的，每个线程有各自独立的值
         */
        static thread_local Action<MSG &> NullHwndMsgHandler;

        /**
         * @brief  消息循环
         * @return 退出代码
         */
        static int MsgLoop();

        /**
         * @brief          退出当前线程的消息循环
         * @param exitCode 退出代码
         */
        static void QuitMsgLoop(int exitCode = 0);
    };
}
