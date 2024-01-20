#pragma once

#include "Control.h"

namespace sw
{
    template <typename TItem>
    class ItemsControl; // 向前声明

    /**
     * @brief 表示可用于呈现一组字符串的控件
     */
    typedef ItemsControl<std::wstring> StrItemsControl;

    /**
     * @brief 表示可用于呈现一组项的控件
     */
    template <typename TItem>
    class ItemsControl : public Control
    {
    public:
        /**
         * @brief 项数
         */
        const ReadOnlyProperty<int> ItemsCount;

        /**
         * @brief 选中项的索引，当无选中项时为-1
         */
        const Property<int> SelectedIndex;

        /**
         * @brief 选中项
         */
        const ReadOnlyProperty<TItem> SelectedItem;

    protected:
        /**
         * @brief 初始化ItemsControl
         */
        ItemsControl()
            : ItemsCount(
                  // get
                  [&]() -> const int & {
                      static int result;
                      result = this->GetItemsCount();
                      return result;
                  }),

              SelectedIndex(
                  // get
                  [&]() -> const int & {
                      static int result;
                      result = this->GetSelectedIndex();
                      return result;
                  },
                  // set
                  [&](const int &value) {
                      this->SetSelectedIndex(value);
                  }),

              SelectedItem(
                  // get
                  [&]() -> const TItem & {
                      static TItem result;
                      result = this->GetSelectedItem();
                      return result;
                  })
        {
        }

        /**
         * @brief 选中项改变时调用该函数
         */
        virtual void OnSelectionChanged()
        {
            this->RaiseRoutedEvent(ItemsControl_SelectionChanged);
        }

    protected:
        /**
         * @brief  获取子项数
         */
        virtual int GetItemsCount() = 0;

        /**
         * @brief 选中项的索引，当无选中项时为-1
         */
        virtual int GetSelectedIndex() = 0;

        /**
         * @brief 设置选中项索引
         */
        virtual void SetSelectedIndex(int index) = 0;

        /**
         * @brief 获取选中项
         */
        virtual TItem GetSelectedItem() = 0;

    public:
        /**
         * @brief 清空所有子项
         */
        virtual void Clear() = 0;

        /**
         * @brief       获取指定索引处子项的值
         * @param index 子项的索引
         */
        virtual TItem GetItemAt(int index) = 0;

        /**
         * @brief      添加新的子项
         * @param item 要添加的子项
         * @return     是否添加成功
         */
        virtual bool AddItem(const TItem &item) = 0;

        /**
         * @brief       添加子项到指定索引
         * @param index 要插入的位置
         * @param item  要添加的子项
         * @return      是否添加成功
         */
        virtual bool InsertItem(int index, const TItem &item) = 0;

        /**
         * @brief          更新指定位置的子项
         * @param index    要更新子项的位置
         * @param newValue 子项的新值
         * @return         操作是否成功
         */
        virtual bool UpdateItem(int index, const TItem &newValue) = 0;

        /**
         * @brief       移除指定索引处的子项
         * @param index 要移除子项的索引
         * @return      操作是否成功
         */
        virtual bool RemoveItemAt(int index) = 0;
    };
}
