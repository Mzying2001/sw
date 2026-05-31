#pragma once

#include "ItemsControl.h"
#include "ObservableCollection.h"

namespace sw
{
    /**
     * @brief 列表框
     */
    class ListBox : public ItemsControl
    {
    private:
        /**
         * @brief 列表框的默认数据源
         */
        ObservableCollection<std::wstring> _items;

        /**
         * @brief 选中项的背景颜色
         */
        Color _selectedBackColor{GetSysColor(COLOR_HIGHLIGHT)};

        /**
         * @brief 选中项的文本颜色
         */
        Color _selectedTextColor{GetSysColor(COLOR_HIGHLIGHTTEXT)};

    public:
        /**
         * @brief 列表框的子项集合，当未设置ItemsSource时使用该集合作为数据源
         */
        const ReadOnlyProperty<ObservableCollection<std::wstring> *> Items;

        /**
         * @brief 列表框子项的高度
         */
        const Property<double> ItemHeight;

        /**
         * @brief 当前列表框页面第一个子项的索引
         */
        const Property<int> TopIndex;

        /**
         * @brief 选中项的背景颜色
         */
        const Property<Color> SelectedBackColor;

        /**
         * @brief 选中项的文本颜色
         */
        const Property<Color> SelectedTextColor;

    public:
        /**
         * @brief 初始化列表框
         */
        ListBox();

        /**
         * @brief 刷新控件以反映数据源的当前状态
         */
        void Refresh();

        /**
         * @brief 获取指定点处子项的索引
         * @param point 相对于列表框用户区左上角点的位置
         */
        int GetItemIndexFromPoint(const Point &point);

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
         * @brief 接收到WM_CONTEXTMENU后调用目标控件的该函数
         * @param isKeyboardMsg 消息是否由按下快捷键（Shift+F10、VK_APPS）产生
         * @param mousePosition 鼠标在屏幕中的位置
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnContextMenu(bool isKeyboardMsg, const Point &mousePosition) override;

        /**
         * @brief 父窗口接收到WM_DRAWITEM后且父窗口OnDrawItem函数返回false时调用发出通知控件的该函数
         * @param pDrawItem 包含有关要绘制的项和所需绘图类型的信息的结构体指针
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnDrawItemSelf(DRAWITEMSTRUCT *pDrawItem) override;

    private:
        /**
         * @brief 设置子项数量
         * @param count 指定列表框中的新项计数
         */
        void _SetCount(int count);

        /**
         * @brief 更新子项数量
         */
        void _UpdateCount();
    };
}
