#include "ItemsControl.h"
#include <CommCtrl.h>

namespace sw
{
    /**
     * @brief 树视图项
     */
    class TreeViewNode : public IToString<TreeViewNode>,
                         public IEqualityComparable<TreeViewNode>
    {
    private:
        /**
         * @brief 树视图控件的窗口句柄
         */
        HWND _hwnd;

        /**
         * @brief 树视图项的句柄
         */
        HTREEITEM _hitem;

    public:
        /**
         * @brief 默认构造函数
         */
        TreeViewNode() = default;

        /**
         * @brief 创建TreeViewItem
         */
        TreeViewNode(HWND hwnd, HTREEITEM hitem);

        /**
         * @brief 获取所属树视图控件的窗口句柄
         */
        HWND GetOwnerHandle() const;

        /**
         * @brief 获取当前项的句柄
         */
        HTREEITEM GetHandle() const;

        /**
         * @brief 获取当前项的文本
         */
        std::wstring ToString() const;

        /**
         * @brief 判断当前项与另一个项是否相等
         */
        bool Equals(const TreeViewNode &other) const;

        /**
         * @brief 判断当前项是否为空
         */
        bool IsNull() const;

        /**
         * @brief 判断当前项是否有效
         */
        operator bool() const;

        /**
         * @brief 获取当前项的文本
         */
        std::wstring GetText() const;

        /**
         * @brief  设置当前项的文本
         * @return 操作是否成功
         */
        bool SetText(const std::wstring &text) const;

        /**
         * @brief  获取父节点
         * @return 父节点，若无父节点则返回空节点
         */
        TreeViewNode GetParent() const;

        /**
         * @brief  获取下一个节点
         * @return 下一个节点，若无下一个节点则返回空节点
         */
        TreeViewNode GetNextNode() const;

        /**
         * @brief  获取上一个节点
         * @return 上一个节点，若无上一个节点则返回空节点
         */
        TreeViewNode GetPreviousNode() const;

        /**
         * @brief  获取第一个子节点
         * @return 第一个子节点，若无子节点则返回空节点
         */
        TreeViewNode GetFirstChildNode() const;

        /**
         * @brief  在当前节点后插入新节点
         * @return 新插入的节点
         */
        TreeViewNode InsertAfter(const std::wstring &text) const;

        /**
         * @brief  添加子节点到当前节点下
         * @return 新插入的节点
         */
        TreeViewNode AddChild(const std::wstring &text) const;

        /**
         * @brief  判断当前节点是否被选中
         * @return 若节点被选中则返回true，否则返回false
         */
        bool IsSelected() const;

        /**
         * @brief  选中当前节点
         * @return 操作是否成功
         */
        bool Select() const;

        /**
         * @brief  删除当前节点
         * @return 操作是否成功
         */
        bool Delete() const;

        /**
         * @brief  判断当前节点是否展开
         * @return 若节点已展开则返回true，否则返回false
         */
        bool IsExpanded() const;

        /**
         * @brief  设置当前节点展开或折叠
         * @return 操作是否成功
         */
        bool SetExpand(bool expand) const;

        /**
         * @brief  展开当前节点
         * @return 操作是否成功
         */
        bool Expand() const;

        /**
         * @brief  折叠当前节点
         * @return 操作是否成功
         */
        bool Collapse() const;

        /**
         * @brief 获取与当前节点关联的用户数据
         */
        void *GetUserData() const;

        /**
         * @brief  设置与当前节点关联的用户数据
         * @return 操作是否成功
         */
        bool SetUserData(void *data) const;

        /**
         * @brief  判断当前节点是否被选中复选框
         * @return 若节点被选中则返回true，否则返回false
         */
        bool IsChecked() const;

        /**
         * @brief 设置当前节点的复选框选中状态
         */
        void SetCheck(bool check) const;
    };

    // clang-format off

    /**
     * @brief 树视图节点正在展开或折叠事件参数类型
     */
    struct TreeViewItemExpandingEventArgs : TypedRoutedEventArgs<TreeView_ItemExpanding> {
        bool cancel = false; // 是否取消本次展开或折叠操作
        bool action;         // true表示展开，false表示折叠
        TreeViewNode node;   // 正在展开或折叠的节点
        TreeViewItemExpandingEventArgs(bool action, const TreeViewNode &node): action(action), node(node) {}
    };

    /**
     * @brief 树视图节点已展开或折叠事件参数类型
     */
    struct TreeViewItemExpandedEventArgs : TypedRoutedEventArgs<TreeView_ItemExpanding> {
        bool action;       // true表示展开，false表示折叠
        TreeViewNode node; // 正在展开或折叠的节点
        TreeViewItemExpandedEventArgs(bool action, const TreeViewNode &node): action(action), node(node) {}
    };

    /**
     * @brief 树视图节点复选框状态改变事件参数类型
     */
    struct TreeViewCheckStateChangedEventArgs : TypedRoutedEventArgs<TreeView_CheckStateChanged> {
        int checkState;    // 复选框的新状态，0表示未选中，1表示选中，-1表示无复选框
        TreeViewNode node; // 复选框状态改变的节点
        TreeViewCheckStateChangedEventArgs(int checkState, const TreeViewNode &node): checkState(checkState), node(node) {}
    };

    // clang-format on

    /**
     * @brief 树视图控件
     */
    class TreeView : public ItemsControl<TreeViewNode>
    {
    private:
        /**
         * @brief 基类别名，方便调用基类函数实现
         */
        using TBase = ItemsControl<TreeViewNode>;

    public:
        /**
         * @brief 根节点
         */
        const ReadOnlyProperty<TreeViewNode> Root;

        /**
         * @brief 所有节点数
         */
        const ReadOnlyProperty<int> AllItemsCount;

        /**
         * @brief 是否在第一列显示复选框
         */
        const Property<bool> CheckBoxes;

        /**
         * @brief 线条颜色
         */
        const Property<Color> LineColor;

    public:
        /**
         * @brief 初始化TreeView
         */
        TreeView();

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
        virtual TreeViewNode GetSelectedItem() override;

        /**
         * @brief     绘制虚线框时调用该函数
         * @param hdc 绘制设备句柄
         */
        virtual void OnDrawFocusRect(HDC hdc) override;

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
         * @brief        父窗口接收到WM_NOTIFY后且父窗口OnNotify函数返回false时调用发出通知控件的该函数
         * @param pNMHDR 包含有关通知消息的信息
         * @param result 函数返回值为true时将该值作为消息的返回值
         * @return       若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnNotified(NMHDR *pNMHDR, LRESULT &result) override;

        /**
         * @brief        控件被单机时调用该函数
         * @param pNMHDR 包含有关通知消息的信息
         * @param result 函数返回值为true时将该值作为消息的返回值
         * @return       若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnClicked(NMHDR *pNMHDR, LRESULT &result);

        /**
         * @brief        控件被双击时调用该函数
         * @param pNMHDR 包含有关通知消息的信息
         * @param result 函数返回值为true时将该值作为消息的返回值
         * @return       若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnDoubleClicked(NMHDR *pNMHDR, LRESULT &result);

        /**
         * @brief         当OnNotified接收到TVN_GETDISPINFO通知时调用该函数
         * @param pNMInfo 包含有关通知消息的信息
         */
        virtual void OnGetDispInfo(NMTVDISPINFOW *pNMInfo);

        /**
         * @brief       节点展开或折叠前调用该函数
         * @param pNMTV 包含有关通知消息的信息
         * @return      若返回true则取消展开或折叠操作，否则继续进行
         */
        virtual bool OnItemExpanding(NMTREEVIEWW *pNMTV);

        /**
         * @brief       节点展开或折叠后调用该函数
         * @param pNMTV 包含有关通知消息的信息
         */
        virtual void OnItemExpanded(NMTREEVIEWW *pNMTV);

        /**
         * @brief         节点某些属性发生变化时调用该函数
         * @param pNMInfo 包含有关通知消息的信息
         */
        virtual void OnItemChanged(NMTVITEMCHANGE *pNMInfo);

    public:
        /**
         * @brief 清空所有子项
         */
        virtual void Clear() override;

        /**
         * @brief       获取指定索引处子项的值
         * @param index 子项的索引
         */
        virtual TreeViewNode GetItemAt(int index) override;

        /**
         * @brief      添加新的子项
         * @param item 要添加的子项
         * @return     是否添加成功
         * @note       TreeView不支持该操作，该函数始终返回false
         */
        virtual bool AddItem(const TreeViewNode &item) override;

        /**
         * @brief       添加子项到指定索引
         * @param index 要插入的位置
         * @param item  要添加的子项
         * @return      是否添加成功
         * @note        TreeView不支持该操作，该函数始终返回false
         */
        virtual bool InsertItem(int index, const TreeViewNode &item) override;

        /**
         * @brief          更新指定位置的子项
         * @param index    要更新子项的位置
         * @param newValue 子项的新值
         * @return         操作是否成功
         * @note           TreeView不支持该操作，该函数始终返回false
         */
        virtual bool UpdateItem(int index, const TreeViewNode &newValue) override;

        /**
         * @brief       移除指定索引处的子项
         * @param index 要移除子项的索引
         * @return      操作是否成功
         */
        virtual bool RemoveItemAt(int index) override;

        /**
         * @brief      添加新节点到根节点
         * @param text 节点文本
         * @return     新插入的节点
         */
        TreeViewNode AddItem(const std::wstring &text);

        /**
         * @brief       在指定索引处插入新节点
         * @param index 节点索引
         * @param text  节点文本
         * @return      新插入的节点
         */
        TreeViewNode InsertItem(int index, const std::wstring &text);

    private:
        /**
         * @brief 获取根节点
         */
        TreeViewNode _GetRoot();

        /**
         * @brief 插入新节点
         */
        TreeViewNode _InsertItem(HTREEITEM hParent, HTREEITEM hInsertAfter, const std::wstring &text);
    };
}
