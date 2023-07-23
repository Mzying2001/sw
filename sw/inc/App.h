#pragma once

#include "Property.h"
#include <Windows.h>
#include <string>

namespace sw
{
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
         * @brief  消息循环
         * @return 退出代码
         */
        static int MsgLoop();

        /**
         * @brief          退出程序
         * @param exitCode 退出代码
         */
        static void Quit(int exitCode = 0);

    private:
        /**
         * @brief  获取当前exe文件路径
         */
        static std::wstring _GetExePath();
    };
}
