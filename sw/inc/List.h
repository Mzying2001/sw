#pragma once

#include "IList.h"
#include "IToString.h"
#include "Utils.h"
#include <algorithm>

namespace sw
{
    /**
     * @brief 基于std::vector的泛型列表，实现IListT接口
     * @tparam T 列表元素类型
     */
    template <typename T>
    class List final : public IListT<T>,
                       public IToString<List<T>>
    {
    private:
        /**
         * @brief 底层数据存储
         */
        std::vector<T> _data;

    public:
        /**
         * @brief 默认构造函数，创建空列表
         */
        List() = default;

        /**
         * @brief 使用初始化列表构造
         * @param list 初始化元素列表
         */
        List(std::initializer_list<T> list)
            : _data(list)
        {
        }

        /**
         * @brief 指定初始容量构造
         * @param capacity 初始容量
         */
        explicit List(int capacity)
            : _data()
        {
            _data.reserve(static_cast<size_t>(capacity));
        }

        /**
         * @brief 拷贝构造函数
         * @param other 源列表
         */
        List(const List<T> &other)
            : _data(other._data)
        {
        }

        /**
         * @brief 移动构造函数
         * @param other 源列表
         */
        List(List<T> &&other) noexcept
            : _data(std::move(other._data))
        {
        }

        /**
         * @brief 拷贝赋值运算符
         * @param other 源列表
         * @return 当前列表的引用
         */
        List<T> &operator=(const List<T> &other)
        {
            if (this != &other) {
                _data = other._data;
            }
            return *this;
        }

        /**
         * @brief 移动赋值运算符
         * @param other 源列表
         * @return 当前列表的引用
         */
        List<T> &operator=(List<T> &&other) noexcept
        {
            if (this != &other) {
                _data = std::move(other._data);
            }
            return *this;
        }

        /**
         * @brief 获取当前分配的容量
         * @return 容量大小
         */
        int Capacity() const noexcept
        {
            return static_cast<int>(_data.capacity());
        }

        /**
         * @brief 预留至少指定数量的元素空间
         * @param newCapacity 要预留的容量，仅在大于当前容量时生效
         */
        void Reserve(int newCapacity)
        {
            if (newCapacity > Capacity()) {
                _data.reserve(static_cast<size_t>(newCapacity));
            }
        }

        /**
         * @brief 清空列表中的所有元素
         */
        void Clear()
        {
            _data.clear();
        }

        /**
         * @brief 在列表末尾追加元素
         * @param value 要追加的值
         * @return 当前列表的引用，支持链式调用
         */
        List<T> &Append(const T &value)
        {
            _data.push_back(value);
            return *this;
        }

        /**
         * @brief 在列表末尾追加元素（移动语义）
         * @param value 要追加的值
         * @return 当前列表的引用，支持链式调用
         */
        List<T> &Append(T &&value)
        {
            _data.push_back(std::move(value));
            return *this;
        }

        /**
         * @brief 移除指定索引处的元素
         * @param index 要移除的元素索引
         * @return 当前列表的引用，支持链式调用
         * @throws std::out_of_range 索引超出范围
         */
        List<T> &RemoveAt(int index)
        {
            if (index < 0 || index >= Count()) {
                throw std::out_of_range("Index out of range in List::RemoveAt.");
            }
            _data.erase(_data.begin() + static_cast<size_t>(index));
            return *this;
        }

        /**
         * @brief 在指定索引处插入元素
         * @param index 插入位置
         * @param value 要插入的值
         * @return 当前列表的引用，支持链式调用
         * @throws std::out_of_range 索引超出范围
         */
        List<T> &Insert(int index, const T &value)
        {
            if (index < 0 || index > Count()) {
                throw std::out_of_range("Index out of range in List::Insert.");
            }
            _data.insert(_data.begin() + static_cast<size_t>(index), value);
            return *this;
        }

        /**
         * @brief 在指定索引处插入元素（移动语义）
         * @param index 插入位置
         * @param value 要插入的值
         * @return 当前列表的引用，支持链式调用
         * @throws std::out_of_range 索引超出范围
         */
        List<T> &Insert(int index, T &&value)
        {
            if (index < 0 || index > Count()) {
                throw std::out_of_range("Index out of range in List::Insert.");
            }
            _data.insert(_data.begin() + static_cast<size_t>(index), std::move(value));
            return *this;
        }

        /**
         * @brief 查找指定值在列表中首次出现的索引
         * @param value 要查找的值
         * @return 首次出现的索引，未找到返回-1
         */
        int IndexOf(const T &value) const
        {
            auto it = std::find(_data.begin(), _data.end(), value);
            return it != _data.end() ? static_cast<int>(std::distance(_data.begin(), it)) : -1;
        }

        /**
         * @brief 查找指定值在列表中最后出现的索引
         * @param value 要查找的值
         * @return 最后出现的索引，未找到返回-1
         */
        int LastIndexOf(const T &value) const
        {
            auto it = std::find(_data.rbegin(), _data.rend(), value);
            return it != _data.rend() ? static_cast<int>(std::distance(it, _data.rend()) - 1) : -1;
        }

        /**
         * @brief 判断列表是否包含指定值
         * @param value 要查找的值
         * @return 包含返回true，否则返回false
         */
        bool Contains(const T &value) const
        {
            return std::find(_data.begin(), _data.end(), value) != _data.end();
        }

        /**
         * @brief 移除列表中首次出现的指定值
         * @param value 要移除的值
         * @return 成功移除返回true，未找到返回false
         */
        bool Remove(const T &value)
        {
            int index = IndexOf(value);
            if (index == -1) {
                return false;
            } else {
                return RemoveAt(index), true;
            }
        }

        /**
         * @brief 将列表转换为字符串表示
         * @return 列表的字符串表示
         */
        std::wstring ToString() const
        {
            return Utils::BuildStr(_data);
        }

        /**
         * @brief 获取底层std::vector的引用
         * @return std::vector的引用
         */
        std::vector<T> &GetStdVector() noexcept
        {
            return _data;
        }

        /**
         * @brief 获取底层std::vector的const引用
         * @return std::vector的const引用
         */
        const std::vector<T> &GetStdVector() const noexcept
        {
            return _data;
        }

    private:
        /**
         * @brief 设置元素值的实现（T可拷贝赋值时）
         * @param index 元素索引
         * @param value 要设置的值
         * @throws std::out_of_range 索引超出范围
         */
        template <typename U = T>
        auto SetAtImpl(int index, _OptimalParamType<T> value)
            -> typename std::enable_if<std::is_copy_assignable<U>::value>::type
        {
            if (index < 0 || index >= Count()) {
                throw std::out_of_range("Index out of range in List::SetAt.");
            }
            _data[static_cast<size_t>(index)] = value;
        }

        /**
         * @brief 设置元素值的实现（T不可拷贝赋值时，抛出异常）
         * @param index 元素索引
         * @param value 要设置的值
         * @throws std::logic_error T不可拷贝赋值
         */
        template <typename U = T>
        auto SetAtImpl(int index, _OptimalParamType<T> value)
            -> typename std::enable_if<!std::is_copy_assignable<U>::value>::type
        {
            throw std::logic_error("Type T must be copy assignable to use SetAt in List.");
        }

    public:
        /**
         * @brief 返回列表中的元素数量
         * @return 元素数量
         */
        virtual int Count() const noexcept override
        {
            return static_cast<int>(_data.size());
        }

        /**
         * @brief 获取指定索引处的元素引用
         * @param index 元素索引
         * @return 元素引用
         * @throws std::out_of_range 索引超出范围
         */
        virtual T &GetAt(int index) override
        {
            if (index < 0 || index >= Count()) {
                throw std::out_of_range("Index out of range in List::GetAt.");
            }
            return _data[static_cast<size_t>(index)];
        }

        /**
         * @brief 获取指定索引处的const元素引用
         * @param index 元素索引
         * @return const元素引用
         * @throws std::out_of_range 索引超出范围
         */
        virtual const T &GetAt(int index) const override
        {
            if (index < 0 || index >= Count()) {
                throw std::out_of_range("Index out of range in List::GetAt.");
            }
            return _data[static_cast<size_t>(index)];
        }

        /**
         * @brief 设置指定索引处的元素值
         * @param index 元素索引
         * @param value 要设置的值
         * @throws std::out_of_range 索引超出范围
         * @throws std::logic_error 类型T不支持复制赋值
         */
        virtual void SetAt(int index, _OptimalParamType<T> value) override
        {
            SetAtImpl(index, value);
        }
    };
}
