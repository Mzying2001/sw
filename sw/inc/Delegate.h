#pragma once

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <typeinfo>
#include <vector>

namespace sw
{
    // ICallable接口声明
    template <typename>
    struct ICallable;

    // Delegate类声明
    template <typename>
    class Delegate;

    /**
     * @brief ICallable接口，用于表示可调用对象的接口
     */
    template <typename TRet, typename... Args>
    struct ICallable<TRet(Args...)> {
        /**
         * @brief 析构函数
         */
        virtual ~ICallable() = default;

        /**
         * @brief      调用函数
         * @param args 函数参数
         * @return     函数返回值
         */
        virtual TRet Invoke(Args... args) const = 0;

        /**
         * @brief 克隆当前可调用对象
         */
        virtual ICallable *Clone() const = 0;

        /**
         * @brief 获取当前可调用对象的类型信息
         */
        virtual const std::type_info &GetTypeInfo() const = 0;

        /**
         * @brief       判断当前可调用对象是否与另一个可调用对象相等
         * @param other 另一个可调用对象
         * @return      如果相等则返回true，否则返回false
         */
        virtual bool Equals(const ICallable &other) const = 0;
    };

    /**
     * @brief 用于存储和管理多个可调用对象的列表，针对单个可调用对象的情况进行优化
     */
    template <typename T>
    class CallableList
    {
    public:
        /**
         * @brief 可调用对象类型别名
         */
        using TCallable = ICallable<T>;

        /**
         * @brief 智能指针类型别名，用于存储可调用对象的唯一指针
         */
        using TSinglePtr = std::unique_ptr<TCallable>;

        /**
         * @brief 列表类型别名，用于存储多个可调用对象的智能指针
         */
        using TSharedList = std::vector<std::shared_ptr<TCallable>>;

    private:
        /**
         * @brief 内部存储可调用对象的联合体
         */
        mutable union {
            alignas(TSinglePtr) uint8_t _single[sizeof(TSinglePtr)];
            alignas(TSharedList) uint8_t _list[sizeof(TSharedList)];
        } _data = {};

        /**
         * @brief 当前状态枚举
         */
        enum : uint8_t {
            STATE_NONE,   // 未存储任何可调用对象
            STATE_SINGLE, // 储存了一个可调用对象
            STATE_LIST,   // 储存了多个可调用对象
        } _state = STATE_NONE;

    public:
        /**
         * @brief 默认构造函数
         */
        CallableList()
        {
        }

        /**
         * @brief 拷贝构造函数
         */
        CallableList(const CallableList &other)
        {
            *this = other;
        }

        /**
         * @brief 移动构造函数
         */
        CallableList(CallableList &&other) noexcept
        {
            *this = std::move(other);
        }

        /**
         * @brief 拷贝赋值运算
         */
        CallableList &operator=(const CallableList &other)
        {
            if (this == &other) {
                return *this;
            }

            _Reset(other._state);

            switch (other._state) {
                case STATE_SINGLE: {
                    _GetSingle().reset(other._GetSingle()->Clone());
                    break;
                }
                case STATE_LIST: {
                    _GetList() = other._GetList();
                    break;
                }
            }
            return *this;
        }

        /**
         * @brief 移动赋值运算
         */
        CallableList &operator=(CallableList &&other) noexcept
        {
            if (this == &other) {
                return *this;
            }

            _Reset(other._state);

            switch (other._state) {
                case STATE_SINGLE: {
                    _GetSingle() = std::move(other._GetSingle());
                    other._Reset();
                    break;
                }
                case STATE_LIST: {
                    _GetList() = std::move(other._GetList());
                    other._Reset();
                    break;
                }
            }
            return *this;
        }

        /**
         * @brief 析构函数
         */
        ~CallableList()
        {
            _Reset();
        }

        /**
         * @brief  获取当前存储的可调用对象数量
         * @return 可调用对象的数量
         */
        size_t Count() const noexcept
        {
            switch (_state) {
                case STATE_SINGLE: {
                    return 1;
                }
                case STATE_LIST: {
                    return _GetList().size();
                }
                default: {
                    return 0;
                }
            }
        }

        /**
         * @brief  判断当前存储的可调用对象是否为空
         * @return 如果没有存储任何可调用对象则返回true，否则返回false
         */
        bool IsEmpty() const noexcept
        {
            return _state == STATE_NONE;
        }

        /**
         * @brief 清空当前存储的可调用对象
         */
        void Clear() noexcept
        {
            _Reset();
        }

        /**
         * @brief 添加一个可调用对象到列表中
         * @note  传入对象的生命周期将由CallableList管理
         */
        void Add(TCallable *callable)
        {
            if (callable == nullptr) {
                return;
            }

            switch (_state) {
                case STATE_NONE: {
                    _Reset(STATE_SINGLE);
                    _GetSingle().reset(callable);
                    break;
                }
                case STATE_SINGLE: {
                    TSharedList list;
                    list.emplace_back(_GetSingle().release());
                    list.emplace_back(callable);
                    _Reset(STATE_LIST);
                    _GetList() = std::move(list);
                    break;
                }
                case STATE_LIST: {
                    _GetList().emplace_back(callable);
                    break;
                }
            }
        }

        /**
         * @brief  移除指定索引处的可调用对象
         * @return 如果成功移除则返回true，否则返回false
         */
        bool RemoveAt(size_t index) noexcept
        {
            switch (_state) {
                case STATE_SINGLE: {
                    if (index != 0) {
                        return false;
                    } else {
                        _Reset();
                        return true;
                    }
                }
                case STATE_LIST: {
                    auto &list = _GetList();
                    if (index >= list.size()) {
                        return false;
                    }
                    list.erase(list.begin() + index);
                    if (list.empty()) {
                        _Reset();
                    } else if (list.size() == 1) {
                        auto ptr = list.front()->Clone();
                        _Reset(STATE_SINGLE);
                        _GetSingle().reset(ptr);
                    }
                    return true;
                }
                default: {
                    return false;
                }
            }
        }

        /**
         * @brief  获取指定索引处的可调用对象
         * @return 如果索引有效则返回对应的可调用对象，否则返回nullptr
         */
        TCallable *GetAt(size_t index) const noexcept
        {
            switch (_state) {
                case STATE_SINGLE: {
                    return index == 0 ? _GetSingle().get() : nullptr;
                }
                case STATE_LIST: {
                    auto &list = _GetList();
                    return (index < list.size()) ? list[index].get() : nullptr;
                }
                default: {
                    return nullptr;
                }
            }
        }

        /**
         * @brief  获取指定索引处的可调用对象
         * @return 如果索引有效则返回对应的可调用对象，否则返回nullptr
         */
        TCallable *operator[](size_t index) const noexcept
        {
            return GetAt(index);
        }

    private:
        /**
         * @brief 内部函数，当状态为STATE_SINGLE时返回单个可调用对象的引用，
         */
        constexpr TSinglePtr &_GetSingle() const noexcept
        {
            return *reinterpret_cast<TSinglePtr *>(_data._single);
        }

        /**
         * @brief 内部函数，当状态为STATE_LIST时返回可调用对象列表的引用
         */
        constexpr TSharedList &_GetList() const noexcept
        {
            return *reinterpret_cast<TSharedList *>(_data._list);
        }

        /**
         * @brief 重置当前状态，释放存储的可调用对象
         */
        void _Reset() noexcept
        {
            switch (_state) {
                case STATE_SINGLE: {
                    _GetSingle().~TSinglePtr();
                    _state = STATE_NONE;
                    break;
                }
                case STATE_LIST: {
                    _GetList().~TSharedList();
                    _state = STATE_NONE;
                    break;
                }
            }
        }

        /**
         * @brief 重置当前状态并根据给定状态进行初始化
         */
        void _Reset(uint8_t state) noexcept
        {
            _Reset();

            switch (state) {
                case STATE_SINGLE: {
                    new (_data._single) TSinglePtr();
                    _state = STATE_SINGLE;
                    break;
                }
                case STATE_LIST: {
                    new (_data._list) TSharedList();
                    _state = STATE_LIST;
                    break;
                }
            }
        }
    };

    /**
     * @brief 委托类，类似于C#中的委托，支持存储和调用任意可调用对象
     */
    template <typename TRet, typename... Args>
    class Delegate<TRet(Args...)> final : public ICallable<TRet(Args...)>
    {
    private:
        using _ICallable = ICallable<TRet(Args...)>;

        template <typename T, typename = void>
        struct _IsEqualityComparable : std::false_type {
        };

        template <typename T>
        struct _IsEqualityComparable<
            T,
            typename std::enable_if<true, decltype(void(std::declval<T>() == std::declval<T>()))>::type> : std::true_type {
        };

        template <typename T, typename = void>
        struct _IsMemcmpSafe : std::false_type {
        };

        template <typename T>
        struct _IsMemcmpSafe<
            T,
            typename std::enable_if</*std::is_trivial<T>::value &&*/ std::is_standard_layout<T>::value, void>::type> : std::true_type {
        };

        template <typename T>
        class _CallableWrapperImpl : public _ICallable
        {
            alignas(T) mutable uint8_t _storage[sizeof(T)];

        public:
            _CallableWrapperImpl(const T &value)
            {
                memset(_storage, 0, sizeof(_storage));
                new (_storage) T(value);
            }
            _CallableWrapperImpl(T &&value)
            {
                memset(_storage, 0, sizeof(_storage));
                new (_storage) T(std::move(value));
            }
            virtual ~_CallableWrapperImpl()
            {
                GetValue().~T();
                // memset(_storage, 0, sizeof(_storage));
            }
            T &GetValue() const noexcept
            {
                return *reinterpret_cast<T *>(_storage);
            }
            TRet Invoke(Args... args) const override
            {
                return GetValue()(std::forward<Args>(args)...);
            }
            _ICallable *Clone() const override
            {
                return new _CallableWrapperImpl(GetValue());
            }
            const std::type_info &GetTypeInfo() const override
            {
                return typeid(T);
            }
            bool Equals(const _ICallable &other) const override
            {
                return EqualsImpl(other);
            }
            template <typename U = T>
            typename std::enable_if<_IsEqualityComparable<U>::value, bool>::type
            EqualsImpl(const _ICallable &other) const
            {
                if (this == &other) {
                    return true;
                }
                if (GetTypeInfo() != other.GetTypeInfo()) {
                    return false;
                }
                const auto &otherWrapper = static_cast<const _CallableWrapperImpl &>(other);
                return GetValue() == otherWrapper.GetValue();
            }
            template <typename U = T>
            typename std::enable_if<!_IsEqualityComparable<U>::value && _IsMemcmpSafe<U>::value, bool>::type
            EqualsImpl(const _ICallable &other) const
            {
                if (this == &other) {
                    return true;
                }
                if (GetTypeInfo() != other.GetTypeInfo()) {
                    return false;
                }
                const auto &otherWrapper = static_cast<const _CallableWrapperImpl &>(other);
                return memcmp(_storage, otherWrapper._storage, sizeof(_storage)) == 0;
            }
            template <typename U = T>
            typename std::enable_if<!_IsEqualityComparable<U>::value && !_IsMemcmpSafe<U>::value, bool>::type
            EqualsImpl(const _ICallable &other) const
            {
                return this == &other;
            }
        };

        template <typename T>
        using _CallableWrapper = _CallableWrapperImpl<typename std::decay<T>::type>;

        template <typename T>
        class _MemberFuncWrapper : public _ICallable
        {
            T *obj;
            TRet (T::*func)(Args...);

        public:
            _MemberFuncWrapper(T &obj, TRet (T::*func)(Args...))
                : obj(&obj), func(func)
            {
            }
            TRet Invoke(Args... args) const override
            {
                return (obj->*func)(std::forward<Args>(args)...);
            }
            _ICallable *Clone() const override
            {
                return new _MemberFuncWrapper(*obj, func);
            }
            const std::type_info &GetTypeInfo() const override
            {
                return typeid(func);
            }
            bool Equals(const _ICallable &other) const override
            {
                if (this == &other) {
                    return true;
                }
                if (GetTypeInfo() != other.GetTypeInfo()) {
                    return false;
                }
                const auto &otherWrapper = static_cast<const _MemberFuncWrapper &>(other);
                return obj == otherWrapper.obj && func == otherWrapper.func;
            }
        };

        template <typename T>
        class _ConstMemberFuncWrapper : public _ICallable
        {
            const T *obj;
            TRet (T::*func)(Args...) const;

        public:
            _ConstMemberFuncWrapper(const T &obj, TRet (T::*func)(Args...) const)
                : obj(&obj), func(func)
            {
            }
            TRet Invoke(Args... args) const override
            {
                return (obj->*func)(std::forward<Args>(args)...);
            }
            _ICallable *Clone() const override
            {
                return new _ConstMemberFuncWrapper(*obj, func);
            }
            const std::type_info &GetTypeInfo() const override
            {
                return typeid(func);
            }
            bool Equals(const _ICallable &other) const override
            {
                if (this == &other) {
                    return true;
                }
                if (GetTypeInfo() != other.GetTypeInfo()) {
                    return false;
                }
                const auto &otherWrapper = static_cast<const _ConstMemberFuncWrapper &>(other);
                return obj == otherWrapper.obj && func == otherWrapper.func;
            }
        };

    private:
        /**
         * @brief 内部存储可调用对象的容器
         */
        CallableList<TRet(Args...)> _data;

    public:
        /**
         * @brief 默认构造函数
         */
        Delegate(std::nullptr_t = nullptr)
        {
        }

        /**
         * @brief 构造函数，接受一个可调用对象
         */
        Delegate(const ICallable<TRet(Args...)> &callable)
        {
            Add(callable);
        }

        /**
         * @brief 构造函数，接受一个函数指针
         */
        Delegate(TRet (*func)(Args...))
        {
            Add(func);
        }

        /**
         * @brief 构造函数，接受一个可调用对象
         */
        template <typename T, typename std::enable_if<!std::is_base_of<_ICallable, T>::value, int>::type = 0>
        Delegate(const T &callable)
        {
            Add(callable);
        }

        /**
         * @brief 构造函数，接受一个成员函数指针
         */
        template <typename T>
        Delegate(T &obj, TRet (T::*func)(Args...))
        {
            Add(obj, func);
        }

        /**
         * @brief 构造函数，接受一个常量成员函数指针
         */
        template <typename T>
        Delegate(const T &obj, TRet (T::*func)(Args...) const)
        {
            Add(obj, func);
        }

        /**
         * @brief 拷贝构造函数
         */
        Delegate(const Delegate &other)
            : _data(other._data)
        {
        }

        /**
         * @brief 移动构造函数
         */
        Delegate(Delegate &&other)
            : _data(std::move(other._data))
        {
        }

        /**
         * @brief 拷贝赋值运算符
         */
        Delegate &operator=(const Delegate &other)
        {
            if (this != &other) {
                _data = other._data;
            }
            return *this;
        }

        /**
         * @brief 移动赋值运算符
         */
        Delegate &operator=(Delegate &&other)
        {
            if (this != &other) {
                _data = std::move(other._data);
            }
            return *this;
        }

        /**
         * @brief 添加一个可调用对象到委托中
         */
        void Add(const ICallable<TRet(Args...)> &callable)
        {
            // 当添加的可调用对象与当前委托类型相同时（针对单播委托进行优化）：
            // - 若委托内容为空，则直接返回
            // - 若委托内容只有一个元素，则克隆该元素并添加到当前委托中
            // - 否则，直接添加该可调用对象的克隆
            if (callable.GetTypeInfo() == GetTypeInfo()) {
                auto &delegate = static_cast<const Delegate &>(callable);
                if (delegate._data.IsEmpty()) {
                    return;
                } else if (delegate._data.Count() == 1) {
                    _data.Add(delegate._data[0]->Clone());
                    return;
                }
            }
            _data.Add(callable.Clone());
        }

        /**
         * @brief 添加一个函数指针到委托中
         */
        void Add(TRet (*func)(Args...))
        {
            if (func != nullptr) {
                _data.Add(new _CallableWrapper<decltype(func)>(func));
            }
        }

        /**
         * @brief 添加一个可调用对象到委托中
         */
        template <typename T>
        typename std::enable_if<!std::is_base_of<_ICallable, T>::value, void>::type
        Add(const T &callable)
        {
            _data.Add(new _CallableWrapper<T>(callable));
        }

        /**
         * @brief 添加一个成员函数指针到委托中
         */
        template <typename T>
        void Add(T &obj, TRet (T::*func)(Args...))
        {
            _data.Add(new _MemberFuncWrapper<T>(obj, func));
        }

        /**
         * @brief 添加一个常量成员函数指针到委托中
         */
        template <typename T>
        void Add(const T &obj, TRet (T::*func)(Args...) const)
        {
            _data.Add(new _ConstMemberFuncWrapper<T>(obj, func));
        }

        /**
         * @brief 清空委托中的所有可调用对象
         */
        void Clear()
        {
            _data.Clear();
        }

        /**
         * @brief  移除一个可调用对象
         * @return 如果成功移除则返回true，否则返回false
         * @note   按照添加顺序从后向前查找，找到第一个匹配的可调用对象并移除
         */
        bool Remove(const ICallable<TRet(Args...)> &callable)
        {
            // 当移除的可调用对象与当前委托类型相同时（与Add逻辑相对应）：
            // - 若委托内容为空，则直接返回false
            // - 若委托内容只有一个元素，则尝试移除该元素
            // - 否则，直接调用_Remove函数进行移除
            if (callable.GetTypeInfo() == GetTypeInfo()) {
                auto &delegate = static_cast<const Delegate &>(callable);
                if (delegate._data.IsEmpty()) {
                    return false;
                } else if (delegate._data.Count() == 1) {
                    return _Remove(*delegate._data[0]);
                }
            }
            return _Remove(callable);
        }

        /**
         * @brief  移除一个函数指针
         * @return 如果成功移除则返回true，否则返回false
         * @note   按照添加顺序从后向前查找，找到第一个匹配的函数指针并移除
         */
        bool Remove(TRet (*func)(Args...))
        {
            if (func == nullptr) {
                return false;
            }
            return _Remove(_CallableWrapper<decltype(func)>(func));
        }

        /**
         * @brief  移除一个可调用对象
         * @return 如果成功移除则返回true，否则返回false
         * @note   按照添加顺序从后向前查找，找到第一个匹配的可调用对象并移除
         */
        template <typename T>
        typename std::enable_if<!std::is_base_of<_ICallable, T>::value, bool>::type
        Remove(const T &callable)
        {
            return _Remove(_CallableWrapper<T>(callable));
        }

        /**
         * @brief  移除一个成员函数指针
         * @return 如果成功移除则返回true，否则返回false
         * @note   按照添加顺序从后向前查找，找到第一个匹配的可调用对象并移除
         */
        template <typename T>
        bool Remove(T &obj, TRet (T::*func)(Args...))
        {
            return _Remove(_MemberFuncWrapper<T>(obj, func));
        }

        /**
         * @brief  移除一个常量成员函数指针
         * @return 如果成功移除则返回true，否则返回false
         * @note   按照添加顺序从后向前查找，找到第一个匹配的可调用对象并移除
         */
        template <typename T>
        bool Remove(const T &obj, TRet (T::*func)(Args...) const)
        {
            return _Remove(_ConstMemberFuncWrapper<T>(obj, func));
        }

        /**
         * @brief      调用委托，执行所有存储的可调用对象
         * @param args 函数参数
         * @return     最后一个可调用对象的返回值
         * @throw      std::runtime_error 如果委托为空
         */
        TRet operator()(Args... args) const
        {
            return Invoke(std::forward<Args>(args)...);
        }

        /**
         * @brief       判断当前委托是否等于另一个委托
         * @param other 另一个委托
         * @return      如果相等则返回true，否则返回false
         */
        bool operator==(const Delegate &other) const
        {
            return Equals(other);
        }

        /**
         * @brief       判断当前委托是否不等于另一个委托
         * @param other 另一个委托
         * @return      如果不相等则返回true，否则返回false
         */
        bool operator!=(const Delegate &other) const
        {
            return !Equals(other);
        }

        /**
         * @brief  判断当前委托是否等于nullptr
         * @return 如果委托为空则返回true，否则返回false
         */
        bool operator==(std::nullptr_t) const
        {
            return _data.IsEmpty();
        }

        /**
         * @brief  判断当前委托是否不等于nullptr
         * @return 如果委托不为空则返回true，否则返回false
         */
        bool operator!=(std::nullptr_t) const
        {
            return !_data.IsEmpty();
        }

        /**
         * @brief  判断当前委托是否有效
         * @return 如果委托不为空则返回true，否则返回false
         */
        operator bool() const
        {
            return !_data.IsEmpty();
        }

        /**
         * @brief 添加一个可调用对象到委托中
         * @note  该函数调用Add函数
         */
        Delegate &operator+=(const ICallable<TRet(Args...)> &callable)
        {
            Add(callable);
            return *this;
        }

        /**
         * @brief 添加一个函数指针到委托中
         * @note  该函数调用Add函数
         */
        Delegate &operator+=(TRet (*func)(Args...))
        {
            Add(func);
            return *this;
        }

        /**
         * @brief 添加一个可调用对象到委托中
         * @note  该函数调用Add函数
         */
        template <typename T>
        typename std::enable_if<!std::is_base_of<_ICallable, T>::value, Delegate &>::type
        operator+=(const T &callable)
        {
            Add(callable);
            return *this;
        }

        /**
         * @brief 移除一个可调用对象
         * @note  该函数调用Remove函数
         */
        Delegate &operator-=(const ICallable<TRet(Args...)> &callable)
        {
            Remove(callable);
            return *this;
        }

        /**
         * @brief 移除一个函数指针
         * @note  该函数调用Remove函数
         */
        Delegate &operator-=(TRet (*func)(Args...))
        {
            Remove(func);
            return *this;
        }

        /**
         * @brief 移除一个可调用对象
         * @note  该函数调用Remove函数
         */
        template <typename T>
        typename std::enable_if<!std::is_base_of<_ICallable, T>::value, Delegate &>::type
        operator-=(const T &callable)
        {
            Remove(callable);
            return *this;
        }

        /**
         * @brief      调用委托，执行所有存储的可调用对象
         * @param args 函数参数
         * @return     最后一个可调用对象的返回值
         * @throw      std::runtime_error 如果委托为空
         */
        virtual TRet Invoke(Args... args) const override
        {
            size_t count = _data.Count();
            if (count == 0) {
                throw std::runtime_error("Delegate is empty");
            } else if (count == 1) {
                return _data[0]->Invoke(std::forward<Args>(args)...);
            } else {
                auto list = _data;
                for (size_t i = 0; i < count - 1; ++i)
                    list[i]->Invoke(std::forward<Args>(args)...);
                return list[count - 1]->Invoke(std::forward<Args>(args)...);
            }
        }

        /**
         * @brief  克隆当前委托
         * @return 返回一个新的Delegate对象，包含相同的可调用对象
         */
        virtual ICallable<TRet(Args...)> *Clone() const override
        {
            auto delegate = new Delegate();
            for (size_t i = 0; i < _data.Count(); ++i)
                delegate->_data.Add(_data[i]->Clone());
            return delegate;
        }

        /**
         * @brief  获取当前委托的类型信息
         * @return 返回typeid(Delegate<TRet(Args...)>)
         */
        virtual const std::type_info &GetTypeInfo() const override
        {
            return typeid(Delegate<TRet(Args...)>);
        }

        /**
         * @brief       判断当前委托是否与另一个可调用对象相等
         * @param other 另一个可调用对象
         * @return      如果相等则返回true，否则返回false
         */
        virtual bool Equals(const ICallable<TRet(Args...)> &other) const override
        {
            if (this == &other) {
                return true;
            }
            if (GetTypeInfo() != other.GetTypeInfo()) {
                return false;
            }
            const auto &otherDelegate = static_cast<const Delegate &>(other);
            if (_data.Count() != otherDelegate._data.Count()) {
                return false;
            }
            for (size_t i = _data.Count(); i > 0; --i) {
                if (!_data[i - 1]->Equals(*otherDelegate._data[i - 1])) {
                    return false;
                }
            }
            return true;
        }

        /**
         * @brief      调用所有存储的可调用对象，并返回它们的结果
         * @param args 函数参数
         * @return     返回一个包含所有可调用对象返回值的vector
         */
        template <typename U = TRet>
        typename std::enable_if<!std::is_void<U>::value, std::vector<U>>::type
        InvokeAll(Args... args) const
        {
            auto list  = _data;
            auto count = list.Count();
            std::vector<U> results;
            results.reserve(count);
            for (size_t i = 0; i < count; ++i)
                results.emplace_back(list[i]->Invoke(std::forward<Args>(args)...));
            return results;
        }

    private:
        /**
         * @brief 内部函数，用于从后向前查找并移除一个可调用对象
         */
        bool _Remove(const _ICallable &callable)
        {
            for (size_t i = _data.Count(); i > 0; --i) {
                if (_data[i - 1]->Equals(callable)) {
                    return _data.RemoveAt(i - 1);
                }
            }
            return false;
        }
    };

    /**
     * @brief 比较委托和nullptr
     * @note  如果委托为空则返回true，否则返回false
     */
    template <typename TRet, typename... Args>
    inline bool operator==(std::nullptr_t, const Delegate<TRet(Args...)> &d)
    {
        return d == nullptr;
    }

    /**
     * @brief 比较委托和nullptr
     * @note  如果委托不为空则返回true，否则返回false
     */
    template <typename TRet, typename... Args>
    inline bool operator!=(std::nullptr_t, const Delegate<TRet(Args...)> &d)
    {
        return d != nullptr;
    }

    /**
     * @brief Func类型别名，与Delegate<T>等价
     */
    template <typename T>
    using Func = Delegate<T>;

    /**
     * @brief Action类型别名，表示无返回值的委托
     */
    template <typename... Args>
    using Action = Delegate<void(Args...)>;
}
