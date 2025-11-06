#pragma once

#include "ItemsControl.h"
#include "List.h"

namespace sw
{
    /**
     * @brief 列表框
     */
    class ListBox : public StrItemsControl
    {
    public:
        /**
         * @brief 当前列表框页面第一个子项的索引
         */
        const Property<int> TopIndex;

        /**
         * @brief 是否允许多选，更新该属性会导致已添加的子项被清空
         */
        const Property<bool> MultiSelect;

        /**
         * @brief 多选状态下可通过该属性获取选中项的个数
         */
        const ReadOnlyProperty<int> SelectedCount;

    public:
        /**
         * @brief 初始化列表框
         */
        ListBox();

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
        virtual std::wstring GetSelectedItem() override;

        /**
         * @brief               接收到WM_CONTEXTMENU后调用目标控件的该函数
         * @param isKeyboardMsg 消息是否由按下快捷键（Shift+F10、VK_APPS）产生
         * @param mousePosition 鼠标在屏幕中的位置
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnContextMenu(bool isKeyboardMsg, const Point &mousePosition) override;

        /**
         * @brief      当父窗口接收到控件的WM_COMMAND时调用该函数
         * @param code 通知代码
         */
        virtual void OnCommand(int code) override;

    public:
        /**
         * @brief 清空所有子项
         */
        virtual void Clear() override;

        /**
         * @brief       获取指定索引处子项的值
         * @param index 子项的索引
         */
        virtual std::wstring GetItemAt(int index) override;

        /**
         * @brief      添加新的子项
         * @param item 要添加的子项
         * @return     是否添加成功
         */
        virtual bool AddItem(const std::wstring &item) override;

        /**
         * @brief       添加子项到指定索引
         * @param index 要插入的位置
         * @param item  要添加的子项
         * @return      是否添加成功
         */
        virtual bool InsertItem(int index, const std::wstring &item) override;

        /**
         * @brief          更新指定位置的子项
         * @param index    要更新子项的位置
         * @param newValue 子项的新值
         * @return         操作是否成功
         */
        virtual bool UpdateItem(int index, const std::wstring &newValue) override;

        /**
         * @brief       移除指定索引处的子项
         * @param index 要移除子项的索引
         * @return      操作是否成功
         */
        virtual bool RemoveItemAt(int index) override;

        /**
         * @brief       获取指定点处子项的索引
         * @param point 相对于列表框用户区左上角点的位置
         */
        int GetItemIndexFromPoint(const Point &point);

        /**
         * @brief  多选状态下可通过该函数获取所有选中项的索引
         * @return 所有选中项的索引
         */
        List<int> GetSelectedIndices();

        /**
         * @brief  多选状态下可通过该函数获取所有选中项的内容
         * @return 所有选中项的内容
         */
        StrList GetSelectedItems();

        /**
         * @brief       获取指定索引处子项的选中状态
         * @param index 子项的索引
         * @return      若子项选中则返回true，否则返回false
         */
        bool GetItemSelectionState(int index);

        /**
         * @brief       多选状态下设置指定索引处子项的选中状态
         * @param index 子项的索引，输入-1可设置所有子项的选中状态
         * @param value 要设置的子项状态
         */
        void SetItemSelectionState(int index, bool value);
    };
}
