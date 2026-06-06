#pragma once

#include "MenuItem.h"

namespace sw
{
    /**
     * @brief 菜单类型的基类
     */
    class MenuBase
    {
    private:
        /**
         * @brief 根菜单项
         */
        std::unique_ptr<MenuItem> _root;

    public:
        /**
         * @brief 根菜单项
         */
        const ReadOnlyProperty<MenuItem *> Root;

        /**
         * @brief 菜单句柄
         */
        const ReadOnlyProperty<HMENU> Handle;

    protected:
        /**
         * @brief 创建菜单实例
         */
        MenuBase(MenuItem *root);

    public:
        /**
         * @brief 析构函数
         */
        virtual ~MenuBase() = default;

        // 删除拷贝构造函数
        MenuBase(const MenuBase &) = delete;

        // 删除移动构造函数
        MenuBase(MenuBase &&) = delete;

        // 删除拷贝赋值运算符
        MenuBase &operator=(const MenuBase &) = delete;

        // 删除移动赋值运算符
        MenuBase &operator=(MenuBase &&) = delete;

    public:
        /**
         * @brief 触发菜单项点击事件
         * @param menuItemId 菜单项ID
         * @return 若事件被成功触发则返回true，否则返回false
         */
        bool RaiseClickEvent(int menuItemId);
    };

    /**
     * @brief 窗口菜单
     */
    class Menu : public MenuBase
    {
    public:
        /**
         * @brief 初始化窗口菜单
         */
        Menu();

        /**
         * @brief 初始化窗口菜单并设置菜单项
         * @param items 菜单项列表
         */
        Menu(std::initializer_list<MenuItemDesc> items);
    };

    /**
     * @brief 上下文菜单
     */
    class ContextMenu : public MenuBase
    {
    public:
        /**
         * @brief 初始化上下文菜单
         */
        ContextMenu();

        /**
         * @brief 初始化上下文菜单并设置菜单项
         * @param items 菜单项列表
         */
        ContextMenu(std::initializer_list<MenuItemDesc> items);
    };
}
