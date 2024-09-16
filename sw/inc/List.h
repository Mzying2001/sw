#pragma once

#include "Utils.h"
#include <algorithm>
#include <memory>
#include <string>
#include <vector>

namespace sw
{
    template <typename T>
    class List; // 向前声明

    /**
     * @brief 字符串列表
     */
    using StrList = List<std::wstring>;

    /**
     * @brief 列表类，内部维护了一个指向std::vector的智能指针
     */
    template <typename T>
    class List
    {
    private:
        /**
         * @brief 指向std::vector的智能指针
         */
        std::shared_ptr<std::vector<T>> _pVec;

    public:
        /**
         * @brief 初始化列表
         */
        List()
            : _pVec(std::make_shared<std::vector<T>>())
        {
        }

        /**
         * @brief 使用初始化列表
         */
        List(std::initializer_list<T> list)
            : _pVec(std::make_shared<std::vector<T>>(list))
        {
        }

        /**
         * @brief 初始化列表并指定容量
         */
        explicit List(int capacity)
            : List()
        {
            this->_pVec->reserve(capacity);
        }

        /**
         * @brief 正向迭代器开始
         */
        auto begin() const
        {
            return this->_pVec->begin();
        }

        /**
         * @brief 正向迭代器结束
         */
        auto end() const
        {
            return this->_pVec->end();
        }

        /**
         * @brief 反向迭代器开始
         */
        auto rbegin() const
        {
            return this->_pVec->rbegin();
        }

        /**
         * @brief 反向迭代器结束
         */
        auto rend() const
        {
            return this->_pVec->rend();
        }

        /**
         * @brief 获取或设置列表中指定位置的值
         */
        auto &operator[](int index) const
        {
            return this->_pVec->at(index);
        }

        /**
         * @brief 判断是否为同一个列表
         */
        bool operator==(const List &other) const
        {
            return this->_pVec == other._pVec;
        }

        /**
         * @brief 判断是否不是同一个列表
         */
        bool operator!=(const List &other) const
        {
            return this->_pVec != other._pVec;
        }

        /**
         * @brief 列表当前的容量
         */
        int Capacity() const
        {
            return (int)this->_pVec->capacity();
        }

        /**
         * @brief 获取元素个数
         */
        int Count() const
        {
            return (int)this->_pVec->size();
        }

        /**
         * @brief 列表是否为空
         */
        bool IsEmpty() const
        {
            return this->_pVec->empty();
        }

        /**
         * @brief 添加一个值到列表末尾
         */
        auto &Append(const T &value) const
        {
            this->_pVec->push_back(value);
            return *this;
        }

        /**
         * @brief 添加一个值到列表末尾
         */
        auto &Append(T &&value) const
        {
            this->_pVec->push_back(std::forward<T>(value));
            return *this;
        }

        /**
         * @brief 在指定位置插入值
         */
        void Insert(int index, const T &value) const
        {
            this->_pVec->insert(this->_pVec->begin() + index, value);
        }

        /**
         * @brief 在指定位置插入值
         */
        void Insert(int index, T &&value) const
        {
            this->_pVec->insert(this->_pVec->begin() + index, std::forward<T>(value));
        }

        /**
         * @brief       列表是否包含某个值
         * @param value 要查找的值
         */
        bool Contains(const T &value) const
        {
            return std::find(this->_pVec->begin(), this->_pVec->end(), value) != this->_pVec->end();
        }

        /**
         * @brief       查找值在列表中的索引
         * @param value 要查找的值
         * @return      若列表中包含该值则返回其索引，否则返回-1
         */
        int IndexOf(const T &value) const
        {
            auto it = std::find(this->_pVec->begin(), this->_pVec->end(), value);
            return it == this->_pVec->end() ? -1 : int(it - this->_pVec->begin());
        }

        /**
         * @brief       移除列表中第一个指定的值
         * @param value 要移除的值
         * @return      是否成功移除
         */
        bool Remove(const T &value) const
        {
            auto it = std::find(this->_pVec->begin(), this->_pVec->end(), value);
            if (it == this->_pVec->end())
                return false;
            this->_pVec->erase(it);
            return true;
        }

        /**
         * @brief       移除指定索引处的值
         * @param index 要移除元素的索引
         */
        void RemoveAt(int index) const
        {
            this->_pVec->erase(this->_pVec->begin() + index);
        }

        /**
         * @brief 清空列表
         */
        void Clear() const
        {
            this->_pVec->clear();
        }

        /**
         * @brief 复制当前列表
         */
        List Copy() const
        {
            List list((int)this->_pVec->capacity());
            list._pVec->assign(this->_pVec->begin(), this->_pVec->end());
            return list;
        }

        /**
         * @brief 支持Utils::BuildStr
         */
        friend std::wostream &operator<<(std::wostream &wos, const List &list)
        {
            wos << Utils::BuildStr(*list._pVec);
            return wos;
        }
    };
}
