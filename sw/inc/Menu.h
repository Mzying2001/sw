#pragma once

#include <Windows.h>

namespace sw
{
    class Menu
    {
    private:
        /**
         * @brief 菜单句柄
         */
        HMENU _hMenu = NULL;

    public:
        Menu();
    };
}
