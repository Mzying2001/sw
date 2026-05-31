#pragma once

#include "ItemsControl.h"
#include "ObservableCollection.h"

namespace sw
{
    /**
     * @brief 组合框
     */
    class ComboBox : public ItemsControl
    {
    private:
        /**
         * @brief 组合框的默认数据源
         */
        ObservableCollection<std::wstring> _items;

    public:
        /**
         * @brief 列表框的子项集合，当未设置ItemsSource时使用该集合作为数据源
         */
        const ReadOnlyProperty<ObservableCollection<std::wstring> *> Items;

        /**
         * @brief 组合框内容是否可编辑
         */
        const Property<bool> IsEditable;

    public:
        /**
         * @brief 初始化组合框
         */
        ComboBox();

        /**
         * @brief 刷新控件以反映数据源的当前状态
         */
        void Refresh();

        /**
         * @brief 显示下拉列表
         */
        void ShowDropDown();

        /**
         * @brief 关闭下拉列表
         */
        void CloseDropDown();

    protected:
        /**
         * @brief 获取默认数据源，当ItemsSource未设置时使用该数据源
         * @return 默认数据源的IList指针，若无默认数据源则返回nullptr
         * @note 子类应确保返回的IList在ItemsControl生命周期内始终有效，且保证多次调用返回同一指针
         */
        virtual IList *GetDefaultItemsSource() override final;

        /**
         * @brief 当前数据源改变时调用该函数
         * @param oldItemsSource 旧的数据源
         * @param newItemsSource 新的数据源
         */
        virtual void OnCurrentItemsSourceChanged(IList *oldItemsSource, IList *newItemsSource) override;

        /**
         * @brief 当数据源集合发生变更时调用该函数
         * @param args 包含集合变更信息的事件参数
         */
        virtual void OnCurrentItemsSourceCollectionChanged(const NotifyCollectionChangedEventArgs &args) override;

        /**
         * @brief 选中项的索引，当无选中项时为-1
         */
        virtual int GetSelectedIndex() override;

        /**
         * @brief 设置选中项索引
         */
        virtual void SetSelectedIndex(int index) override;

        /**
         * @brief 当父窗口接收到控件的WM_COMMAND时调用该函数
         * @param code 通知代码
         */
        virtual void OnCommand(int code) override;

        /**
         * @brief 选中项改变时调用该函数
         */
        virtual void OnSelectionChanged() override;

        /**
         * @brief 获取子项要显示的文本
         * @param index 子项索引
         * @param item 包含子项数据的Variant对象
         */
        virtual std::wstring GetDisplayText(int index, const Variant &item);

    private:
        /**
         * @brief 根据当前选中项更新组合框的文本内容
         */
        void _UpdateSelectedText();

        /**
         * @brief 根据数据源更新组合框的项内容
         */
        void _UpdateItems();

        /**
         * @brief 发送CB_ADDSTRING消息添加一个字符串项
         * @param str 要添加的字符串
         */
        void _AddString(const std::wstring &str);

        /**
         * @brief 发送CB_INSERTSTRING消息在指定索引处插入一个字符串项
         * @param index 要插入的项的索引
         * @param str 要插入的字符串
         */
        void _InsertString(int index, const std::wstring &str);

        /**
         * @brief 发送CB_DELETESTRING消息删除指定索引处的项
         * @param index 要删除的项的索引
         */
        void _DeleteString(int index);
    };
}
