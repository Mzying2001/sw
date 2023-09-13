#pragma once

#include "ITag.h"
#include <Windows.h>
#include <functional>
#include <initializer_list>
#include <memory>
#include <string>
#include <vector>

namespace sw
{
    class MenuItem : public ITag
    {
    public:
        /**
         * @brief 储存用户自定义信息
         */
        uint64_t tag = 0;

        /**
         * @brief 菜单项的文本，当值为“-”时表示当前项为分隔条
         */
        std::wstring text;

        /**
         * @brief 子项
         */
        std::vector<std::shared_ptr<MenuItem>> subItems;

        /**
         * @brief 菜单项被单击时调用的函数
         */
        std::function<void(MenuItem &)> command;

        /**
         * @brief      构造一个MenuItem，并设置文本
         * @param text 菜单项的文本
         */
        MenuItem(const std::wstring &text);

        /**
         * @brief          构造一个MenuItem，并设置其子项
         * @param text     菜单下的文本
         * @param subItems 子项列表
         */
        MenuItem(const std::wstring &text, std::initializer_list<MenuItem> subItems);

        /**
         * @brief         构造一个MenuItem，并设置其回调函数
         * @param text    菜单项的文本
         * @param command 被单击时调用的函数
         */
        MenuItem(const std::wstring &text, const decltype(command) &command);

        /**
         * @brief 获取一个值，表示当前菜单项是否为分隔条
         */
        bool IsSeparator() const;

        /**
         * @brief 调用command
         */
        void CallCommand();

        /**
         * @brief 获取Tag
         */
        virtual uint64_t GetTag();

        /**
         * @brief 设置Tag
         */
        virtual void SetTag(uint64_t tag);
    };
}
