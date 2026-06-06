#pragma once

#include "FrameworkElement.h"
#include "List.h"
#include <cstdint>
#include <initializer_list>
#include <memory>
#include <string>
#include <windows.h>

namespace sw
{
    /**
     * @brief 菜单项描述结构体
     */
    struct MenuItemDesc //
    {
        /// @brief 菜单项标记，可用于区分不同菜单项
        uint64_t tag = 0;

        /// @brief 菜单项文本
        std::wstring text{};

        /// @brief 菜单项是否可用，默认为true
        bool enabled = true;

        /// @brief 菜单项是否选中，默认为false
        bool checked = false;

        /// @brief 菜单项位图句柄，默认为NULL
        HBITMAP hBitmap = NULL;

        /// @brief 子菜单项列表
        List<MenuItemDesc> subItems{};

        /// @brief 默认构造函数
        MenuItemDesc() = default;

        /// @brief 构造指定文本的菜单项描述
        /// @param text 菜单项文本
        MenuItemDesc(const std::wstring &text);

        /// @brief 构造指定文本和子项的菜单项描述
        /// @param text 菜单项文本
        /// @param subItems 子菜单项列表
        MenuItemDesc(const std::wstring &text,
                     std::initializer_list<MenuItemDesc> subItems);

        /// @brief 构造指定标记和文本的菜单项描述
        /// @param tag 菜单项标记
        /// @param text 菜单项文本
        MenuItemDesc(uint64_t tag, const std::wstring &text);

        /// @brief 构造指定标记、文本和子项的菜单项描述
        /// @param tag 菜单项标记
        /// @param text 菜单项文本
        /// @param subItems 子菜单项列表
        MenuItemDesc(uint64_t tag, const std::wstring &text,
                     std::initializer_list<MenuItemDesc> subItems);
    };

    /**
     * @brief 菜单项类
     */
    class MenuItem : public FrameworkElement
    {
    private:
        /**
         * @brief 基类别名，方便调用基类函数
         */
        using TBase = FrameworkElement;

        /**
         * @brief 菜单项描述信息
         */
        MenuItemDesc _desc{};

        /**
         * @brief 菜单项句柄
         */
        HMENU _hMenu = NULL;

        /**
         * @brief 菜单项ID
         */
        int _id = -1;

        /**
         * @brief 是否为根菜单项
         */
        bool _isRoot = false;

        /**
         * @brief 父菜单项指针
         */
        MenuItem *_parent = nullptr;

        /**
         * @brief 子菜单项列表
         */
        std::vector<std::unique_ptr<MenuItem>> subItems{};

    public:
        /**
         * @brief 菜单项Id
         */
        const ReadOnlyProperty<int> Id;

        /**
         * @brief 菜单项句柄
         */
        const ReadOnlyProperty<HMENU> Handle;

        /**
         * @brief 菜单项文本
         */
        const Property<std::wstring> Text;

        /**
         * @brief 菜单项是否可用
         */
        const Property<bool> Enabled;

        /**
         * @brief 菜单项是否选中
         */
        const Property<bool> IsChecked;

        /**
         * @brief 菜单项位图
         */
        const Property<HBITMAP> Bitmap;

        /**
         * @brief 菜单项是否为分隔符
         */
        const ReadOnlyProperty<bool> IsSeparator;

        /**
         * @brief 菜单项Tag
         */
        const Property<uint64_t> Tag;

    private:
        /**
         * @brief 构造函数，使用菜单项描述信息初始化菜单项
         * @param desc 菜单项描述信息
         */
        MenuItem(const MenuItemDesc &desc);

    public:
        /**
         * @brief 析构函数
         */
        virtual ~MenuItem();

        /**
         * @brief 创建一个新的菜单项实例
         * @param desc 菜单项描述信息
         */
        static MenuItem *Create(const MenuItemDesc &desc);

        /**
         * @brief 创建一个新的根菜单项实例
         * @return 新创建的根菜单项实例
         */
        static MenuItem *CreateRoot(bool isPopup);

    protected:
        /**
         * @brief 当Tag更改时调用此函数
         */
        virtual void OnTagChanged() override;

    public:
        /**
         * @brief 获取逻辑树中的父元素
         * @return 父元素指针，如果没有父元素则返回nullptr
         */
        virtual MenuItem *GetParent() const override final;

        /**
         * @brief 获取逻辑树中的子元素数量
         * @return 子元素数量
         */
        virtual int GetChildCount() const override final;

        /**
         * @brief 获取逻辑树中指定索引处的子元素
         * @param index 子元素索引
         * @throw std::out_of_range 如果索引超出范围
         */
        virtual MenuItem &GetChildAt(int index) const override final;

        /**
         * @brief 向当前菜单项添加一个子菜单项
         * @param desc 子菜单项的描述信息
         * @return 新增的子菜单项实例
         */
        MenuItem *AddChild(const MenuItemDesc &desc);

        /**
         * @brief 向当前菜单项的指定索引处插入一个子菜单项
         * @param index 插入位置的子菜单项索引
         * @param desc 子菜单项的描述信息
         * @return 新增的子菜单项实例
         * @throw std::out_of_range 如果索引超出范围
         */
        MenuItem *InsertChild(int index, const MenuItemDesc &desc);

        /**
         * @brief 移除指定索引处的子菜单项
         * @param index 子菜单项索引
         * @return 若函数成功则返回true，否则返回false
         */
        bool RemoveChildAt(int index);

        /**
         * @brief 移除指定子菜单项
         * @param child 子菜单项指针
         * @return 若函数成功则返回true，否则返回false
         */
        bool RemoveChild(MenuItem *child);

        /**
         * @brief 移除所有子菜单项
         */
        void ClearChildren();

        /**
         * @brief 获取指定子菜单项在当前菜单项中的索引
         * @param child 子菜单项指针
         * @return 子菜单项索引，如果未找到则返回-1
         */
        int IndexOf(MenuItem *child) const;

        /**
         * @brief 重置子菜单项列表
         * @param descs 新的子菜单项描述信息列表
         */
        void ResetChildren(std::initializer_list<MenuItemDesc> descs);

        /**
         * @brief 查找对应ID的子菜单项
         * @param id 子菜单项ID
         * @return 指向子菜单项的指针，如果未找到则返回nullptr
         */
        MenuItem *FindChildById(int id);

        /**
         * @brief 查找对应tag的子菜单项
         * @param tag 子菜单项tag
         * @return 指向子菜单项的指针，如果未找到则返回nullptr
         */
        MenuItem *FindChildByTag(uint64_t tag);

    private:
        /**
         * @brief 重建菜单项
         */
        void _ResetMenuItem();

        /**
         * @brief 更新菜单项状态
         */
        void _UpdateState();

        /**
         * @brief 设置父菜单项指针
         * @param parent 父菜单项指针
         * @param child 子菜单项指针
         */
        static void _SetParent(MenuItem *parent, MenuItem *child);

        /**
         * @brief 生成一个新的菜单项ID
         * @return 新的菜单项ID
         */
        static int _GenerateMenuItemID();

        /**
         * @brief 根据菜单项描述信息填充MENUITEMINFOW结构体
         * @param desc 菜单项描述信息
         * @param pMii 待填充的MENUITEMINFOW结构体指针
         */
        static void _ApplyMenuDesc(const MenuItemDesc &desc, MENUITEMINFOW *pMii);

        /**
         * @brief 根据菜单项描述信息和菜单句柄应用菜单项属性
         * @param hParentMenu 菜单项所在菜单的句柄
         * @param index 菜单项在父菜单中的索引
         * @param desc 菜单项描述信息
         */
        static void _UpdateMenuItem(HMENU hParentMenu, int index, const MenuItemDesc &desc);
    };
}
