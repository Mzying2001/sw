#pragma once

#include "INotifyCollectionChanged.h"
#include "List.h"
#include "ObservableObject.h"
#include <initializer_list>
#include <stdexcept>

namespace sw
{
    /**
     * @brief 支持集合变更通知的泛型集合类
     * @tparam T 集合元素类型
     */
    template <typename T>
    class ObservableCollection : public ObservableObject,
                                 public IListT<T>,
                                 public INotifyCollectionChanged,
                                 public IToString<ObservableCollection<T>>
    {
    private:
        /**
         * @brief 内部列表存储
         */
        List<T> _items;

        /**
         * @brief 集合变更事件委托
         */
        NotifyCollectionChangedEventHandler _collectionChanged;

    public:
        /**
         * @brief 默认构造函数，创建空集合
         */
        ObservableCollection() = default;

        // 删除拷贝构造函数
        ObservableCollection(const ObservableCollection<T> &) = delete;

        // 删除移动构造函数
        ObservableCollection(ObservableCollection<T> &&) = delete;

        // 删除拷贝赋值运算符
        ObservableCollection<T> &operator=(const ObservableCollection<T> &) = delete;

        // 删除移动赋值运算符
        ObservableCollection<T> &operator=(ObservableCollection<T> &&) = delete;

        /**
         * @brief 使用初始化列表构造
         * @param list 初始化元素列表
         */
        ObservableCollection(std::initializer_list<T> list)
            : _items(list)
        {
        }

        /**
         * @brief 指定初始容量构造
         * @param capacity 初始容量
         */
        explicit ObservableCollection(int capacity)
            : _items(capacity)
        {
        }

    protected:
        /**
         * @brief 获取集合变更事件委托的引用
         * @note CollectionChanged事件使用该函数返回的委托来保存事件处理程序
         */
        virtual NotifyCollectionChangedEventHandler &GetCollectionChangedEventDelegate() override final
        {
            return _collectionChanged;
        }

        /**
         * @brief 触发集合变更事件
         * @param args 集合变更事件参数
         */
        virtual void OnCollectionChanged(NotifyCollectionChangedEventArgs &args)
        {
            if (_collectionChanged) {
                _collectionChanged(*this, args);
            }
        }

    public:
        /**
         * @brief 获取当前分配的容量
         * @return 容量大小
         */
        int Capacity() const noexcept
        {
            return _items.Capacity();
        }

        /**
         * @brief 预留至少指定数量的元素空间
         * @param newCapacity 要预留的容量，仅在大于当前容量时生效
         */
        void Reserve(int newCapacity)
        {
            _items.Reserve(newCapacity);
        }

        /**
         * @brief 刷新集合，触发集合重置通知
         */
        void Refresh()
        {
            NotifyCollectionChangedEventArgs args{};
            args.action = NotifyCollectionChangedAction::Reset;
            args.list   = this;
            OnCollectionChanged(args);
        }

        /**
         * @brief 清空集合中的所有元素，并触发集合重置通知
         */
        void Clear()
        {
            if (_items.Count() == 0) {
                return;
            }

            _items.Clear();

            NotifyCollectionChangedEventArgs args{};
            args.action = NotifyCollectionChangedAction::Reset;
            args.list   = this;
            OnCollectionChanged(args);
        }

        /**
         * @brief 在集合末尾追加元素，并触发添加通知
         * @param value 要追加的值
         * @return 当前集合的引用，支持链式调用
         */
        ObservableCollection<T> &Add(const T &value)
        {
            int index = _items.Count();
            _items.Add(value);

            NotifyCollectionChangedEventArgs args{};
            args.action = NotifyCollectionChangedAction::Add;
            args.list   = this;
            args.index  = index;
            OnCollectionChanged(args);

            return *this;
        }

        /**
         * @brief 在集合末尾追加元素（移动语义），并触发添加通知
         * @param value 要追加的值
         * @return 当前集合的引用，支持链式调用
         */
        ObservableCollection<T> &Add(T &&value)
        {
            int index = _items.Count();
            _items.Add(std::move(value));

            NotifyCollectionChangedEventArgs args{};
            args.action = NotifyCollectionChangedAction::Add;
            args.list   = this;
            args.index  = index;
            OnCollectionChanged(args);

            return *this;
        }

        /**
         * @brief 移除指定索引处的元素，并触发移除通知
         * @param index 要移除的元素索引
         * @return 当前集合的引用，支持链式调用
         * @throws std::out_of_range 索引超出范围
         */
        ObservableCollection<T> &RemoveAt(int index)
        {
            _items.RemoveAt(index);

            NotifyCollectionChangedEventArgs args{};
            args.action = NotifyCollectionChangedAction::Remove;
            args.list   = this;
            args.index  = index;
            OnCollectionChanged(args);

            return *this;
        }

        /**
         * @brief 在指定索引处插入元素，并触发添加通知
         * @param index 插入位置
         * @param value 要插入的值
         * @return 当前集合的引用，支持链式调用
         * @throws std::out_of_range 索引超出范围
         */
        ObservableCollection<T> &Insert(int index, const T &value)
        {
            _items.Insert(index, value);

            NotifyCollectionChangedEventArgs args{};
            args.action = NotifyCollectionChangedAction::Add;
            args.list   = this;
            args.index  = index;
            OnCollectionChanged(args);

            return *this;
        }

        /**
         * @brief 在指定索引处插入元素（移动语义），并触发添加通知
         * @param index 插入位置
         * @param value 要插入的值
         * @return 当前集合的引用，支持链式调用
         * @throws std::out_of_range 索引超出范围
         */
        ObservableCollection<T> &Insert(int index, T &&value)
        {
            _items.Insert(index, std::move(value));

            NotifyCollectionChangedEventArgs args{};
            args.action = NotifyCollectionChangedAction::Add;
            args.list   = this;
            args.index  = index;
            OnCollectionChanged(args);

            return *this;
        }

        /**
         * @brief 将元素从一个索引移动到另一个索引，并触发移动通知
         * @param oldIndex 要移动的元素原索引
         * @param newIndex 移动后的元素索引
         * @return 当前集合的引用，支持链式调用
         * @throws std::out_of_range 索引超出范围
         */
        ObservableCollection<T> &Move(int oldIndex, int newIndex)
        {
            int count = _items.Count();
            if (oldIndex < 0 || oldIndex >= count || newIndex < 0 || newIndex >= count) {
                throw std::out_of_range("Index out of range in ObservableCollection::Move.");
            }

            if (oldIndex == newIndex) {
                return *this;
            }

            auto &items = _items.GetStdVector();

            T value = std::move(items[static_cast<size_t>(oldIndex)]);
            items.erase(items.begin() + static_cast<size_t>(oldIndex));
            items.insert(items.begin() + static_cast<size_t>(newIndex), std::move(value));

            NotifyCollectionChangedEventArgs args{};
            args.action   = NotifyCollectionChangedAction::Move;
            args.list     = this;
            args.index    = newIndex;
            args.oldIndex = oldIndex;
            OnCollectionChanged(args);

            return *this;
        }

        /**
         * @brief 查找指定值在集合中首次出现的索引
         * @param value 要查找的值
         * @return 首次出现的索引，未找到返回-1
         */
        int IndexOf(const T &value) const
        {
            return _items.IndexOf(value);
        }

        /**
         * @brief 查找指定值在集合中最后出现的索引
         * @param value 要查找的值
         * @return 最后出现的索引，未找到返回-1
         */
        int LastIndexOf(const T &value) const
        {
            return _items.LastIndexOf(value);
        }

        /**
         * @brief 判断集合是否包含指定值
         * @param value 要查找的值
         * @return 包含返回true，否则返回false
         */
        bool Contains(const T &value) const
        {
            return _items.Contains(value);
        }

        /**
         * @brief 移除集合中首次出现的指定值，并在成功移除时触发移除通知
         * @param value 要移除的值
         * @return 成功移除返回true，未找到返回false
         */
        bool Remove(const T &value)
        {
            int index = _items.IndexOf(value);
            if (index == -1) {
                return false;
            }

            _items.RemoveAt(index);

            NotifyCollectionChangedEventArgs args{};
            args.action = NotifyCollectionChangedAction::Remove;
            args.list   = this;
            args.index  = index;
            OnCollectionChanged(args);

            return true;
        }

        /**
         * @brief 将集合转换为字符串表示
         * @return 集合的字符串表示
         */
        std::wstring ToString() const
        {
            return _items.ToString();
        }

        /**
         * @brief 获取底层std::vector的引用
         * @return std::vector的引用
         */
        std::vector<T> &GetStdVector() noexcept
        {
            return _items.GetStdVector();
        }

        /**
         * @brief 获取底层std::vector的const引用
         * @return std::vector的const引用
         */
        const std::vector<T> &GetStdVector() const noexcept
        {
            return _items.GetStdVector();
        }

    public:
        /**
         * @brief 返回列表中的元素数量
         * @return 元素数量
         */
        virtual int Count() const noexcept override final
        {
            return _items.Count();
        }

        /**
         * @brief 获取指定索引处的元素引用
         * @param index 元素索引
         * @return 元素引用
         * @throws std::out_of_range 索引超出范围
         */
        virtual T &GetAt(int index) override final
        {
            return _items.GetAt(index);
        }

        /**
         * @brief 获取指定索引处的const元素引用
         * @param index 元素索引
         * @return const元素引用
         * @throws std::out_of_range 索引超出范围
         */
        virtual const T &GetAt(int index) const override final
        {
            return _items.GetAt(index);
        }

        /**
         * @brief 设置指定索引处的元素值
         * @param index 元素索引
         * @param value 要设置的值
         * @throws std::out_of_range 索引超出范围
         * @throws std::logic_error T不可复制赋值时
         */
        virtual void SetAt(int index, const T &value) override final
        {
            _items.SetAt(index, value);

            NotifyCollectionChangedEventArgs args{};
            args.action = NotifyCollectionChangedAction::Replace;
            args.list   = this;
            args.index  = index;
            OnCollectionChanged(args);
        }

        /**
         * @brief 设置指定索引处的元素值（移动语义）
         * @param index 元素索引
         * @param value 要设置的值
         * @throws std::out_of_range 索引超出范围
         * @throws std::logic_error T不可移动赋值时
         */
        virtual void SetAt(int index, T &&value) override final
        {
            _items.SetAt(index, std::move(value));

            NotifyCollectionChangedEventArgs args{};
            args.action = NotifyCollectionChangedAction::Replace;
            args.list   = this;
            args.index  = index;
            OnCollectionChanged(args);
        }
    };
}
