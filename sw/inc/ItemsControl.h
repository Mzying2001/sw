#pragma once

#include "Control.h"
#include "IList.h"
#include "INotifyCollectionChanged.h"

namespace sw
{
    /**
     * @brief 表示可用于呈现一组项的控件
     */
    class ItemsControl : public Control
    {
    private:
        /**
         * @brief 用户设置的数据源
         */
        IList *_itemsSource = nullptr;

        /**
         * @brief 当前数据源相关的状态
         */
        struct {
            /// @brief 当前数据源的IList指针
            IList *itemsSource = nullptr;

            /// @brief 若当前数据源实现了INotifyCollectionChanged接口，
            ///        则该指针指向该接口以便订阅事件；否则为nullptr
            INotifyCollectionChanged *notifyCollectionChanged = nullptr;
        } _current;

    public:
        /**
         * @brief 数据源
         */
        const Property<IList *> ItemsSource;

        /**
         * @brief 子项数量
         */
        const ReadOnlyProperty<int> ItemsCount;

        /**
         * @brief 选中项的索引，当无选中项时为-1
         */
        const Property<int> SelectedIndex;

        /**
         * @brief 当前选中项的引用，当无选中项时返回空Variant对象
         */
        const ReadOnlyProperty<Variant> SelectedItem;

    public:
        /**
         * @brief 构造函数
         */
        ItemsControl();

    protected:
        /**
         * @brief 获取当前正在使用的数据源
         */
        IList *GetCurrentItemsSource();

        /**
         * @brief 更新当前数据源状态并订阅新数据源的CollectionChanged事件（如果支持）
         */
        void UpdateCurrentItemsSource();

        /**
         * @brief 选中项改变时调用该函数
         */
        virtual void OnSelectionChanged();

    private:
        /**
         * @brief 处理数据源集合变更事件的函数
         * @param sender 事件发送者，即数据源对象
         * @param args 集合变更事件参数
         */
        void _CollectionChangedEventHandler(
            INotifyCollectionChanged &sender, NotifyCollectionChangedEventArgs &args);

    protected:
        /**
         * @brief 获取默认数据源，当ItemsSource未设置时使用该数据源
         * @return 默认数据源的IList指针，若无默认数据源则返回nullptr
         * @note 子类应确保返回的IList在ItemsControl生命周期内始终有效，且保证多次调用返回同一指针
         */
        virtual IList *GetDefaultItemsSource() = 0;

        /**
         * @brief 当前数据源改变时调用该函数
         * @param oldItemsSource 旧的数据源
         * @param newItemsSource 新的数据源
         */
        virtual void OnCurrentItemsSourceChanged(IList *oldItemsSource, IList *newItemsSource) = 0;

        /**
         * @brief 当数据源集合发生变更时调用该函数
         * @param args 包含集合变更信息的事件参数
         */
        virtual void OnCurrentItemsSourceCollectionChanged(const NotifyCollectionChangedEventArgs &args) = 0;

        /**
         * @brief 选中项的索引，当无选中项时为-1
         */
        virtual int GetSelectedIndex() = 0;

        /**
         * @brief 设置选中项索引
         */
        virtual void SetSelectedIndex(int index) = 0;
    };
}
