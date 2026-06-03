#include "Control.h"
#include "ImageList.h"
#include <commctrl.h>

namespace sw
{
    /**
     * @brief 树视图的图像列表枚举
     */
    enum class TreeViewImageList {
        Normal = TVSIL_NORMAL, ///< 普通图像列表
        State  = TVSIL_STATE,  ///< 状态映像列表
    };

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
        explicit operator bool() const;

        /**
         * @brief 获取当前项的文本
         */
        std::wstring GetText() const;

        /**
         * @brief 设置当前项的文本
         * @return 操作是否成功
         */
        bool SetText(const std::wstring &text);

        /**
         * @brief 获取父节点
         * @return 父节点，若无父节点则返回空节点
         */
        TreeViewNode GetParent() const;

        /**
         * @brief 获取下一个节点
         * @return 下一个节点，若无下一个节点则返回空节点
         */
        TreeViewNode GetNextNode() const;

        /**
         * @brief 获取上一个节点
         * @return 上一个节点，若无上一个节点则返回空节点
         */
        TreeViewNode GetPreviousNode() const;

        /**
         * @brief 获取第一个子节点
         * @return 第一个子节点，若无子节点则返回空节点
         */
        TreeViewNode GetFirstChildNode() const;

        /**
         * @brief 在当前节点后插入新节点
         * @return 新插入的节点
         */
        TreeViewNode InsertAfter(const std::wstring &text);

        /**
         * @brief 添加子节点到当前节点下
         * @return 新插入的节点
         */
        TreeViewNode AddChild(const std::wstring &text);

        /**
         * @brief 判断当前节点是否被选中
         * @return 若节点被选中则返回true，否则返回false
         */
        bool IsSelected() const;

        /**
         * @brief 选中当前节点
         * @return 操作是否成功
         */
        bool Select();

        /**
         * @brief 删除当前节点
         * @return 操作是否成功
         */
        bool Delete();

        /**
         * @brief 判断当前节点是否展开
         * @return 若节点已展开则返回true，否则返回false
         */
        bool IsExpanded() const;

        /**
         * @brief 设置当前节点展开或折叠
         * @return 操作是否成功
         */
        bool SetExpand(bool expand);

        /**
         * @brief 展开当前节点
         * @return 操作是否成功
         */
        bool Expand();

        /**
         * @brief 折叠当前节点
         * @return 操作是否成功
         */
        bool Collapse();

        /**
         * @brief 获取与当前节点关联的用户数据
         */
        void *GetUserData() const;

        /**
         * @brief 设置与当前节点关联的用户数据
         * @return 操作是否成功
         */
        bool SetUserData(void *data);

        /**
         * @brief 判断当前节点是否被选中复选框
         * @return 若节点被选中则返回true，否则返回false
         */
        bool IsChecked() const;

        /**
         * @brief 设置当前节点的复选框选中状态
         */
        void SetCheck(bool check);

        /**
         * @brief 设置当前节点的图像
         * @return 操作是否成功
         */
        bool SetImages(int imageIndex, int selectedImageIndex);

        /**
         * @brief 获取当前节点的直接子节点数
         */
        int GetChildCount() const;

        /**
         * @brief 删除当前节点的所有子节点
         * @return 删除直接子节点的个数
         */
        int DeleteAllChildren();
    };

    /**
     * @brief 树视图节点正在展开或折叠事件参数类型
     */
    class TreeViewItemExpandingEventArgs : public TypedRoutedEventArgs<TreeView_ItemExpanding, CancelableEventArgs>
    {
    public:
        /// @brief true表示展开，false表示折叠
        bool action;

        /// @brief 正在展开或折叠的节点
        TreeViewNode node;

        /// @brief 构造函数
        /// @param action true表示展开，false表示折叠
        /// @param node 正在展开或折叠的节点
        TreeViewItemExpandingEventArgs(bool action, const TreeViewNode &node)
            : action(action), node(node)
        {
        }
    };

    /**
     * @brief 树视图节点已展开或折叠事件参数类型
     */
    class TreeViewItemExpandedEventArgs : public TypedRoutedEventArgs<TreeView_ItemExpanding>
    {
    public:
        /// @brief true表示展开，false表示折叠
        bool action;

        /// @brief 已展开或折叠的节点
        TreeViewNode node;

        /// @brief 构造函数
        /// @param action true表示展开，false表示折叠
        /// @param node 已展开或折叠的节点
        TreeViewItemExpandedEventArgs(bool action, const TreeViewNode &node)
            : action(action), node(node)
        {
        }
    };

    /**
     * @brief 树视图节点复选框状态改变事件参数类型
     */
    class TreeViewCheckStateChangedEventArgs : public TypedRoutedEventArgs<TreeView_CheckStateChanged>
    {
    public:
        /// @brief 复选框的新状态，0表示未选中，1表示选中，-1表示无复选框
        int checkState;

        /// @brief 复选框状态改变的节点
        TreeViewNode node;

        /// @brief 构造函数
        /// @param checkState 复选框的新状态，0表示未选中，1表示选中，-1表示无复选框
        /// @param node 复选框状态改变的节点
        TreeViewCheckStateChangedEventArgs(int checkState, const TreeViewNode &node)
            : checkState(checkState), node(node)
        {
        }
    };

    /**
     * @brief 树视图控件
     */
    class TreeView : public Control
    {
    private:
        /**
         * @brief 基类别名，方便调用基类函数实现
         */
        using TBase = Control;

    public:
        /**
         * @brief 根节点
         */
        const ReadOnlyProperty<TreeViewNode> Root;

        /**
         * @brief 选中的节点，若无选中节点则返回空节点
         */
        const ReadOnlyProperty<TreeViewNode> SelectedItem;

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

        /**
         * @brief 缩进宽度
         */
        const Property<double> IndentWidth;

    public:
        /**
         * @brief 初始化TreeView
         */
        TreeView();

    protected:
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
         * @brief 父窗口接收到WM_NOTIFY后且父窗口OnNotify函数返回false时调用发出通知控件的该函数
         * @param pNMHDR 包含有关通知消息的信息
         * @param result 函数返回值为true时将该值作为消息的返回值
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnNotified(NMHDR *pNMHDR, LRESULT &result) override;

        /**
         * @brief 选中的节点发生改变时调用该函数
         */
        virtual void OnSelectionChanged();

        /**
         * @brief 控件被单机时调用该函数
         * @param pNMHDR 包含有关通知消息的信息
         * @param result 函数返回值为true时将该值作为消息的返回值
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnClicked(NMHDR *pNMHDR, LRESULT &result);

        /**
         * @brief 控件被双击时调用该函数
         * @param pNMHDR 包含有关通知消息的信息
         * @param result 函数返回值为true时将该值作为消息的返回值
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnDoubleClicked(NMHDR *pNMHDR, LRESULT &result);

        /**
         * @brief 当OnNotified接收到TVN_GETDISPINFO通知时调用该函数
         * @param pNMInfo 包含有关通知消息的信息
         */
        virtual void OnGetDispInfo(NMTVDISPINFOW *pNMInfo);

        /**
         * @brief 节点展开或折叠前调用该函数
         * @param pNMTV 包含有关通知消息的信息
         * @return 若返回true则取消展开或折叠操作，否则继续进行
         */
        virtual bool OnItemExpanding(NMTREEVIEWW *pNMTV);

        /**
         * @brief 节点展开或折叠后调用该函数
         * @param pNMTV 包含有关通知消息的信息
         */
        virtual void OnItemExpanded(NMTREEVIEWW *pNMTV);

        /**
         * @brief 节点某些属性发生变化时调用该函数
         * @param pNMInfo 包含有关通知消息的信息
         */
        virtual void OnItemChanged(NMTVITEMCHANGE *pNMInfo);

    public:
        /**
         * @brief 清空所有节点
         */
        void Clear();

        /**
         * @brief 添加新节点到根节点
         * @param text 节点文本
         * @return 新插入的节点
         */
        TreeViewNode AddItem(const std::wstring &text);

        /**
         * @brief 获取指定类型的图像列表
         * @param imageList 图像列表类型
         */
        ImageList GetImageList(TreeViewImageList imageList);

        /**
         * @brief 设置指定类型的图像列表
         * @param imageList 图像列表类型
         * @param value 要设置的图像列表
         */
        HIMAGELIST SetImageList(TreeViewImageList imageList, HIMAGELIST value);

    private:
        /**
         * @brief 获取根节点
         */
        TreeViewNode _GetRoot();

        /**
         * @brief 获取选中的节点，若无选中节点则返回空节点
         */
        TreeViewNode _GetSelectedItem();

        /**
         * @brief 插入新节点
         */
        TreeViewNode _InsertItem(HTREEITEM hParent, HTREEITEM hInsertAfter, const std::wstring &text);
    };
}
