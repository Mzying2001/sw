#pragma once

#include "IToString.h"
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
    class Dictionary : public IToString<Dictionary<TKey, TVal>>
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
            : _pMap(std::make_shared<std::map<TKey, TVal>>())
        {
        }

        /**
         * @brief 使用初始化列表
         */
        Dictionary(std::initializer_list<std::pair<const TKey, TVal>> list)
            : _pMap(std::make_shared<std::map<TKey, TVal>>(list))
        {
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
            return this->_pMap->at(key);
        }

        /**
         * @brief 判断是否为同一个字典
         */
        bool operator==(const Dictionary &other) const
        {
            return this->_pMap == other._pMap;
        }

        /**
         * @brief 判断是否不是同一个字典
         */
        bool operator!=(const Dictionary &other) const
        {
            return this->_pMap != other._pMap;
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
         * @brief  添加键值对到字典
         * @return 当前字典
         */
        auto Add(const TKey &key, const TVal &value) const
        {
            this->_pMap->insert(std::make_pair(key, value));
            return *this;
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
         * @brief       遍历字典，查询是否存在某个值
         * @param value 要查询的值
         */
        bool ContainsValue(const TVal &value) const
        {
            for (const auto &pair : *this->_pMap) {
                if (pair.second == value) {
                    return true;
                }
            }
            return false;
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

        /**
         * @brief 获取字典内部维护的std::map
         */
        std::map<TKey, TVal> &GetStdMap() const
        {
            return *this->_pMap;
        }

        /**
         * @brief 获取描述当前对象的字符串
         */
        std::wstring ToString() const
        {
            return Utils::BuildStr(*this->_pMap);
        }
    };
}
