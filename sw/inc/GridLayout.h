#pragma once

#include "LayoutHost.h"

namespace sw
{
    /**
     * @brief Grid布局方式的布局标记
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
     * @brief Grid的行定义
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
         * @brief 初始化GridRow
         */
        GridRow(GridRCType type, double height);

        /**
         * @brief 固定大小的行
         */
        GridRow(double height);
    };

    /**
     * @brief Grid的列定义
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
         * @brief 初始化GridColumn
         */
        GridColumn(GridRCType type, double width);

        /**
         * @brief 固定大小的列
         */
        GridColumn(double width);
    };

    /**
     * @brief Grid布局方式
     */
    class GridLayout : public LayoutHost
    {
    public:
        /**
         * @brief 计算所需尺寸
         */
        virtual void MeasureOverride(Size &availableSize) override;

        /**
         * @brief 安排控件
         */
        virtual void ArrangeOverride(Size &finalSize) override;
    };
}
