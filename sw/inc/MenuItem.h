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
    class MenuItem; // 向前声明

    /**
     * @brief 菜单项关联的回调函数类型
     */
    using MenuItemCommand = std::function<void(MenuItem &)>;

    /**
     * @brief 菜单项
     */
    class MenuItem : public ITag
    {
    public:
        /**
         * @brief 储存用户自定义信息
         */
        uint64_t tag;

        /**
         * @brief 菜单项的文本，当值为“-”时表示当前项为分隔条
         */
        std::wstring text;

        /**
         * @brief 菜单项被单击时调用的函数
         */
        MenuItemCommand command;

        /**
         * @brief 子项
         */
        std::vector<std::shared_ptr<MenuItem>> subItems{};

    public:
        /**
         * @brief      构造一个MenuItem，并设置文本
         * @param text 菜单项的文本
         */
        MenuItem(const std::wstring &text);

        /**
         * @brief         构造一个MenuItem，并设置其回调函数
         * @param text    菜单项的文本
         * @param command 被单击时调用的函数
         */
        MenuItem(const std::wstring &text, const MenuItemCommand &command);

        /**
         * @brief          构造一个MenuItem，并设置其子项
         * @param text     菜单下的文本
         * @param subItems 子项列表
         */
        MenuItem(const std::wstring &text, std::initializer_list<MenuItem> subItems);

        /**
         * @brief      构造一个MenuItem，并设置tag及文本
         * @param text 菜单项的文本
         */
        MenuItem(uint64_t tag, const std::wstring &text);

        /**
         * @brief         构造一个MenuItem，并设置tag及回调函数
         * @param text    菜单项的文本
         * @param command 被单击时调用的函数
         */
        MenuItem(uint64_t tag, const std::wstring &text, const MenuItemCommand &command);

        /**
         * @brief         构造一个MenuItem，设置成员函数为回调函数
         * @tparam T      成员函数所在的类
         * @param obj     成员函数所在的对象
         * @param handler 处理函数
         */
        template <typename T>
        MenuItem(const std::wstring &text, T &obj, void (T::*handler)(MenuItem &))
            : MenuItem(0, text, obj, handler)
        {
        }

        /**
         * @brief         构造一个MenuItem，设置成员函数为回调函数
         * @tparam T      成员函数所在的类
         * @param obj     成员函数所在的对象
         * @param handler 处理函数
         */
        template <typename T>
        MenuItem(uint64_t tag, const std::wstring &text, T &obj, void (T::*handler)(MenuItem &))
            : MenuItem(tag, text)
        {
            T *pObj       = &obj;
            this->command = [pObj, handler](MenuItem &item) { (pObj->*handler)(item); };
        }

    public:
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
        virtual uint64_t GetTag() override;

        /**
         * @brief 设置Tag
         */
        virtual void SetTag(uint64_t tag) override;
    };
}
