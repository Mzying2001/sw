#pragma once

#include "MenuItem.h"
#include <Windows.h>
#include <initializer_list>
#include <map>
#include <memory>
#include <tuple>
#include <vector>

namespace sw
{
    /**
     * @brief 菜单类型的基类
     */
    class MenuBase
    {
    private:
        /**
         * @brief 记录菜单项的依赖关系
         */
        struct _MenuItemDependencyInfo {
            HMENU hParent; // 所在菜单的句柄
            HMENU hSelf;   // 若本身含有子项，则此项为本身的菜单句柄，否则为NULL
            int index;     // 所在菜单中的索引
        };

    private:
        /**
         * @brief 菜单句柄，使用InitMenuBase函数设置该值
         */
        HMENU _hMenu = NULL;

        /**
         * @brief 菜单项集合
         */
        std::vector<std::shared_ptr<MenuItem>> _items;

        /**
         * @brief 记录所有包含子项的菜单的句柄
         */
        std::vector<std::tuple<std::shared_ptr<MenuItem>, HMENU>> _popupMenus;

        /**
         * @brief 记录菜单项的ID，可通过菜单项所在索引获取ID（调用IndexToID）
         */
        std::vector<std::shared_ptr<MenuItem>> _ids;

        /**
         * @brief 记录菜单项直接依赖关系的map
         */
        std::map<MenuItem *, _MenuItemDependencyInfo> _dependencyInfoMap;

    protected:
        /**
         * @brief 初始化菜单
         */
        MenuBase(HMENU hMenu);

        MenuBase(const MenuBase &)            = delete; // 删除拷贝构造函数
        MenuBase(MenuBase &&)                 = delete; // 删除移动构造函数
        MenuBase &operator=(const MenuBase &) = delete; // 删除拷贝赋值运算符
        MenuBase &operator=(MenuBase &&)      = delete; // 删除移动赋值运算符

    public:
        /**
         * @brief 释放资源
         */
        virtual ~MenuBase();

        /**
         * @brief 获取菜单句柄
         */
        HMENU GetHandle();

        /**
         * @brief 更新菜单，该操作会导致菜单项的Enabled、Checked等恢复到初始状态
         */
        void Update();

        /**
         * @brief 初始化菜单并添加菜单项
         */
        void SetItems(std::initializer_list<MenuItem> items);

        /**
         * @brief          重新设置当前菜单中某个菜单项的子项
         * @param item     要修改的菜单项，当该项原先不含有子项时将会调用Update更新整个菜单
         * @param subItems 新的子项列表
         * @return         返回一个bool值，表示操作是否成功
         */
        bool SetSubItems(MenuItem &item, std::initializer_list<MenuItem> subItems);

        /**
         * @brief      添加新的菜单项到菜单
         * @param item 新的菜单项
         */
        void AddItem(const MenuItem &item);

        /**
         * @brief         像当前菜单中的某个菜单项添加新的子项
         * @param item    要添加子项的菜单项，当该项原本不含有子项时将会调用Update更新整个菜单
         * @param subItem 要添加的子菜单项
         * @return        返回一个bool值，表示操作是否成功
         */
        bool AddSubItem(MenuItem &item, const MenuItem &subItem);

        /**
         * @brief      移除当前菜单中的某个子项
         * @param item 要移除的菜单项
         * @return     返回一个bool值，表示操作是否成功
         */
        bool RemoveItem(MenuItem &item);

        /**
         * @brief    通过id获取菜单项
         * @param id 要获取菜单项的id
         * @return   若函数成功则返回菜单项的指针，否则返回nullptr
         */
        MenuItem *GetMenuItem(int id);

        /**
         * @brief      通过索引来获取菜单项
         * @param path 要找项所在下索引
         * @return     若函数成功则返回菜单项的指针，否则返回nullptr
         */
        MenuItem *GetMenuItem(std::initializer_list<int> path);

        /**
         * @brief      通过菜单项的text来获取菜单项
         * @param path 每层要找的text
         * @return     若函数成功则返回菜单项的指针，否则返回nullptr
         */
        MenuItem *GetMenuItem(std::initializer_list<std::wstring> path);

        /**
         * @brief     通过tag值获取菜单项
         * @param tag 指定的tag
         * @return    若函数成功则返回菜单项的指针，否则返回nullptr
         */
        MenuItem *GetMenuItemByTag(uint64_t tag);

        /**
         * @brief      获取当前菜单中指定菜单项的直接父菜单项
         * @param item 要查询的子菜单项
         * @return     若函数成功则返回指向直接父菜单项的指针，否则返回nullptr
         */
        MenuItem *GetParent(MenuItem &item);

        /**
         * @brief      获取一个值，表示菜单项是否可用
         * @param item 要获取的菜单项
         * @param out  输出值
         * @return     函数是否成功
         */
        bool GetEnabled(MenuItem &item, bool &out);

        /**
         * @brief       设置菜单项是否可用
         * @param item  要修改的菜单项
         * @param value 设置的值
         * @return      修改是否成功
         */
        bool SetEnabled(MenuItem &item, bool value);

        /**
         * @brief      获取一个值，表示菜单项是否选中
         * @param item 要获取的菜单项
         * @param out  输出值
         * @return     函数是否成功
         */
        bool GetChecked(MenuItem &item, bool &out);

        /**
         * @brief       设置菜单项是否选中
         * @param item  要修改的菜单项
         * @param value 设置的值
         * @return      修改是否成功
         */
        bool SetChecked(MenuItem &item, bool value);

        /**
         * @brief       设置菜单项文本
         * @param item  要修改的菜单项
         * @param value 设置的值
         * @return      修改是否成功
         */
        bool SetText(MenuItem &item, const std::wstring &value);

        /**
         * @brief         设置菜单项要显示的位图
         * @param item    要修改的菜单项
         * @param hBitmap 要设置的位图句柄
         * @return        修改是否成功
         */
        bool SetBitmap(MenuItem &item, HBITMAP hBitmap);

        /**
         * @brief               设置菜单不同选中状态下显示的位图
         * @param item          要修改的菜单项
         * @param hBmpUnchecked 未选中时显示的位图
         * @param hBmpChecked   选中时显示的位图
         * @return              修改是否成功
         */
        bool SetCheckBitmap(MenuItem &item, HBITMAP hBmpUnchecked, HBITMAP hBmpChecked);

    private:
        /**
         * @brief 清除已添加的所有菜单项
         */
        void _ClearAddedItems();

        /**
         * @brief       添加菜单项到指定句柄
         * @param hMenu 要添加子项的菜单句柄
         * @param pItem 要添加的菜单项
         * @param index 菜单项在父菜单中的索引
         */
        void _AppendMenuItem(HMENU hMenu, std::shared_ptr<MenuItem> pItem, int index);

        /**
         * @brief      获取菜单项的依赖信息
         * @param item 要获取信息的菜单项
         * @return     若函数成功则返回指向_MenuItemDependencyInfo的指针，否则返回nullptr
         */
        _MenuItemDependencyInfo *_GetMenuItemDependencyInfo(MenuItem &item);

        /**
         * @brief       通过tag值获取菜单项
         * @param items 查找的vector
         * @param tag   指定的tag
         * @return      若函数成功则返回菜单项的指针，否则返回nullptr
         */
        MenuItem *_GetMenuItemByTag(std::vector<std::shared_ptr<MenuItem>> &items, uint64_t tag);

    protected:
        /**
         * @brief       根据索引获取ID
         * @param index 索引
         * @return      菜单项的ID
         */
        virtual int IndexToID(int index) = 0;

        /**
         * @brief    根据ID获取索引
         * @param id 菜单项的ID
         * @return   索引
         */
        virtual int IDToIndex(int id) = 0;
    };
}
