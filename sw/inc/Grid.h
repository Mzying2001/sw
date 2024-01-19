#pragma once

#include "GridLayout.h"
#include "Panel.h"
#include <initializer_list>

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

        /**
         * @brief 添加行
         */
        void AddRow(const GridRow &row);

        /**
         * @brief 设置行信息
         */
        void SetRows(std::initializer_list<GridRow> rows);

        /**
         * @brief 添加列
         */
        void AddColumn(const GridColumn &col);

        /**
         * @brief 设置列信息
         */
        void SetColumns(std::initializer_list<GridColumn> cols);

        /**
         * @brief 清空行
         */
        void ClearRows();

        /**
         * @brief 清空列
         */
        void ClearColumns();

        /**
         * @brief 获取指定元素的网格布局标记
         */
        static GridLayoutTag GetGridLayoutTag(UIElement &element);

        /**
         * @brief 给指定元素设置网格布局标记
         */
        static void SetGridLayoutTag(UIElement &element, const GridLayoutTag &tag);

    protected:
        /**
         * @brief 获取默认布局对象
         */
        virtual LayoutHost *GetDefaultLayout() override;
    };
}
