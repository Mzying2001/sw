#pragma once

#include "GridLayout.h"
#include "Panel.h"

namespace sw
{
    /**
     * @brief 由列和行组成的灵活的网格区域
     */
    class Grid : public Panel
    {
    private:
        /**
         * @brief 默认布局对象
         */
        GridLayout _gridLayout = GridLayout();

    public:
        /**
         * @brief 初始化Grid
         */
        Grid();

    protected:
        /**
         * @brief 获取默认布局对象
         */
        virtual LayoutHost *GetDefaultLayout() override;
    };
}
