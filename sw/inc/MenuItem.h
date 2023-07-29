#pragma once

#include <Windows.h>

namespace sw
{
    class MenuItem
    {
    private:
        /**
         * @brief 菜单项句柄
         */
        HMENU _hMenu = NULL;

    public:
        MenuItem();
    };
}
