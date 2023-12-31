#pragma once

#include "Utils.h"
#include <map>
#include <memory>
#include <string>

namespace sw
{
    template <typename TKey, typename TVal>
    class Dictionary; // 向前声明

    /**
     * @brief 以字符串为键值的字典
     */
    template <typename TVal>
    using StrDictionary = Dictionary<std::wstring, TVal>;

    /**
     * @brief 字典类，内部维护了一个指向std::map的智能指针
     */
    template <typename TKey, typename TVal>
    class Dictionary
    {
    private:
        /**
         * @brief 指向std::map的智能指针
         */
        std::shared_ptr<std::map<TKey, TVal>> _pMap;

    public:
        /**
         * @brief 初始化字典
         */
        Dictionary()
            : _pMap(new std::map<TKey, TVal>)
        {
        }

        /**
         * @brief 使用初始化列表
         */
        Dictionary(std::initializer_list<std::pair<const TKey, TVal>> list)
            : Dictionary()
        {
            this->_pMap->insert(list);
        }

        /**
         * @brief 正向迭代器开始
         */
        auto begin() const
        {
            return this->_pMap->begin();
        }

        /**
         * @brief 正向迭代器结束
         */
        auto end() const
        {
            return this->_pMap->end();
        }

        /**
         * @brief 反向迭代器开始
         */
        auto rbegin() const
        {
            return this->_pMap->rbegin();
        }

        /**
         * @brief 反向迭代器结束
         */
        auto rend() const
        {
            return this->_pMap->rend();
        }

        /**
         * @brief     获取或设置值
         * @param key 键值
         */
        auto &operator[](const TKey &key) const
        {
            return this->_pMap->operator[](key);
        }

        /**
         * @brief 判断是否为同一个字典
         */
        friend bool operator==(const Dictionary &left, const Dictionary &right)
        {
            return left._pMap == right._pMap;
        }

        /**
         * @brief 判断是否不是同一个字典
         */
        friend bool operator!=(const Dictionary &left, const Dictionary &right)
        {
            return left._pMap != right._pMap;
        }

        /**
         * @brief 支持Utils::BuildStr
         */
        friend std::wostream &operator<<(std::wostream &wos, const Dictionary &dic)
        {
            wos << Utils::BuildStr(*dic._pMap);
            return wos;
        }

        /**
         * @brief 获取键值对个数
         */
        int Count() const
        {
            return (int)this->_pMap->size();
        }

        /**
         * @brief 字典是否为空
         */
        bool IsEmpty() const
        {
            return this->_pMap->empty();
        }

        /**
         * @brief     是否存在某个键值
         * @param key 要查询的键值
         */
        bool ContainsKey(const TKey &key) const
        {
            return this->_pMap->count(key);
        }

        /**
         * @brief     移除指定键值对
         * @param key 要删除的键值
         */
        void Remove(const TKey &key) const
        {
            this->_pMap->erase(key);
        }

        /**
         * @brief 清空字典
         */
        void Clear() const
        {
            this->_pMap->clear();
        }

        /**
         * @brief 复制当前字典
         */
        Dictionary Copy() const
        {
            Dictionary dic;
            dic._pMap->insert(this->_pMap->begin(), this->_pMap->end());
            return dic;
        }
    };
}
