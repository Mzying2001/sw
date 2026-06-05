#pragma once

#include "ImageList.h"
#include "ItemsControl.h"
#include "List.h"
#include "ObservableCollection.h"
#include <commctrl.h>

namespace sw
{
    /**
     * @brief 列表视图的图像列表枚举
     */
    enum class ListViewImageList {
        Normal      = LVSIL_NORMAL,      ///< 包含大图标的图像列表
        Small       = LVSIL_SMALL,       ///< 包含小图标的图像列表
        State       = LVSIL_STATE,       ///< 包含状态图像的图像列表
        GroupHeader = LVSIL_GROUPHEADER, ///< 组标头的图像列表
    };

    /**
     * @brief 列表视图的列对齐方式
     */
    enum class ListViewColumnAlignment {
        Left   = LVCFMT_LEFT,   ///< 左对齐
        Right  = LVCFMT_RIGHT,  ///< 右对齐
        Center = LVCFMT_CENTER, ///< 居中
    };

    /**
     * @brief 列表视图的列信息
     */
    struct ListViewColumn {
        /// @brief 列标题
        std::wstring header;

        /// @brief 列宽度
        double width;

        /// @brief 列对齐方式
        ListViewColumnAlignment alignment;

        /// @brief 构造函数，默认宽度100，左对齐
        ListViewColumn(const wchar_t *header, double width = 100,
                       ListViewColumnAlignment alignment = ListViewColumnAlignment::Left);

        /// @brief 构造函数，默认宽度100，左对齐
        ListViewColumn(const std::wstring &header, double width = 100,
                       ListViewColumnAlignment alignment = ListViewColumnAlignment::Left);
    };

    /**
     * @brief 列表视图的子项信息
     */
    struct ListViewItem {
        /// @brief 子项文本列表，索引0为主项文本
        List<std::wstring> subItems;

        /// @brief 关联图像在图像列表中的索引，-1表示无图像
        int imageIndex = -1;

        /// @brief 复选框选中状态
        bool checked = false;

        /// @brief 默认构造函数
        ListViewItem() = default;

        /// @brief 默认拷贝构造函数
        ListViewItem(const ListViewItem &other) = default;

        /// @brief 移动构造函数
        ListViewItem(ListViewItem &&other) noexcept;

        /// @brief 使用初始化列表构造
        ListViewItem(std::initializer_list<std::wstring> subItems);
    };

    /**
     * @brief 列表视图
     */
    class ListView : public ItemsControl
    {
    private:
        /**
         * @brief 列表项集合
         */
        ObservableCollection<ListViewItem> _items;

        /**
         * @brief 列集合
         */
        ObservableCollection<ListViewColumn> _columns;

        /**
         * @brief 用于给GetDisplayInfo提供显示信息的缓冲对象
         */
        ListViewItem _itemDisplayBuffer;

    public:
        /**
         * @brief 列表项集合，当未设置ItemsSource时使用该集合作为数据源
         */
        const ReadOnlyProperty<ObservableCollection<ListViewItem> *> Items;

        /**
         * @brief 列集合
         */
        const ReadOnlyProperty<ObservableCollection<ListViewColumn> *> Columns;

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

        /**
         * @brief 是否允许编辑
         */
        const Property<bool> Editable;

    public:
        /**
         * @brief 初始化ListView
         */
        ListView();

        /**
         * @brief 刷新控件以反映数据源的当前状态
         * @param refreshColumns 是否刷新列信息
         */
        void Refresh(bool refreshColumns = true);

        /**
         * @brief 获取所有选中项的索引
         */
        List<int> GetSelectedIndices();

        /**
         * @brief 获取指定索引项的复选框是否选中
         */
        bool GetItemCheckState(int index);

        /**
         * @brief 设置指定索引项复选框的选中状态
         */
        void SetItemCheckState(int index, bool value);

        /**
         * @brief 获取指定点处子项的索引
         * @param point 相对于用户区左上角点的位置
         */
        int GetItemIndexFromPoint(const Point &point);

        /**
         * @brief 获取列表视图的图像列表
         * @param imageList 要获取的图像列表类型
         */
        ImageList GetImageList(ListViewImageList imageList);

        /**
         * @brief 设置列表视图的图像列表
         * @param imageList 要设置的图像列表类型
         * @param value 要设置的图像列表的句柄
         * @return 若函数成功则返回之前与控件关联的图像列表，否则返回NULL
         */
        HIMAGELIST SetImageList(ListViewImageList imageList, HIMAGELIST value);

        /**
         * @brief 进入编辑模式，调用该函数前需要将Editable属性设为true
         * @param index 编辑项的索引
         * @return 操作是否成功
         */
        bool EditItem(int index);

        /**
         * @brief 取消编辑
         */
        void CancelEdit();

        /**
         * @brief 确保指定索引的项可见
         * @param index 要确保可见的项的索引
         * @param partialOK 是否允许部分可见，默认为false表示必须完全可见
         * @return 操作是否成功，若index无效或无法确保可见则返回false
         */
        bool EnsureVisible(int index, bool partialOK = false);

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
         * @brief 设置背景颜色
         * @param color 要设置的颜色
         * @param redraw 是否重绘
         */
        virtual void SetBackColor(Color color, bool redraw) override;

        /**
         * @brief 设置文本颜色
         * @param color 要设置的颜色
         * @param redraw 是否重绘
         */
        virtual void SetTextColor(Color color, bool redraw) override;

        /**
         * @brief 接收到WM_NOTIFY后调用该函数
         * @param pNMHDR 包含有关通知消息的信息
         * @param result 函数返回值为true时将该值作为消息的返回值，默认值为0
         * @return 若已处理该消息则返回true，否则调用发出通知控件的OnNotified函数，依据其返回值判断是否调用DefaultWndProc
         */
        virtual bool OnNotify(NMHDR *pNMHDR, LRESULT &result) override;

        /**
         * @brief 父窗口接收到WM_NOTIFY后且父窗口OnNotify函数返回false时调用发出通知控件的该函数
         * @param pNMHDR 包含有关通知消息的信息
         * @param result 函数返回值为true时将该值作为消息的返回值
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnNotified(NMHDR *pNMHDR, LRESULT &result) override;

        /**
         * @brief 绘制控件前调用该函数
         * @param pNMCD 包含有关自定义绘制的信息
         * @param result 函数返回值为true时将该值作为NM_CUSTOMDRAW消息的返回值
         * @return 若已完成绘制则返回true，否则返回false以使用默认绘制
         */
        virtual bool OnPrePaint(NMCUSTOMDRAW *pNMCD, LRESULT &result) override;

        /**
         * @brief 绘制子项前调用该函数
         * @param pNMCD 包含有关自定义绘制的信息
         * @param subItem 若消息包含CDDS_SUBITEM标志则该值为true，否则为false
         * @param result 函数返回值为true时将该值作为NM_CUSTOMDRAW消息的返回值
         * @return 若已完成绘制则返回true，否则返回false以使用默认绘制
         */
        virtual bool OnItemPrePaint(NMCUSTOMDRAW *pNMCD, bool subItem, LRESULT &result) override;

        /**
         * @brief 列表项某些属性发生变化时调用该函数
         * @param pNMLV 包含有关列表项变化的信息
         */
        virtual void OnItemChanged(NMLISTVIEW *pNMLV);

        /**
         * @brief 复选框选中状态发生改变
         * @param index 改变项的索引
         */
        virtual void OnCheckStateChanged(int index);

        /**
         * @brief 接收到内部Header控件发送的HDN_ITEMCHANGINGW通知时调用该函数
         * @param pNMH 包含有关列标题变化的信息
         */
        virtual void OnHeaderItemChanging(NMHEADERW *pNMH);

        /**
         * @brief 接收到内部Header控件发送的HDN_ITEMCHANGEDW通知时调用该函数
         * @param pNMH 包含有关列标题变化的信息
         */
        virtual void OnHeaderItemChanged(NMHEADERW *pNMH);

        /**
         * @brief 鼠标左键单击列标题时调用该函数
         * @note 内部Header控件接收到HDN_ITEMCLICKW通知后会调用该函数
         */
        virtual void OnHeaderItemClicked(NMHEADERW *pNMH);

        /**
         * @brief 鼠标左键双击列标题时调用该函数
         * @note 内部Header控件接收到HDN_ITEMDBLCLICKW通知后会调用该函数
         */
        virtual void OnHeaderItemDoubleClicked(NMHEADERW *pNMH);

        /**
         * @brief 鼠标左键单击某一项时调用该函数
         * @note 控件接收到NM_CLICK通知后会调用该函数
         */
        virtual void OnItemClicked(NMITEMACTIVATE *pNMIA);

        /**
         * @brief 鼠标左键双击某一项调用该函数
         * @note 控件接收到NM_DBLCLK通知后会调用该函数
         */
        virtual void OnItemDoubleClicked(NMITEMACTIVATE *pNMIA);

        /**
         * @brief 复选框被点击时调用该函数
         * @param index 被点击项的索引
         */
        virtual void OnCheckBoxClicked(int index);

        /**
         * @brief 编辑状态结束后调用该函数
         * @param args 包含编辑结果的事件参数
         */
        virtual void OnEndEdit(ListViewEndEditEventArgs &args);

        /**
         * @brief 应用编辑结果时调用该函数
         * @param index 编辑项的索引
         * @param newText 编辑后的文本
         */
        virtual void OnApplyEdit(int index, const std::wstring &newText);

        /**
         * @brief 当OnNotified接收到LVN_GETDISPINFOW通知时调用该函数
         * @param pNMInfo 包含有关通知消息的信息
         */
        virtual void OnGetDispInfo(NMLVDISPINFOW *pNMInfo);

        /**
         * @brief 获取指定子项要显示的信息
         * @param index 子项索引
         * @param item 包含子项数据的Variant对象
         * @param pNMInfo 包含有关通知消息的信息，修改该结构体以提供要显示的信息
         */
        virtual void GetDisplayInfo(int index, const Variant &item, NMLVDISPINFOW *pNMInfo);

        /**
         * @brief 获取指定子项要显示的信息，子类可以重写该函数以显示自定义类型的数据
         * @param index 子项索引
         * @param item 包含子项数据的Variant对象
         * @param listViewItem 用于接收要显示信息的ListViewItem结构体，修改该结构体以提供要显示的信息
         * @return 若已提供显示信息则返回true，否则返回false以使用默认显示信息
         */
        virtual bool GetDisplayInfo(int index, const Variant &item, ListViewItem &listViewItem);

        /**
         * @brief 获取指定索引项的复选框矩形
         * @param index 项索引
         * @param rect 用于接收复选框矩形的RECT结构体
         */
        virtual void GetCheckBoxRect(int index, RECT &rect);

        /**
         * @brief 获取指定索引项的复选框是否选中
         * @param index 项索引
         * @param checked 用于接收复选框选中状态的布尔变量
         */
        virtual void OnGetItemCheckState(int index, bool &checked);

        /**
         * @brief 设置指定索引项复选框的选中状态
         * @param index 项索引
         * @param checked 要设置的选中状态
         */
        virtual void OnSetItemCheckState(int index, bool checked);

    private:
        /**
         * @brief 设置子项数量
         * @param count 指定列表视图中的新项计数
         */
        void _SetCount(int count);

        /**
         * @brief 更新子项数量以匹配数据源
         */
        void _UpdateCount();

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
         * @brief 设置ListView扩展样式
         * @return 先前的样式
         */
        DWORD _SetExtendedListViewStyle(DWORD style);

        /**
         * @brief 同步ListViewItem数据到Windows消息结构体
         * @param item 包含要显示信息的ListViewItem结构体
         * @param pNMInfo 要同步到的NMLVDISPINFOW结构体指针
         */
        void _ApplyDispInfo(const ListViewItem &item, NMLVDISPINFOW *pNMInfo);

        /**
         * @brief 同步ListViewColumn数据到LVCOLUMNW结构体
         * @param column 包含要显示信息的ListViewColumn结构体
         * @param pLvc 要同步到的LVCOLUMNW结构体指针
         */
        void _ApplyColumnInfo(const ListViewColumn &column, LVCOLUMNW *pLvc);

        /**
         * @brief 插入列
         * @param index 列索引
         * @param column 列信息
         */
        bool _InsertColumn(int index, const ListViewColumn &column);

        /**
         * @brief 移除列
         * @param index 列索引
         */
        bool _DeleteColumn(int index);

        /**
         * @brief 设置列信息
         * @param index 列索引
         * @param column 列信息
         */
        bool _SetColumn(int index, const ListViewColumn &column);

        /**
         * @brief 更新列以匹配数据源
         */
        void _UpdateColumns();

        /**
         * @brief 列集合变更处理函数
         * @param sender 事件发送者，即数据源对象
         * @param args 集合变更事件参数
         */
        void _ColumnsCollectionChangedHandler(
            INotifyCollectionChanged &sender, NotifyCollectionChangedEventArgs &args);
    };
};
