#pragma once

#include "MenuItem.h"
#include <Windows.h>
#include <vector>

namespace sw
{
    class Menu
    {
    private:
        /**
         * @brief 菜单句柄
         */
        HMENU _hMenu = NULL;

        /**
         * @brief 包含所有子项，索引为子项的id
         */
        std::vector<MenuItem *> _itemMap;

    public:
        /**
         * @brief 菜单项
         */
        std::vector<MenuItem> items;

    public:
        Menu();

        /**
         * @brief 获取菜单句柄
         */
        HMENU GetMenuHandle();

        /**
         * @brief 更新菜单
         */
        void Update();

    private:
        /**
         * @brief 清空菜单
         */
        void Clear();

        /**
         * @brief 递归添加子项
         */
        void AppendSubItems(HMENU hMenu, MenuItem &item);
    };
}
