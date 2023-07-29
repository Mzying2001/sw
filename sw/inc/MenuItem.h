#pragma once

#include <Windows.h>
#include <string>
#include <vector>

namespace sw
{
    class MenuItem
    {
    private:
        /**
         * @brief 菜单项文本
         */
        std::wstring _text;

        /**
         * @brief 是否为分隔条
         */
        bool _isSeparator;

        /**
         * @brief 子项
         */
        std::vector<MenuItem> _subItems;

    public:
        /**
         * @brief 创建一个菜单项
         * @param text 菜单项的文本，当文本为“-”时表示该项为一个分隔条
         */
        MenuItem(const std::wstring text);

        /**
         * @brief 获取当前菜单项的文本
         */
        const std::wstring &GetText();

        /**
         * @brief  获取一个bool值，表示当前项是否为分隔条
         */
        bool IsSparator();

        /**
         * @brief  获取子项的数量
         * @return 子项数量
         */
        int GetSubItemCount();

        /**
         * @brief       获取指定索引处的子项
         * @param index 要找子项的索引
         */
        MenuItem &GetSubItemAt(int index);

        /**
         * @brief          添加一个子项到末尾
         * @param itemText 要添加的子项文本
         * @return         新添加的子项的引用
         */
        MenuItem &AppendSubItem(const std::wstring &itemText);

        /**
         * @brief 添加一个分隔条
         */
        void AppendSeparator();
    };
}
