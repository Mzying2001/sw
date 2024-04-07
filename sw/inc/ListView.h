#pragma once

#include "ImageList.h"
#include "ItemsControl.h"
#include "List.h"
#include <CommCtrl.h>

namespace sw
{
    /**
     * @brief 列表视图的图像列表枚举
     */
    enum class ListViewImageList {
        Normal      = LVSIL_NORMAL,      // 包含大图标的图像列表
        Small       = LVSIL_SMALL,       // 包含小图标的图像列表
        State       = LVSIL_STATE,       // 包含状态图像的图像列表
        GroupHeader = LVSIL_GROUPHEADER, // 组标头的图像列表
    };

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

        /**
         * @brief 多选状态下可通过该属性获取选中项的个数
         */
        const ReadOnlyProperty<int> SelectedCount;

        /**
         * @brief 是否在第一列显示复选框
         */
        const Property<bool> CheckBoxes;

        /**
         * @brief 当前列表框页面第一个子项的索引
         */
        const ReadOnlyProperty<int> TopIndex;

        /**
         * @brief 允许将同一图像列表与多个列表视图控件配合使用，控件销毁时若该属性为true则不会销毁图像列表
         */
        const Property<bool> ShareImageLists;

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

        /**
         * @brief        设置背景颜色
         * @param color  要设置的颜色
         * @param redraw 是否重绘
         */
        virtual void SetBackColor(Color color, bool redraw) override;

        /**
         * @brief        设置文本颜色
         * @param color  要设置的颜色
         * @param redraw 是否重绘
         */
        virtual void SetTextColor(Color color, bool redraw) override;

        /**
         * @brief        接收到WM_NOTIFY后调用该函数
         * @param pNMHDR 包含有关通知消息的信息
         * @return       若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnNotify(NMHDR *pNMHDR) override;

        /**
         * @brief 父窗口接收到WM_NOTIFY后调用发出通知控件的该函数
         */
        virtual void OnNotified(NMHDR *pNMHDR) override;

        /**
         * @brief 列表项某些属性发生变化时调用该函数
         */
        virtual void OnItemChanged(NMLISTVIEW *pNMLV);

        /**
         * @brief       复选框选中状态发生改变
         * @param index 改变项的索引
         */
        virtual void OnCheckStateChanged(int index);

        /**
         * @brief 鼠标左键单击列标题时调用该函数
         */
        virtual void OnHeaderItemClicked(NMHEADERW *pNMH);

        /**
         * @brief 鼠标左键双击列标题时调用该函数
         */
        virtual void OnHeaderItemDoubleClicked(NMHEADERW *pNMH);

        /**
         * @brief 鼠标左键单击某一项时调用该函数
         */
        virtual void OnItemClicked(NMITEMACTIVATE *pNMIA);

        /**
         * @brief 鼠标左键双击某一项调用该函数
         */
        virtual void OnItemDoubleClicked(NMITEMACTIVATE *pNMIA);

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

        /**
         * @brief       移除指定列
         * @param index 列的索引
         * @return      操作是否成功
         */
        bool RemoveColumnAt(int index);

        /**
         * @brief 获取所有选中项的索引
         */
        List<int> GetSelectedIndices();

        /**
         * @brief 获取所有复选框选中的项的索引
         */
        List<int> GetCheckedIndices();

        /**
         * @brief 获取指定索引项的复选框是否选中
         */
        bool GetItemCheckState(int index);

        /**
         * @brief 设置指定索引项复选框的选中状态
         */
        void SetItemCheckState(int index, bool value);

        /**
         * @brief       获取指定点处子项的索引
         * @param point 相对于用户区左上角点的位置
         */
        int GetItemIndexFromPoint(const Point &point);

        /**
         * @brief           获取列表视图的图像列表
         * @param imageList 要获取的图像列表类型
         */
        ImageList GetImageList(ListViewImageList imageList);

        /**
         * @brief           设置列表视图的图像列表
         * @param imageList 要设置的图像列表类型
         * @param value     要设置的图像列表的句柄
         * @return          若函数成功则返回之前与控件关联的图像列表，否则返回NULL
         */
        HIMAGELIST SetImageList(ListViewImageList imageList, HIMAGELIST value);

        /**
         * @brief          设置指定子项的图像
         * @param index    子项的索引
         * @param imgIndex 图像在图像列表中的索引
         * @return         操作是否成功
         */
        bool SetItemImage(int index, int imgIndex);

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
