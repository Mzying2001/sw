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
        GridLayout _gridLayout{};

    public:
        /**
         * @brief 行定义
         */
        const ReadOnlyProperty<ObservableCollection<GridRow> *> Rows;

        /**
         * @brief 列定义
         */
        const ReadOnlyProperty<ObservableCollection<GridColumn> *> Columns;

    public:
        /**
         * @brief 初始化Grid
         */
        Grid();

        /**
         * @brief 设置行信息
         */
        void SetRows(std::initializer_list<GridRow> rows);

        /**
         * @brief 设置列信息
         */
        void SetColumns(std::initializer_list<GridColumn> cols);

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
        virtual LayoutHost *GetDefaultLayout() override final;

    private:
        /**
         * @brief 行或列定义变更事件处理函数
         * @param sender 事件发送者
         * @param args 事件参数
         */
        void _RowsColumnsCollectionChangedHandler(
            INotifyCollectionChanged &sender, NotifyCollectionChangedEventArgs &args);
    };
}
