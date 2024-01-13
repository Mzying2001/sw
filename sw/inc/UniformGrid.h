#pragma once

#include "Panel.h"
#include "UniformGridLayout.h"

namespace sw
{
    /**
     * @brief 提供一种在网格（网格中的所有单元格都具有相同的大小）中排列内容的方法
     */
    class UniformGrid : public Panel
    {
    private:
        /**
         * @brief 默认布局对象
         */
        UniformGridLayout _uniformGridLayout = UniformGridLayout();

    public:
        /**
         * @brief 行数
         */
        const Property<int> Rows;

        /**
         * @brief 列数
         */
        const Property<int> Columns;

        /**
         * @brief 网格第一行中前导空白单元格的数量
         */
        const Property<int> FirstColumn;

    public:
        /**
         * @brief 初始化UniformGrid
         */
        UniformGrid();

    protected:
        /**
         * @brief 获取默认布局对象
         */
        virtual LayoutHost *GetDefaultLayout() override;
    };
}
