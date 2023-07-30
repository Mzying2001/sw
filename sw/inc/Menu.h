#pragma once

#include "MenuItem.h"
#include <Windows.h>
#include <initializer_list>
#include <memory>
#include <vector>

namespace sw
{
    class Menu
    {
    private:
        /**
         * @brief 菜单句柄
         */
        HMENU _hMenu;

        /**
         * @brief 储存所有子项菜单句柄
         */
        std::vector<HMENU> _popupMenus;

        /**
         * @brief 储存所有叶子节点，即可以被单击的菜单项，索引为其id
         */
        std::vector<std::shared_ptr<MenuItem>> _leaves;

    public:
        /**
         * @brief 菜单项集合
         */
        std::vector<std::shared_ptr<MenuItem>> items;

        /**
         * @brief 初始化菜单
         */
        Menu();

        /**
         * @brief 重载拷贝构造
         */
        Menu(const Menu &menu);

        /**
         * @brief 初始化菜单并添加菜单项
         */
        Menu(std::initializer_list<MenuItem> items);

        /**
         * @brief 释放资源
         */
        ~Menu();

        /**
         * @brief 获取菜单句柄
         */
        HMENU GetHandle();

        /**
         * @brief 更新菜单项
         */
        void Update();

        /**
         * @brief    通过id获取菜单项
         * @param id 要获取菜单项的id
         * @return   若函数成功则返回菜单项的指针，否则返回nullptr
         */
        MenuItem *GetMenuItem(int id);

        /**
         * @brief      通过索引来获取菜单项
         * @param path 要找项所在下索引
         * @return     若函数成功则返回菜单项的指针，否则返回nullptr
        */
        MenuItem *GetMenuItem(std::initializer_list<int> path);

        /**
         * @brief      通过菜单项的text来获取菜单项
         * @param path 每层要找的text
         * @return     若函数成功则返回菜单项的指针，否则返回nullptr
        */
        MenuItem *GetMenuItem(std::initializer_list<std::wstring> path);

    private:
        /**
         * @brief 清除已添加的所有菜单项
         */
        void ClearAddedItems();

        /**
         * @brief 添加菜单项到指定句柄
         */
        void AppendMenuItem(HMENU hMenu, std::shared_ptr<MenuItem> pItem);

        /**
         * @brief 触发菜单项所绑定的回调函数
         */
        void RaiseMenuItemCommand(MenuItem &item);
    };
}
