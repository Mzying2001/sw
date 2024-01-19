#pragma once

#include "MenuBase.h"

namespace sw
{
    /**
     * @brief 菜单
     */
    class Menu : public MenuBase
    {
    public:
        /**
         * @brief 初始化菜单
         */
        Menu();

        /**
         * @brief 初始化菜单并设置菜单项
         */
        Menu(std::initializer_list<MenuItem> items);

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
