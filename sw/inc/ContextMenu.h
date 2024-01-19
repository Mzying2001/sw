#pragma once

#include "MenuBase.h"

namespace sw
{
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
         */
        ContextMenu(std::initializer_list<MenuItem> items);

        /**
         * @brief    判断ID是否为上下文菜单项的ID
         * @param id 要判断的ID
         * @return   ID是否为上下文菜单项的ID
         */
        static bool IsContextMenuID(int id);

    protected:
        /**
         * @brief       根据索引获取ID
         * @param index 索引
         * @return      菜单项的ID
         */
        virtual int IndexToID(int index) override;

        /**
         * @brief    根据ID获取索引
         * @param id 菜单项的ID
         * @return   索引
         */
        virtual int IDToIndex(int id) override;
    };
}
