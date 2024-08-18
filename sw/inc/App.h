#pragma once

#include "Property.h"
#include <Windows.h>
#include <memory>
#include <string>

namespace sw
{
    /**
     * @brief 程序退出消息循环的方式
     */
    enum class AppQuitMode {
        Auto,   // 当所有窗口都销毁时自动退出消息循环
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
         * @brief 程序退出消息循环的方式
         */
        static const Property<AppQuitMode> QuitMode;

        /**
         * @brief  消息循环
         * @return 退出代码
         */
        static int MsgLoop();

        /**
         * @brief          退出当前消息循环
         * @param exitCode 退出代码
         */
        static void QuitMsgLoop(int exitCode = 0);
    };
}
