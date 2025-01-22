#pragma once

#include "ItemsControl.h"

namespace sw
{
    /**
     * @brief 组合框
     */
    class ComboBox : public StrItemsControl
    {
    private:
        /**
         * @brief 在读取Text属性时用于判断是否需要更新储存的文本
         */
        bool _isTextChanged = false;

    public:
        /**
         * @brief 组合框内容是否可编辑，更新该属性会导致已添加的子项被清空
         */
        const Property<bool> IsEditable;

    public:
        /**
         * @brief 初始化组合框
         */
        ComboBox();

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
         * @brief  获取可编辑状态下的编辑框文本内容
         */
        virtual std::wstring &GetText() override;

        /**
         * @brief       设置Text属性时调用该函数
         * @param value 要设置的文本
         */
        virtual void SetText(const std::wstring &value) override;

        /**
         * @brief      当父窗口接收到控件的WM_COMMAND时调用该函数
         * @param code 通知代码
         */
        virtual void OnCommand(int code) override;

        /**
         * @brief 选中项改变时调用该函数
         */
        virtual void OnSelectionChanged() override;

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
         * @brief 显示下拉列表
         */
        void ShowDropDown();

        /**
         * @brief 关闭下拉列表
         */
        void CloseDropDown();
    };
}
