#pragma once

#include "ItemsControl.h"
#include "List.h"
#include <CommCtrl.h>

namespace sw
{
    /**
     * @brief 列表视图的列对齐方式
     */
    enum class ListViewColumnAlignment {
        Left   = LVCFMT_LEFT,   // 左对齐
        Right  = LVCFMT_RIGHT,  // 右对齐
        Center = LVCFMT_CENTER, // 居中
    };

    /**
     * @brief 列表视图的列信息
     */
    struct ListViewColumn {
        /**
         * @brief 列标题
         */
        std::wstring header;

        /**
         * @brief 列宽度
         */
        double width;

        /**
         * @brief 对齐方式
         */
        ListViewColumnAlignment alignment = ListViewColumnAlignment::Left;

        ListViewColumn(const std::wstring &header);
        ListViewColumn(const std::wstring &header, double width);
        ListViewColumn(const LVCOLUMNW &lvc);
        operator LVCOLUMNW() const;
    };

    /**
     * @brief 列表视图
     */
    class ListView : public ItemsControl<StrList>
    {
    public:
        /**
         * @brief 列数
         */
        const ReadOnlyProperty<int> ColumnsCount;

        /**
         * @brief 是否显示网格线
         */
        const Property<bool> GridLines;

        /**
         * @brief 是否允许多选
         */
        const Property<bool> MultiSelect;

    public:
        /**
         * @brief 初始化ListView
         */
        ListView();

    protected:
        /**
         * @brief  获取子项数
         */
        virtual int GetItemsCount() override;

        /**
         * @brief 选中项的索引，当无选中项时为-1
         */
        virtual int GetSelectedIndex() override;

        /**
         * @brief 设置选中项索引
         */
        virtual void SetSelectedIndex(int index) override;

        /**
         * @brief 获取选中项
         */
        virtual StrList GetSelectedItem() override;

    public:
        /**
         * @brief 清空所有子项
         */
        virtual void Clear() override;

        /**
         * @brief       获取指定索引处子项的值
         * @param index 子项的索引
         */
        virtual StrList GetItemAt(int index) override;

        /**
         * @brief      添加新的子项
         * @param item 要添加的子项
         * @return     是否添加成功
         */
        virtual bool AddItem(const StrList &item) override;

        /**
         * @brief       添加子项到指定索引
         * @param index 要插入的位置
         * @param item  要添加的子项
         * @return      是否添加成功
         */
        virtual bool InsertItem(int index, const StrList &item) override;

        /**
         * @brief          更新指定位置的子项
         * @param index    要更新子项的位置
         * @param newValue 子项的新值
         * @return         操作是否成功
         */
        virtual bool UpdateItem(int index, const StrList &newValue) override;

        /**
         * @brief       移除指定索引处的子项
         * @param index 要移除子项的索引
         * @return      操作是否成功
         */
        virtual bool RemoveItemAt(int index) override;

        /**
         * @brief     获取指定位置处文本
         * @param row 所在行
         * @param col 所在列
         * @return    对应位置的文本，若获取失败则返回空字符串
         */
        std::wstring GetItemAt(int row, int col);

        /**
         * @brief          更新指定位置处文本
         * @param row      所在行
         * @param col      所在列
         * @param newValue 要设置的文本
         * @return         操作是否成功
         */
        bool UpdateItem(int row, int col, const std::wstring &newValue);

        /**
         * @brief        添加新的列
         * @param column 要添加的列信息
         * @return       操作是否成功
         */
        bool AddColumn(const ListViewColumn &column);

        /**
         * @brief        添加新的列
         * @param header 要添加列的标题
         * @return       操作是否成功
         */
        bool AddColumn(const std::wstring &header);

        /**
         * @brief        添加新的列到指定索引
         * @param index  要插入的位置
         * @param column 要添加的列信息
         * @return       操作是否成功
         */
        bool InsertColumn(int index, const ListViewColumn &column);

        /**
         * @brief        添加新的列到指定索引
         * @param index  要插入的位置
         * @param header 要添加列的标题
         * @return       操作是否成功
         */
        bool InsertColumn(int index, const std::wstring &header);

        /**
         * @brief       设置指定列的标题
         * @param index 列的索引
         * @return      操作是否成功
         */
        bool SetColumnHeader(int index, const std::wstring &header);

        /**
         * @brief       获取指定列的宽度
         * @param index 列的索引
         * @return      列的宽度，若列不存在则返回-1
         */
        double GetColumnWidth(int index);

        /**
         * @brief       设置指定列的宽度
         * @param index 列的索引
         * @param width 要设置的宽度
         * @return      操作是否成功
         */
        bool SetColumnWidth(int index, double width);

    private:
        /**
         * @brief 获取行数
         */
        int _GetRowCount();

        /**
         * @brief 获取列数
         */
        int _GetColCount();

        /**
         * @brief 获取ListView扩展样式
         */
        DWORD _GetExtendedListViewStyle();

        /**
         * @brief  设置ListView扩展样式
         * @return 先前的样式
         */
        DWORD _SetExtendedListViewStyle(DWORD style);
    };
};
