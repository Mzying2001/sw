#pragma once

#include "LayoutHost.h"
#include "List.h"

namespace sw
{
    /**
     * @brief 网格布局方式的布局标记
     */
    struct GridLayoutTag {
        /**
         * @brief 所在行
         */
        uint16_t row;

        /**
         * @brief 所在列
         */
        uint16_t column;

        /**
         * @brief 所跨行数
         */
        uint16_t rowSpan;

        /**
         * @brief 所跨列数
         */
        uint16_t columnSpan;

        /**
         * @brief GridLayoutTag默认值
         */
        GridLayoutTag();

        /**
         * @brief 初始化GridLayoutTag
         */
        GridLayoutTag(uint16_t row, uint16_t column, uint16_t rowSpan, uint16_t columnSpan);

        /**
         * @brief 从LayoutTag创建
         */
        GridLayoutTag(uint64_t layoutTag);

        /**
         * @brief 隐式转换LayoutTag
         */
        operator uint64_t() const;
    };

    /**
     * @brief GridRow和GridColumn的类型
     */
    enum class GridRCType {
        FixSize,    // 固定大小
        AutoSize,   // 自动大小
        FillRemain, // 填充剩余空间
    };

    /**
     * @brief 网格中的行信息
     */
    struct GridRow {
        /**
         * @brief 类型
         */
        GridRCType type;

        /**
         * @brief 高度
         */
        double height;

        /**
         * @brief 创建一个FillRemain的GridRow
         */
        GridRow();

        /**
         * @brief 初始化GridRow
         */
        GridRow(GridRCType type, double height);

        /**
         * @brief 固定大小的行
         */
        GridRow(double height);
    };

    /**
     * @brief 固定高度的行
     */
    struct FixSizeGridRow : public GridRow {
        /**
         * @brief 初始化FixSizeGridRow
         */
        FixSizeGridRow(double height);
    };

    /**
     * @brief 自动高度的行
     */
    struct AutoSizeGridRow : public GridRow {
        /**
         * @brief 初始化AutoSizeGridRow
         */
        AutoSizeGridRow();
    };

    /**
     * @brief 填充剩余高度的行
     */
    struct FillRemainGridRow : public GridRow {
        /**
         * @brief 初始化FillRemainGridRow
         */
        FillRemainGridRow(double proportion);
    };

    /**
     * @brief 网格中的列信息
     */
    struct GridColumn {
        /**
         * @brief 类型
         */
        GridRCType type;

        /**
         * @brief 宽度
         */
        double width;

        /**
         * @brief 创建一个FillRemain的GridColumn
         */
        GridColumn();

        /**
         * @brief 初始化GridColumn
         */
        GridColumn(GridRCType type, double width);

        /**
         * @brief 固定大小的列
         */
        GridColumn(double width);
    };

    /**
     * @brief 固定宽度的列
     */
    struct FixSizeGridColumn : public GridColumn {
        /**
         * @brief 初始化FixSizeGridColumn
         */
        FixSizeGridColumn(double width);
    };

    /**
     * @brief 自动宽度的列
     */
    struct AutoSizeGridColumn : public GridColumn {
        /**
         * @brief 初始化AutoSizeGridColumn
         */
        AutoSizeGridColumn();
    };

    /**
     * @brief 填充剩余宽度的列
     */
    struct FillRemainGridColumn : public GridColumn {
        /**
         * @brief 初始化FillRemainGridColumn
         */
        FillRemainGridColumn(double proportion);
    };

    /**
     * @brief 网格布局方式
     */
    class GridLayout : public LayoutHost
    {
    private:
        /**
         * @brief 子元素的信息
         */
        struct _ChildInfo {
            ILayout *instance;         // 子元素对象
            GridLayoutTag layoutTag;   // 布局标记
            GridRCType rowMeasureType; // 元素measure行时的类型
            GridRCType colMeasureType; // 元素measure列时的类型
        };

        /**
         * @brief 行信息
         */
        struct _RowInfo {
            GridRow row;           // 行
            double size       = 0; // 所需空间大小
            double proportion = 0; // 类型为FillRemain时该字段保存该行的高度占比，范围为0~1
        };

        /**
         * @brief 列信息
         */
        struct _ColInfo {
            GridColumn col;        // 列
            double size       = 0; // 所需空间大小
            double proportion = 0; // 类型为FillRemain时该字段保存该列的宽度占比，范围为0~1
        };

        /**
         * @brief 一些内部数据
         */
        struct {
            std::vector<_RowInfo> rowsInfo;       // 行信息
            std::vector<_ColInfo> colsInfo;       // 列信息
            std::vector<_ChildInfo> childrenInfo; // 子元素信息
            std::vector<Rect> cells;              // 保存格信息
        } _internalData;

    public:
        /**
         * @brief 行定义
         */
        List<GridRow> rows;

        /**
         * @brief 列定义
         */
        List<GridColumn> columns;

        /**
         * @brief 计算所需尺寸
         */
        virtual void MeasureOverride(Size &availableSize) override;

        /**
         * @brief 安排控件
         */
        virtual void ArrangeOverride(Size &finalSize) override;

    private:
        /**
         * @brief 更新内部数据
         */
        void _UpdateInternalData();

        /**
         * @brief 获取指定行列处的网格信息
         */
        Rect &_GetCell(int row, int col);
    };
}
