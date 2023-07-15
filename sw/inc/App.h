#pragma once

#include "Property.h"
#include <Windows.h>

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
         * @brief  消息循环
         * @return 退出代码
         */
        static int MsgLoop();

        /**
         * @brief          退出程序
         * @param exitCode 退出代码
         */
        static void Quit(int exitCode = 0);
    };
}
