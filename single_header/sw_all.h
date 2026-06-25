/**
 * @file    sw_all.h
 * @brief   SimpleWindow单文件版本（合并头文件）
 * @details 本文件由 single_header/build.py 脚本根据 sw/inc 目录下的全部头文件
 *          按依赖关系进行拓扑排序后自动合并生成，包含SimpleWindow框架对外公开
 *          的所有类型、控件、宏与工具函数声明，可作为单头文件分发版本使用。
 * @note    该文件为自动生成产物，请勿手动修改；如需变更内容，请编辑 sw/inc 下
 *          对应的源头文件并重新运行 single_header/build.py 重新生成。
 * @see     https://github.com/Mzying2001/sw
 */

#pragma once
#include <windows.h>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <commctrl.h>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <initializer_list>
#include <limits>
#include <map>
#include <memory>
#include <shellapi.h>
#include <shlobj.h>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <type_traits>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <vector>
#include <windowsx.h>

// Alignment.h

namespace sw
{
    /**
     * @brief 水平对齐方式
     */
    enum class HorizontalAlignment {
        Center,  ///< 中心
        Stretch, ///< 拉伸
        Left,    ///< 左对齐
        Right,   ///< 右对齐
    };

    /**
     * @brief 垂直对齐方式
     */
    enum class VerticalAlignment {
        Center,  ///< 中心
        Stretch, ///< 拉伸
        Top,     ///< 顶部对齐
        Bottom,  ///< 底部对齐
    };

    /**
     * @brief 排列方式
     */
    enum class Orientation {
        Horizontal, ///< 水平排列
        Vertical,   ///< 垂直排列
    };
}

// Cursor.h


namespace sw
{
    /**
     * @brief 系统标准鼠标样式
     * @brief https://learn.microsoft.com/en-us/windows/win32/menurc/about-cursors
     */
    enum class StandardCursor {
        Arrow       = 32512, ///< Normal select
        IBeam       = 32513, ///< Text select
        Wait        = 32514, ///< Busy
        Cross       = 32515, ///< Precision select
        UpArrow     = 32516, ///< Alternate select
        Handwriting = 32631, ///< Handwriting
        SizeNWSE    = 32642, ///< Diagonal resize 1
        SizeNESW    = 32643, ///< Diagonal resize 2
        SizeWE      = 32644, ///< Horizontal resize
        SizeNS      = 32645, ///< Vertical resize
        SizeAll     = 32646, ///< Move
        No          = 32648, ///< Unavailable
        Hand        = 32649, ///< Link select
        AppStarting = 32650, ///< Working in background
        Help        = 32651, ///< Help select
        Pin         = 32671, ///< Location select
        Person      = 32672, ///< Person select
    };

    /**
     * @brief 用于获取鼠标句柄的工具类
     */
    class CursorHelper
    {
    private:
        /**
         * @brief 静态类，禁止实例化
         */
        CursorHelper() = delete;

    public:
        /**
         * @brief 获取系统标准鼠标样式句柄
         * @param cursor 鼠标样式
         * @return 鼠标句柄
         */
        static HCURSOR GetCursorHandle(StandardCursor cursor);

        /**
         * @brief 从指定模块中获取鼠标句柄
         * @param hInstance DLL或EXE的模块句柄
         * @param resourceId 鼠标的资源序号
         * @return 成功则返回鼠标句柄，否则返回NULL
         */
        static HCURSOR GetCursorHandle(HINSTANCE hInstance, int resourceId);

        /**
         * @brief 从文件加载鼠标句柄
         * @param fileName 鼠标文件路径
         * @return 成功则返回鼠标句柄，否则返回NULL
         */
        static HCURSOR GetCursorHandle(const std::wstring &fileName);
    };
}

// Delegate.h


namespace sw
{
    // ICallable接口声明
    template <typename>
    struct ICallable;

    // Delegate类声明
    template <typename>
    class Delegate;

    /*================================================================================*/

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
         * @brief 调用函数
         * @param args 函数参数
         * @return 函数返回值
         */
        virtual TRet Invoke(Args... args) const = 0;

        /**
         * @brief 克隆当前可调用对象
         */
        virtual ICallable *Clone() const = 0;

        /**
         * @brief 获取当前可调用对象的类型信息
         */
        virtual std::type_index GetType() const = 0;

        /**
         * @brief 判断当前可调用对象是否与另一个可调用对象相等
         * @param other 另一个可调用对象
         * @return 如果相等则返回true，否则返回false
         */
        virtual bool Equals(const ICallable &other) const = 0;
    };

    /*================================================================================*/

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
            STATE_NONE,   ///< 未存储任何可调用对象
            STATE_SINGLE, ///< 储存了一个可调用对象
            STATE_LIST,   ///< 储存了多个可调用对象
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
         * @note 强异常安全：先在本地完成可能抛异常的 Clone / vector 拷贝，
         *       全部成功后再原子地切换 *this 的状态。提交阶段（_Reset(state) 与
         *       unique_ptr/vector 的移动赋值）均为 noexcept，不会导致中间不一致。
         */
        CallableList &operator=(const CallableList &other)
        {
            if (this == &other) {
                return *this;
            }

            switch (other._state) {
                case STATE_NONE: {
                    _Reset();
                    break;
                }
                case STATE_SINGLE: {
                    std::unique_ptr<TCallable> cloned(other._GetSingle()->Clone());
                    _Reset(STATE_SINGLE);
                    _GetSingle() = std::move(cloned);
                    break;
                }
                case STATE_LIST: {
                    TSharedList copied = other._GetList();
                    _Reset(STATE_LIST);
                    _GetList() = std::move(copied);
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
                case STATE_NONE: {
                    break;
                }
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
         * @brief 获取当前存储的可调用对象数量
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
         * @brief 判断当前存储的可调用对象是否为空
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
         * @note 传入对象的生命周期将由CallableList管理
         * @note 异常安全：
         *       - SINGLE→LIST 升级时使用 reserve(2) 避免后续 emplace_back 触发扩容，
         *         并使用 shared_ptr(unique_ptr&&) 接管所有权，构造失败时原 unique_ptr
         *         不会释放其管理的对象。
         *       - STATE_LIST 分支先把裸指针转交给本地 shared_ptr，再 emplace_back，
         *         即使 vector 扩容失败，本地 shared_ptr 析构时也会正确释放对象。
         */
        void Add(TCallable *callable)
        {
            TSinglePtr owned(callable);

            if (owned == nullptr) {
                return;
            }

            switch (_state) {
                case STATE_NONE: {
                    _Reset(STATE_SINGLE);
                    _GetSingle() = std::move(owned);
                    break;
                }
                case STATE_SINGLE: {
                    TSharedList list;
                    list.reserve(2);
                    std::shared_ptr<TCallable> incoming(std::move(owned));
                    std::shared_ptr<TCallable> current(std::move(_GetSingle()));
                    list.emplace_back(std::move(current));
                    list.emplace_back(std::move(incoming));
                    _Reset(STATE_LIST);
                    _GetList() = std::move(list);
                    break;
                }
                case STATE_LIST: {
                    std::shared_ptr<TCallable> sp(std::move(owned));
                    _GetList().emplace_back(std::move(sp));
                    break;
                }
            }
        }

        /**
         * @brief 移除指定索引处的可调用对象
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
                    }
                    // 注：LIST 仅剩 1 元素时不降级回 SINGLE。shared_ptr 无法转移给
                    // unique_ptr，强行 Clone 反而带来额外开销，保留 LIST 单元素状态
                    // 在功能与性能上都可接受。
                    return true;
                }
                default: {
                    return false;
                }
            }
        }

        /**
         * @brief 获取指定索引处的可调用对象
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
         * @brief 获取指定索引处的可调用对象
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
                case STATE_NONE: {
                    break;
                }
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

    /*================================================================================*/

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
            T, decltype(void(std::declval<T>() == std::declval<T>()))> : std::true_type {
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
        class _CallableWrapperImpl final : public _ICallable
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
            virtual std::type_index GetType() const override
            {
                return typeid(T);
            }
            bool Equals(const _ICallable &other) const override
            {
                return EqualsImpl(other);
            }
            template <typename U = T>
            auto EqualsImpl(const _ICallable &other) const
                -> typename std::enable_if<_IsEqualityComparable<U>::value, bool>::type
            {
                if (this == &other) {
                    return true;
                }
                if (GetType() != other.GetType()) {
                    return false;
                }
                const auto &otherWrapper = static_cast<const _CallableWrapperImpl &>(other);
                return GetValue() == otherWrapper.GetValue();
            }
            template <typename U = T>
            auto EqualsImpl(const _ICallable &other) const
                -> typename std::enable_if<!_IsEqualityComparable<U>::value && _IsMemcmpSafe<U>::value, bool>::type
            {
                if (this == &other) {
                    return true;
                }
                if (GetType() != other.GetType()) {
                    return false;
                }
                const auto &otherWrapper = static_cast<const _CallableWrapperImpl &>(other);
                return memcmp(_storage, otherWrapper._storage, sizeof(_storage)) == 0;
            }
            template <typename U = T>
            auto EqualsImpl(const _ICallable &other) const
                -> typename std::enable_if<!_IsEqualityComparable<U>::value && !_IsMemcmpSafe<U>::value, bool>::type
            {
                return this == &other;
            }

        public:
            // 禁用拷贝/移动：默认实现会按字节拷贝 _storage，不会调用 T 的构造函数，
            // 对非平凡可拷贝类型会破坏不变式。需要克隆请走 Clone()。
            _CallableWrapperImpl(const _CallableWrapperImpl &)            = delete;
            _CallableWrapperImpl(_CallableWrapperImpl &&)                 = delete;
            _CallableWrapperImpl &operator=(const _CallableWrapperImpl &) = delete;
            _CallableWrapperImpl &operator=(_CallableWrapperImpl &&)      = delete;
        };

        template <typename T>
        using _CallableWrapper = _CallableWrapperImpl<typename std::decay<T>::type>;

        template <typename T>
        class _MemberFuncWrapper final : public _ICallable
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
            virtual std::type_index GetType() const override
            {
                return typeid(func);
            }
            bool Equals(const _ICallable &other) const override
            {
                if (this == &other) {
                    return true;
                }
                if (GetType() != other.GetType()) {
                    return false;
                }
                const auto &otherWrapper = static_cast<const _MemberFuncWrapper &>(other);
                return obj == otherWrapper.obj && func == otherWrapper.func;
            }

        public:
            // 禁用拷贝/移动：与 _CallableWrapperImpl 保持一致，需要克隆请走 Clone()。
            _MemberFuncWrapper(const _MemberFuncWrapper &)            = delete;
            _MemberFuncWrapper(_MemberFuncWrapper &&)                 = delete;
            _MemberFuncWrapper &operator=(const _MemberFuncWrapper &) = delete;
            _MemberFuncWrapper &operator=(_MemberFuncWrapper &&)      = delete;
        };

        template <typename T>
        class _ConstMemberFuncWrapper final : public _ICallable
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
            virtual std::type_index GetType() const override
            {
                return typeid(func);
            }
            bool Equals(const _ICallable &other) const override
            {
                if (this == &other) {
                    return true;
                }
                if (GetType() != other.GetType()) {
                    return false;
                }
                const auto &otherWrapper = static_cast<const _ConstMemberFuncWrapper &>(other);
                return obj == otherWrapper.obj && func == otherWrapper.func;
            }

        public:
            // 禁用拷贝/移动：与 _CallableWrapperImpl 保持一致，需要克隆请走 Clone()。
            _ConstMemberFuncWrapper(const _ConstMemberFuncWrapper &)            = delete;
            _ConstMemberFuncWrapper(_ConstMemberFuncWrapper &&)                 = delete;
            _ConstMemberFuncWrapper &operator=(const _ConstMemberFuncWrapper &) = delete;
            _ConstMemberFuncWrapper &operator=(_ConstMemberFuncWrapper &&)      = delete;
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
        {
            for (size_t i = 0; i < other._data.Count(); ++i) {
                _data.Add(other._data[i]->Clone());
            }
        }

        /**
         * @brief 移动构造函数
         */
        Delegate(Delegate &&other) noexcept
            : _data(std::move(other._data))
        {
        }

        /**
         * @brief 拷贝赋值运算符
         */
        Delegate &operator=(const Delegate &other)
        {
            if (this == &other) {
                return *this;
            }

            CallableList<TRet(Args...)> copied;

            for (size_t i = 0; i < other._data.Count(); ++i) {
                copied.Add(other._data[i]->Clone());
            }
            _data = std::move(copied);
            return *this;
        }

        /**
         * @brief 移动赋值运算符
         */
        Delegate &operator=(Delegate &&other) noexcept
        {
            if (this != &other) {
                _data = std::move(other._data);
            }
            return *this;
        }

        /**
         * @brief 添加一个可调用对象到委托中
         * @note 当传入对象是同类型 Delegate 时：
         *       - 内部为空：直接返回；
         *       - 内部恰好 1 个元素：展开添加该元素的克隆（与单播添加等价）；
         *       - 内部 ≥2 个元素：作为整体嵌套加入，不展开。
         *       后一种"不展开"是有意为之，目的是保证 += 与 -= 的对称性：
         *       后续 `*this -= callable` 仍可按整体匹配并撤销本次添加。
         *       因此 `b += a; b == a` 在 a 含 ≥2 元素时为 false（Count 不同），
         *       但 `b += a; b -= a` 后 b 与添加前等价。
         */
        void Add(const ICallable<TRet(Args...)> &callable)
        {
            if (callable.GetType() == GetType()) {
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
        auto Add(const T &callable)
            -> typename std::enable_if<!std::is_base_of<_ICallable, T>::value, void>::type
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
         * @brief 移除一个可调用对象
         * @return 如果成功移除则返回true，否则返回false
         * @note 按照添加顺序从后向前查找，找到第一个匹配的可调用对象并移除
         * @note 与 Add 逻辑严格对称——当传入对象是同类型 Delegate 时：
         *       - 内部为空：返回 false；
         *       - 内部恰好 1 个元素：尝试匹配并移除该元素本身；
         *       - 内部 ≥2 个元素：按整体（嵌套 Delegate）匹配并移除，
         *         恰好对应 Add 时"不展开整体加入"的行为。
         */
        bool Remove(const ICallable<TRet(Args...)> &callable)
        {
            if (callable.GetType() == GetType()) {
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
         * @brief 移除一个函数指针
         * @return 如果成功移除则返回true，否则返回false
         * @note 按照添加顺序从后向前查找，找到第一个匹配的函数指针并移除
         */
        bool Remove(TRet (*func)(Args...))
        {
            if (func == nullptr) {
                return false;
            }
            return _Remove(_CallableWrapper<decltype(func)>(func));
        }

        /**
         * @brief 移除一个可调用对象
         * @return 如果成功移除则返回true，否则返回false
         * @note 按照添加顺序从后向前查找，找到第一个匹配的可调用对象并移除
         */
        template <typename T>
        auto Remove(const T &callable)
            -> typename std::enable_if<!std::is_base_of<_ICallable, T>::value, bool>::type
        {
            return _Remove(_CallableWrapper<T>(callable));
        }

        /**
         * @brief 移除一个成员函数指针
         * @return 如果成功移除则返回true，否则返回false
         * @note 按照添加顺序从后向前查找，找到第一个匹配的可调用对象并移除
         */
        template <typename T>
        bool Remove(T &obj, TRet (T::*func)(Args...))
        {
            return _Remove(_MemberFuncWrapper<T>(obj, func));
        }

        /**
         * @brief 移除一个常量成员函数指针
         * @return 如果成功移除则返回true，否则返回false
         * @note 按照添加顺序从后向前查找，找到第一个匹配的可调用对象并移除
         */
        template <typename T>
        bool Remove(const T &obj, TRet (T::*func)(Args...) const)
        {
            return _Remove(_ConstMemberFuncWrapper<T>(obj, func));
        }

        /**
         * @brief 调用委托，执行所有存储的可调用对象
         * @param args 函数参数
         * @return 最后一个可调用对象的返回值
         * @throw std::runtime_error 如果委托为空
         */
        TRet operator()(Args... args) const
        {
            return _InvokeImpl(std::forward<Args>(args)...);
        }

        /**
         * @brief 判断当前委托是否等于另一个委托
         * @param other 另一个委托
         * @return 如果相等则返回true，否则返回false
         */
        bool operator==(const Delegate &other) const
        {
            return Equals(other);
        }

        /**
         * @brief 判断当前委托是否不等于另一个委托
         * @param other 另一个委托
         * @return 如果不相等则返回true，否则返回false
         */
        bool operator!=(const Delegate &other) const
        {
            return !Equals(other);
        }

        /**
         * @brief 判断当前委托是否等于nullptr
         * @return 如果委托为空则返回true，否则返回false
         */
        bool operator==(std::nullptr_t) const noexcept
        {
            return _data.IsEmpty();
        }

        /**
         * @brief 判断当前委托是否不等于nullptr
         * @return 如果委托不为空则返回true，否则返回false
         */
        bool operator!=(std::nullptr_t) const noexcept
        {
            return !_data.IsEmpty();
        }

        /**
         * @brief 判断当前委托是否有效
         * @return 如果委托不为空则返回true，否则返回false
         */
        explicit operator bool() const noexcept
        {
            return !_data.IsEmpty();
        }

        /**
         * @brief 添加一个可调用对象到委托中
         * @note 该函数调用Add函数
         */
        Delegate &operator+=(const ICallable<TRet(Args...)> &callable)
        {
            Add(callable);
            return *this;
        }

        /**
         * @brief 添加一个函数指针到委托中
         * @note 该函数调用Add函数
         */
        Delegate &operator+=(TRet (*func)(Args...))
        {
            Add(func);
            return *this;
        }

        /**
         * @brief 添加一个可调用对象到委托中
         * @note 该函数调用Add函数
         */
        template <typename T>
        auto operator+=(const T &callable)
            -> typename std::enable_if<!std::is_base_of<_ICallable, T>::value, Delegate &>::type
        {
            Add(callable);
            return *this;
        }

        /**
         * @brief 移除一个可调用对象
         * @note 该函数调用Remove函数
         */
        Delegate &operator-=(const ICallable<TRet(Args...)> &callable)
        {
            Remove(callable);
            return *this;
        }

        /**
         * @brief 移除一个函数指针
         * @note 该函数调用Remove函数
         */
        Delegate &operator-=(TRet (*func)(Args...))
        {
            Remove(func);
            return *this;
        }

        /**
         * @brief 移除一个可调用对象
         * @note 该函数调用Remove函数
         */
        template <typename T>
        auto operator-=(const T &callable)
            -> typename std::enable_if<!std::is_base_of<_ICallable, T>::value, Delegate &>::type
        {
            Remove(callable);
            return *this;
        }

        /**
         * @brief 调用委托，执行所有存储的可调用对象
         * @param args 函数参数
         * @return 最后一个可调用对象的返回值
         * @throw std::runtime_error 如果委托为空
         */
        virtual TRet Invoke(Args... args) const override
        {
            return _InvokeImpl(std::forward<Args>(args)...);
        }

        /**
         * @brief 克隆当前委托
         * @return 返回一个新的Delegate对象，包含相同的可调用对象
         */
        virtual ICallable<TRet(Args...)> *Clone() const override
        {
            return new Delegate(*this);
        }

        /**
         * @brief 获取当前委托的类型信息
         * @return 返回typeid(Delegate<TRet(Args...)>)
         */
        virtual std::type_index GetType() const override
        {
            return typeid(Delegate<TRet(Args...)>);
        }

        /**
         * @brief 判断当前委托是否与另一个可调用对象相等
         * @param other 另一个可调用对象
         * @return 如果相等则返回true，否则返回false
         */
        virtual bool Equals(const ICallable<TRet(Args...)> &other) const override
        {
            if (this == &other) {
                return true;
            }
            if (GetType() != other.GetType()) {
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
         * @brief 调用所有存储的可调用对象，并返回它们的结果
         * @param args 函数参数
         * @return 返回一个包含所有可调用对象返回值的vector
         * @note 多播调用时，前 N-1 次按左值传参，仅最后一次执行 std::forward，
         *       避免对 move-only 类型或右值引用形参反复 move 同一对象。
         */
        template <typename U = TRet>
        auto InvokeAll(Args... args) const
            -> typename std::enable_if<!std::is_void<U>::value, std::vector<U>>::type
        {
            std::vector<U> results;
            size_t count = _data.Count();
            if (count == 0) {
                _ThrowEmptyDelegateError();
            } else if (count == 1) {
                results.emplace_back(_data[0]->Invoke(std::forward<Args>(args)...));
            } else {
                auto list = _data;
                results.reserve(count);
                for (size_t i = 0; i + 1 < count; ++i) {
                    results.emplace_back(list[i]->Invoke(args...));
                }
                results.emplace_back(list[count - 1]->Invoke(std::forward<Args>(args)...));
            }
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

        /**
         * @brief 内部函数，调用空委托时抛出异常
         */
        [[noreturn]] void _ThrowEmptyDelegateError() const
        {
            throw std::runtime_error("Delegate is empty");
        }

        /**
         * @brief 内部函数，Invoke和operator()的实现
         * @note 多播调用时，前 N-1 次按左值传参，仅最后一次执行 std::forward，
         *       避免对 move-only 类型或右值引用形参反复 move 同一对象。
         */
        inline TRet _InvokeImpl(Args... args) const
        {
            size_t count = _data.Count();
            if (count == 0) {
                _ThrowEmptyDelegateError();
            } else if (count == 1) {
                return _data[0]->Invoke(std::forward<Args>(args)...);
            } else {
                auto list = _data;
                for (size_t i = 0; i + 1 < count; ++i)
                    list[i]->Invoke(args...);
                return list[count - 1]->Invoke(std::forward<Args>(args)...);
            }
        }
    };

    /*================================================================================*/

    /**
     * @brief 比较委托和nullptr
     * @note 如果委托为空则返回true，否则返回false
     */
    template <typename TRet, typename... Args>
    inline bool operator==(std::nullptr_t, const Delegate<TRet(Args...)> &d) noexcept
    {
        return d == nullptr;
    }

    /**
     * @brief 比较委托和nullptr
     * @note 如果委托不为空则返回true，否则返回false
     */
    template <typename TRet, typename... Args>
    inline bool operator!=(std::nullptr_t, const Delegate<TRet(Args...)> &d) noexcept
    {
        return d != nullptr;
    }

    /*================================================================================*/

    /**
     * @brief Action类型别名，表示无返回值的委托
     */
    template <typename... Args>
    using Action = Delegate<void(Args...)>;

    /**
     * @brief Predicate类型别名，表示返回bool的单参数委托
     */
    template <typename T>
    using Predicate = Delegate<bool(T)>;

    /*================================================================================*/

    /**
     * @brief _FuncTraits模板，用于提取函数类型的返回值和参数类型
     */
    template <typename...>
    struct _FuncTraits;

    /**
     * @brief _FuncTraits特化
     */
    template <typename Last>
    struct _FuncTraits<Last> {
        using TRet       = Last;
        using TArgsTuple = std::tuple<>;
    };

    /**
     * @brief _FuncTraits特化
     */
    template <typename First, typename... Rest>
    struct _FuncTraits<First, Rest...> {
        using TRet       = typename _FuncTraits<Rest...>::TRet;
        using TArgsTuple = decltype(std::tuple_cat(std::declval<std::tuple<First>>(), std::declval<typename _FuncTraits<Rest...>::TArgsTuple>()));
    };

    /**
     * @brief _FuncTypeHelper模板，用于根据参数元组生成对应的Func类型
     */
    template <typename TArgsTuple>
    struct _FuncTypeHelper;

    /**
     * @brief _FuncTypeHelper特化
     */
    template <typename... Args>
    struct _FuncTypeHelper<std::tuple<Args...>> {
        template <typename TRet>
        using TFunc = Delegate<TRet(Args...)>;
    };

    /**
     * @brief Func类型别名，类似C#中的Func<T1, T2, ..., TResult>
     */
    template <typename... Types>
    using Func = typename _FuncTypeHelper<typename _FuncTraits<Types...>::TArgsTuple>::template TFunc<typename _FuncTraits<Types...>::TRet>;
}

// EnumBit.h


/**
 * @brief 为指定枚举类型启用按位运算符
 * @param T 需要启用按位运算的枚举类型
 * @note 该宏应在sw命名空间内、枚举类型声明之后使用。
 */
#define _SW_ENUM_ENABLE_BIT_OPERATIONS(T)                  \
    template <>                                            \
    struct _EnumSupportBitOperations<T> : std::true_type { \
    }

namespace sw
{
    /**
     * @brief 用于标记枚举是否支持位运算
     * @tparam T 要检测的类型
     * @note 默认不支持位运算；通过_SW_ENUM_ENABLE_BIT_OPERATIONS特化后才会启用相关运算符。
     */
    template <typename T>
    struct _EnumSupportBitOperations : std::false_type {
    };

    /**
     * @brief 为标记_EnumSupportBitOperations的枚举类型提供按位或运算
     * @tparam T 已启用位运算的枚举类型
     * @param a 左操作数
     * @param b 右操作数
     * @return 按位或后的枚举值
     */
    template <typename T>
    constexpr auto operator|(T a, T b)
        -> typename std::enable_if<std::is_enum<T>::value && _EnumSupportBitOperations<T>::value, T>::type
    {
        using TUnderlying = typename std::underlying_type<T>::type;
        return static_cast<T>(static_cast<TUnderlying>(a) | static_cast<TUnderlying>(b));
    }

    /**
     * @brief 为标记_EnumSupportBitOperations的枚举类型提供按位或赋值运算
     * @tparam T 已启用位运算的枚举类型
     * @param a 左操作数引用，运算结果会写回该值
     * @param b 右操作数
     * @return 写回后的左操作数引用
     */
    template <typename T>
    constexpr auto operator|=(T &a, T b)
        -> typename std::enable_if<std::is_enum<T>::value && _EnumSupportBitOperations<T>::value, T &>::type
    {
        return a = a | b;
    }

    /**
     * @brief 为标记_EnumSupportBitOperations的枚举类型提供按位与运算
     * @tparam T 已启用位运算的枚举类型
     * @param a 左操作数
     * @param b 右操作数
     * @return 按位与后的枚举值
     */
    template <typename T>
    constexpr auto operator&(T a, T b)
        -> typename std::enable_if<std::is_enum<T>::value && _EnumSupportBitOperations<T>::value, T>::type
    {
        using TUnderlying = typename std::underlying_type<T>::type;
        return static_cast<T>(static_cast<TUnderlying>(a) & static_cast<TUnderlying>(b));
    }

    /**
     * @brief 为标记_EnumSupportBitOperations的枚举类型提供按位与赋值运算
     * @tparam T 已启用位运算的枚举类型
     * @param a 左操作数引用，运算结果会写回该值
     * @param b 右操作数
     * @return 写回后的左操作数引用
     */
    template <typename T>
    constexpr auto operator&=(T &a, T b)
        -> typename std::enable_if<std::is_enum<T>::value && _EnumSupportBitOperations<T>::value, T &>::type
    {
        return a = a & b;
    }

    /**
     * @brief 为标记_EnumSupportBitOperations的枚举类型提供按位异或运算
     * @tparam T 已启用位运算的枚举类型
     * @param a 左操作数
     * @param b 右操作数
     * @return 按位异或后的枚举值
     */
    template <typename T>
    constexpr auto operator^(T a, T b)
        -> typename std::enable_if<std::is_enum<T>::value && _EnumSupportBitOperations<T>::value, T>::type
    {
        using TUnderlying = typename std::underlying_type<T>::type;
        return static_cast<T>(static_cast<TUnderlying>(a) ^ static_cast<TUnderlying>(b));
    }

    /**
     * @brief 为标记_EnumSupportBitOperations的枚举类型提供按位异或赋值运算
     * @tparam T 已启用位运算的枚举类型
     * @param a 左操作数引用，运算结果会写回该值
     * @param b 右操作数
     * @return 写回后的左操作数引用
     */
    template <typename T>
    constexpr auto operator^=(T &a, T b)
        -> typename std::enable_if<std::is_enum<T>::value && _EnumSupportBitOperations<T>::value, T &>::type
    {
        return a = a ^ b;
    }

    /**
     * @brief 为标记_EnumSupportBitOperations的枚举类型提供按位取反运算
     * @tparam T 已启用位运算的枚举类型
     * @param a 操作数
     * @return 按位取反后的枚举值
     */
    template <typename T>
    constexpr auto operator~(T a)
        -> typename std::enable_if<std::is_enum<T>::value && _EnumSupportBitOperations<T>::value, T>::type
    {
        using TUnderlying = typename std::underlying_type<T>::type;
        return static_cast<T>(~static_cast<TUnderlying>(a));
    }
}

// Font.h


namespace sw
{
    /**
     * @brief 字体的粗细
     */
    enum class FontWeight : int {
        DontCare   = 0,   ///< 不指定（由字体映射器选择默认粗细）
        Thin       = 100, ///< 极细
        ExtraLight = 200, ///< 特细，同 UltraLight
        UltraLight = 200, ///< 超细，同 ExtraLight
        Light      = 300, ///< 细
        Normal     = 400, ///< 正常（常规粗细），同 Regular
        Regular    = 400, ///< 常规，同 Normal
        Medium     = 500, ///< 中等
        SemiBold   = 600, ///< 半粗，同 DemiBold
        DemiBold   = 600, ///< 半粗，同 SemiBold
        Bold       = 700, ///< 粗体
        ExtraBold  = 800, ///< 特粗，同 UltraBold
        UltraBold  = 800, ///< 超粗，同 ExtraBold
        Heavy      = 900, ///< 重磅，同 Black
        Black      = 900, ///< 黑体，同 Heavy
    };

    /**
     * @brief 字符集
     */
    enum class FontCharSet : uint8_t {
        ANSI        = ANSI_CHARSET,        ///< ANSI 字符集（Windows-1252，西欧）
        Baltic      = BALTIC_CHARSET,      ///< 波罗的海字符集
        CheneseBig5 = CHINESEBIG5_CHARSET, ///< 繁体中文（Big5）字符集
        Default     = DEFAULT_CHARSET,     ///< 默认字符集（随系统区域设置而定）
        EastEurope  = EASTEUROPE_CHARSET,  ///< 东欧字符集
        GB2312      = GB2312_CHARSET,      ///< 简体中文（GB2312）字符集
        Greek       = GREEK_CHARSET,       ///< 希腊语字符集
        Hangul      = HANGUL_CHARSET,      ///< 韩语字符集
        Mac         = MAC_CHARSET,         ///< Mac 字符集
        Oem         = OEM_CHARSET,         ///< OEM 字符集（随系统 OEM 代码页）
        Russian     = RUSSIAN_CHARSET,     ///< 俄语字符集
        ShiftJIS    = SHIFTJIS_CHARSET,    ///< 日语 Shift-JIS 字符集
        Symbol      = SYMBOL_CHARSET,      ///< Symbol 符号字符集
        Turkish     = TURKISH_CHARSET,     ///< 土耳其语字符集
        Vietnamese  = VIETNAMESE_CHARSET,  ///< 越南语字符集

        // Windows 的朝鲜语版本：
        Johab = JOHAB_CHARSET, ///< 韩语 Johab 字符集

        // Windows 中东语言版本：
        Arabic = ARABIC_CHARSET, ///< 阿拉伯语字符集
        Hebrew = HEBREW_CHARSET, ///< 希伯来语字符集

        // Windows 的泰语版本：
        Thai = THAI_CHARSET, ///< 泰语字符集
    };

    /**
     * @brief 输出质量
     */
    enum class FontOutPrecision : uint8_t {
        Character      = OUT_CHARACTER_PRECIS, ///< 未使用。
        Default        = OUT_DEFAULT_PRECIS,   ///< 指定默认字体映射器行为。
        Device         = OUT_DEVICE_PRECIS,    ///< 当系统包含多个同名字体时，指示字体映射器选择设备字体。
        Outline        = OUT_OUTLINE_PRECIS,   ///< 此值指示字体映射器从 TrueType 和其他基于大纲的字体中进行选择。
        PostScriptOnly = OUT_PS_ONLY_PRECIS,   ///< 指示字体映射器仅从 PostScript 字体中进行选择。 如果系统中没有安装 PostScript 字体，字体映射器将返回到默认行为。
        Raster         = OUT_RASTER_PRECIS,    ///< 当系统包含多个同名字体时，指示字体映射器选择光栅字体。
        String         = OUT_STRING_PRECIS,    ///< 字体映射器不使用此值，但在枚举光栅字体时会返回此值。
        Stroke         = OUT_STROKE_PRECIS,    ///< 字体映射器不使用此值，但在枚举 TrueType、其他基于轮廓的字体和矢量字体时返回此值。
        TrueTypeOnly   = OUT_TT_ONLY_PRECIS,   ///< 指示字体映射器仅从 TrueType 字体中进行选择。 如果系统中没有安装 TrueType 字体，字体映射器将返回到默认行为。
        TrueType       = OUT_TT_PRECIS,        ///< 当系统包含多个同名字体时，指示字体映射器选择 TrueType 字体。
    };

    /**
     * @brief 剪裁精度
     */
    enum class FontClipPrecision : uint8_t {
        CharacterPrecis = CLIP_CHARACTER_PRECIS, ///< 未使用。
        DefaultPrecis   = CLIP_DEFAULT_PRECIS,   ///< 指定默认剪辑行为。

#if defined(CLIP_DFA_DISABLE)
        DFA_Disable = CLIP_DFA_DISABLE, ///< Windows XP SP1： 关闭字体的字体关联。 请注意，此标志不保证在 Windows Server 2003 之后对任何平台产生任何影响。
#endif

        Embedded  = CLIP_EMBEDDED,  ///< 必须指定此标志才能使用嵌入的只读字体。
        LH_Angles = CLIP_LH_ANGLES, ///< 使用此值时，所有字体的旋转取决于坐标系的方向是左手还是右手。如果未使用，设备字体始终逆时针旋转，但其他字体的旋转取决于坐标系的方向。
        Mask      = CLIP_MASK,      ///< 未使用。

#if defined(CLIP_DFA_OVERRIDE)
        DFA_Override = CLIP_DFA_OVERRIDE, ///< 关闭字体的字体关联。 这与CLIP_DFA_DISABLE相同，但在某些情况下可能会有问题：建议使用的标志是CLIP_DFA_DISABLE。
#endif

        StrokePrecis = CLIP_STROKE_PRECIS, ///< 字体映射器不使用，但在枚举光栅、矢量或 TrueType 字体时返回。 为了兼容，枚举字体时始终返回此值。
        TT_Always    = CLIP_TT_ALWAYS,     ///< 未使用。
    };

    /**
     * @brief 输出质量
     */
    enum class FontQuality : uint8_t {
        AntiAliased   = ANTIALIASED_QUALITY,    ///< 如果字体支持该字体，并且字体大小不是太小或太大，则字体始终为抗锯齿。
        ClearType     = CLEARTYPE_QUALITY,      ///< 如果设置，则尽可能使用 ClearType 抗锯齿方法呈现文本。
        Default       = DEFAULT_QUALITY,        ///< 字体的外观并不重要。
        Draft         = DRAFT_QUALITY,          ///< 字体的外观不如使用PROOF_QUALITY时重要。 对于 GDI 光栅字体，会启用缩放，这意味着可以使用更多字号，但质量可能较低。 如有必要，将合成粗体、斜体、下划线和删除线字体。
        NoAntiAliased = NONANTIALIASED_QUALITY, ///< 字体永远不会抗锯齿。
        Proof         = PROOF_QUALITY,          ///< 字体的字符质量比逻辑字体属性的精确匹配更重要。 对于 GDI 光栅字体，将禁用缩放，并选择大小最接近的字体。 虽然在使用PROOF_QUALITY时可能无法精确映射所选字号，但字体质量较高，外观不会失真。 如有必要，将合成粗体、斜体、下划线和删除线字体。
    };

    /**
     * @brief 字体的间距
     */
    enum class FontPitch : uint8_t {
        Default  = DEFAULT_PITCH,  ///< 默认间距（由字体映射器选择）
        Fixed    = FIXED_PITCH,    ///< 等宽间距（所有字符宽度相同，如 Courier New）
        Variable = VARIABLE_PITCH, ///< 可变间距（字符宽度随字形变化，如 Arial）
    };

    /**
     * @brief 字体系列
     */
    enum class FontFamily : uint8_t {
        Decorative = FF_DECORATIVE, ///< 新奇字体。 例如 Old English。
        DontCare   = FF_DONTCARE,   ///< 使用默认字体。
        Modern     = FF_MODERN,     ///< 具有固定笔划宽度的字体 (带衬线或无衬线) 的空白字体。 Monospace 字体通常是新式字体。 例如，Pica、Elite 和 CourierNew。
        Roman      = FF_ROMAN,      ///< 笔划宽度可变的字体 (比例) 和衬线。 例如 MS Serif。
        Script     = FF_SCRIPT,     ///< 设计为类似于手写的字体。 例如 Script 和 Cursive。
        Swiss      = FF_SWISS,      ///< 笔划宽度可变的字体 (成比例) 且不带衬线。 例如 MS Sans Serif。
    };

    /**
     * @brief 字体类
     */
    class Font
    {
    public:
        /**
         * @brief 字体的字体名称，此字符串的长度不能超过 32
         */
        std::wstring name = L"";

        /**
         * @brief 字体大小，以dip为单位
         */
        double size = 12;

        /**
         * @brief 转义向量与设备的 x 轴之间的角度（以十分之一度为单位）
         */
        int escapement = 0;

        /**
         * @brief 每个字符的基线和设备 x 轴之间的角度（以十分之一度为单位）
         */
        int orientation = 0;

        /**
         * @brief 字体的粗细，范围为 0 到 1000
         */
        FontWeight weight = FontWeight::DontCare;

        /**
         * @brief 是否为斜体
         */
        bool italic = false;

        /**
         * @brief 是否有下划线
         */
        bool underline = false;

        /**
         * @brief 是否有删除线
         */
        bool strikeOut = false;

        /**
         * @brief 字符集
         */
        FontCharSet charSet = FontCharSet::Default;

        /**
         * @brief 输出精度
         */
        FontOutPrecision outPrecision = FontOutPrecision::Default;

        /**
         * @brief 裁剪精度
         */
        FontClipPrecision clipPrecision = FontClipPrecision::DefaultPrecis;

        /**
         * @brief 输出质量
         */
        FontQuality quality = FontQuality::Default;

        /**
         * @brief 字体的间距和系列
         */
        /*uint8_t pitchAndFamily;*/

        /**
         * @brief 字体的间距
         */
        FontPitch pitch = FontPitch::Default;

        /**
         * @brief 字体系列
         */
        FontFamily family = FontFamily::DontCare;

    public:
        /**
         * @brief 构造字体对象
         */
        Font();

        /**
         * @brief 构造指定字体名称和字符集的字体对象
         */
        Font(const std::wstring &name, FontCharSet charSet);

        /**
         * @brief 构造指定字体名称、大小与粗细的字体对象
         */
        Font(const std::wstring &name, double size = 12, FontWeight weight = FontWeight::DontCare);

        /**
         * @brief 从LOGFONTW构造字体对象
         */
        Font(const LOGFONTW &logFont);

        /**
         * @brief 隐式转换LOGFONTW
         */
        operator LOGFONTW() const;

        /**
         * @brief 创建HFONT句柄
         */
        HFONT CreateHandle() const;

        /**
         * @brief 通过HFONT获取字体信息
         */
        static Font GetFont(HFONT hFont);

        /**
         * @brief 获取默认字体，线程中首次调用或参数update为true时会获取系统默认字体
         * @param update 是否重新获取
         * @return 当前线程控件的默认字体
         * @note 可修改返回的引用来更改默认字体（已创建的控件字体不会因此改变）
         */
        static Font &GetDefaultFont(bool update = false);
    };
}

// HitTestResult.h


namespace sw
{
    /**
     * @brief NcHitTest（WM_NCHITTEST）的返回值
     */
    enum class HitTestResult {
        HitBorder      = HTBORDER,      ///< 在没有大小边界的窗口边框中。
        HitBottom      = HTBOTTOM,      ///< 在可调整大小的窗口的下水平边框中（用户可以单击鼠标以垂直调整窗口大小）。
        HitBottomLeft  = HTBOTTOMLEFT,  ///< 在可调整大小的窗口的边框左下角（用户可以单击鼠标以对角线调整窗口大小）。
        HitBottomRight = HTBOTTOMRIGHT, ///< 在可调整大小的窗口的边框右下角（用户可以单击鼠标以对角线调整窗口大小）。
        HitCaption     = HTCAPTION,     ///< 在标题栏中。
        HitClient      = HTCLIENT,      ///< 在客户端区中。
        HitClose       = HTCLOSE,       ///< 在关闭按钮中。
        HitError       = HTERROR,       ///< 在屏幕背景上或窗口之间的分割线上（与 HTNOWHERE 相同，只是 DefWindowProc 函数会生成系统蜂鸣音以指示错误）。
        HitGrowBox     = HTGROWBOX,     ///< 在大小框中（与 HTSIZE 相同）。
        HitHelp        = HTHELP,        ///< 在帮助按钮中。
        HitHScroll     = HTHSCROLL,     ///< 在水平滚动条中。
        HitLeft        = HTLEFT,        ///< 在可调整大小的窗口的左边框中（用户可以单击鼠标以水平调整窗口大小）。
        HitMenu        = HTMENU,        ///< 在菜单中。
        HitMaxButton   = HTMAXBUTTON,   ///< 在最大化按钮中。
        HitMinButton   = HTMINBUTTON,   ///< 在最小化按钮中。
        HitNoWhere     = HTNOWHERE,     ///< 在屏幕背景上，或在窗口之间的分隔线上。
        HitReduce      = HTREDUCE,      ///< 在最小化按钮中。
        HitRight       = HTRIGHT,       ///< 在可调整大小的窗口的右左边框中（用户可以单击鼠标以水平调整窗口大小）。
        HitSize        = HTSIZE,        ///< 在大小框中（与 HTGROWBOX 相同）。
        HitSysMenu     = HTSYSMENU,     ///< 在窗口菜单或子窗口的关闭按钮中。
        HitTop         = HTTOP,         ///< 在窗口的上水平边框中。
        HitTopLeft     = HTTOPLEFT,     ///< 在窗口边框的左上角。
        HitTopRight    = HTTOPRIGHT,    ///< 在窗口边框的右上角。
        HitTransparent = HTTRANSPARENT, ///< 在同一线程当前由另一个窗口覆盖的窗口中（消息将发送到同一线程中的基础窗口，直到其中一个窗口返回不是 HTTRANSPARENT 的代码）。
        HitVScroll     = HTVSCROLL,     ///< 在垂直滚动条中。
        HitZoom        = HTZOOM,        ///< 在最大化按钮中。
    };
}

// IComparable.h


namespace sw
{
    /**
     * @brief 相等性比较接口
     */
    template <
        typename TDerived,
        typename TOther = const TDerived &>
    class IEqualityComparable
    {
    public:
        /**
         * @brief 判断当前对象与另一个对象是否相等
         */
        bool Equals(TOther other) const
        {
            static_assert(
                !std::is_same<
                    decltype(&IEqualityComparable<TDerived, TOther>::Equals),
                    decltype(&TDerived::Equals)>::value,
                "Derived class must implement Equals method.");

            return static_cast<const TDerived *>(this)->Equals(other);
        }

        /**
         * @brief 判断当前对象是否与另一个对象相等
         */
        bool operator==(TOther other) const
        {
            return Equals(other);
        }

        /**
         * @brief 判断当前对象是否与另一个对象不相等
         */
        bool operator!=(TOther other) const
        {
            return !Equals(other);
        }
    };

    /**
     * @brief 全序比较接口
     */
    template <
        typename TDerived,
        typename TOther = const TDerived &>
    class IComparable : public IEqualityComparable<TDerived, TOther>
    {
    public:
        /**
         * @brief 比较当前对象与另一个对象的大小关系
         * @return 若当前对象小于另一个对象，返回负数；若等于，返回0；若大于，返回正数
         */
        int CompareTo(TOther other) const
        {
            static_assert(
                !std::is_same<
                    decltype(&IComparable<TDerived, TOther>::CompareTo),
                    decltype(&TDerived::CompareTo)>::value,
                "Derived class must implement CompareTo method.");

            return static_cast<const TDerived *>(this)->CompareTo(other);
        }

        /**
         * @brief 判断当前对象与另一个对象是否相等
         */
        bool Equals(TOther other) const
        {
            return CompareTo(other) == 0;
        }

        /**
         * @brief 判断当前对象是否小于另一个对象
         */
        bool operator<(TOther other) const
        {
            return CompareTo(other) < 0;
        }

        /**
         * @brief 判断当前对象是否小于或等于另一个对象
         */
        bool operator<=(TOther other) const
        {
            return CompareTo(other) <= 0;
        }

        /**
         * @brief 判断当前对象是否大于另一个对象
         */
        bool operator>(TOther other) const
        {
            return CompareTo(other) > 0;
        }

        /**
         * @brief 判断当前对象是否大于或等于另一个对象
         */
        bool operator>=(TOther other) const
        {
            return CompareTo(other) >= 0;
        }
    };
}

// IDialog.h

namespace sw
{
    class Window; // Window.h

    /**
     * @brief 对话框接口
     */
    class IDialog
    {
    public:
        /**
         * @brief 默认虚析构函数
         */
        virtual ~IDialog() = default;

        /**
         * @brief 关闭对话框
         */
        virtual void Close() = 0;

        /**
         * @brief 显示对话框
         */
        virtual void Show() = 0;

        /**
         * @brief 显示模态对话框
         * @param owner 所有者窗口，若为nullptr则使用当前活动窗口
         * @return 对话框结果，不同对话框有不同的返回值含义
         */
        virtual int ShowDialog(Window *owner = nullptr) = 0;
    };
}

// IToString.h


namespace sw
{
    /**
     * @brief 为支持ToString方法的类提供统一接口
     */
    template <typename TDerived>
    class IToString
    {
    public:
        /**
         * @brief 获取描述当前对象的字符串
         */
        std::wstring ToString() const
        {
            static_assert(
                !std::is_same<
                    decltype(&IToString<TDerived>::ToString),
                    decltype(&TDerived::ToString)>::value,
                "Derived class must implement ToString method.");

            return static_cast<const TDerived *>(this)->ToString();
        }
    };
}

// Icon.h


namespace sw
{
    /**
     * @brief 系统标准图标样式
     * @brief https://learn.microsoft.com/en-us/windows/win32/menurc/about-icons
     */
    enum class StandardIcon {
        Application = 32512, ///< Default application icon
        Error       = 32513, ///< Error icon
        Question    = 32514, ///< Question mark icon
        Warning     = 32515, ///< Warning icon
        Information = 32516, ///< Information icon
        WinLogo     = 32517, ///< Windows logo icon
        Shield      = 32518, ///< Security shield icon
    };

    /**
     * @brief 用于获取图标句柄的工具类
     */
    class IconHelper
    {
    private:
        /**
         * @brief 静态类，禁止实例化
         */
        IconHelper() = delete;

    public:
        /**
         * @brief 获取系统标准图标句柄
         * @param icon 图标样式
         * @return 图标句柄
         */
        static HICON GetIconHandle(StandardIcon icon);

        /**
         * @brief 从指定模块中获取图标句柄
         * @param hInstance DLL或EXE的模块句柄
         * @param resourceId 图标的资源序号
         * @return 成功则返回图标句柄，否则返回NULL
         */
        static HICON GetIconHandle(HINSTANCE hInstance, int resourceId);

        /**
         * @brief 从文件加载图标句柄
         * @param fileName 图标文件的路径
         * @return 成功则返回图标句柄，否则返回NULL
         */
        static HICON GetIconHandle(const std::wstring &fileName);
    };
}

// ImageList.h


// commctrl.h需要在windows.h之后包含

namespace sw
{
    /**
     * @brief Win32图像列表句柄的RAII包装
     * @note 非包装对象在析构时会调用ImageList_Destroy；通过Wrap创建的包装对象不负责销毁句柄。
     */
    class ImageList
    {
    private:
        /**
         * @brief 图像列表的句柄
         */
        HIMAGELIST _hImageList;

        /**
         * @brief 记录是否为包装对象
         */
        bool _isWrap;

    protected:
        /**
         * @brief 使用已有句柄初始化图像列表对象
         * @param hImageList 图像列表句柄
         * @param isWrap 是否仅包装句柄，若为true则析构时不销毁句柄
         */
        ImageList(HIMAGELIST hImageList, bool isWrap) noexcept;

    public:
        /**
         * @brief 创建图像列表，该函数调用ImageList_Create
         * @param cx 每个图像的宽度，单位为像素
         * @param cy 每个图像的高度，单位为像素
         * @param flags 创建标志，与ImageList_Create的flags参数相同
         * @param cInitial 图像列表的初始容量
         * @param cGrow 容量不足时每次增长的图像数量
         */
        ImageList(int cx, int cy, UINT flags, int cInitial, int cGrow) noexcept;

        /**
         * @brief 拷贝构造图像列表对象
         * @param other 要拷贝的对象
         * @note 包装对象会共享原句柄；非包装对象会调用ImageList_Duplicate复制句柄。
         */
        ImageList(const ImageList &other) noexcept;

        /**
         * @brief 移动构造图像列表对象
         * @param other 要移动的对象，移动后其句柄被置空
         */
        ImageList(ImageList &&other) noexcept;

        /**
         * @brief 析构图像列表对象
         * @note 若当前对象不是包装对象且句柄有效，则调用ImageList_Destroy销毁句柄。
         */
        ~ImageList() noexcept;

        /**
         * @brief 拷贝赋值图像列表对象
         * @param other 要拷贝的对象
         * @return 当前对象
         * @note 原有非包装句柄会先被销毁；包装对象会共享原句柄，非包装对象会复制句柄。
         */
        ImageList &operator=(const ImageList &other) noexcept;

        /**
         * @brief 移动赋值图像列表对象
         * @param other 要移动的对象，移动后其句柄被置空
         * @return 当前对象
         */
        ImageList &operator=(ImageList &&other) noexcept;

        /**
         * @brief 创建图像列表，该函数调用ImageList_Create
         * @param cx 每个图像的宽度，单位为像素
         * @param cy 每个图像的高度，单位为像素
         * @param flags 创建标志，与ImageList_Create的flags参数相同
         * @param cInitial 图像列表的初始容量
         * @param cGrow 容量不足时每次增长的图像数量
         * @return 拥有新图像列表句柄的对象，创建失败时句柄为NULL
         */
        static ImageList Create(int cx, int cy, UINT flags, int cInitial, int cGrow) noexcept;

        /**
         * @brief 将已有图像列表句柄包装为ImageList对象
         * @param hImageList 要包装的图像列表句柄
         * @return 包装原有句柄的对象，该对象析构时不会销毁句柄
         */
        static ImageList Wrap(HIMAGELIST hImageList) noexcept;

        /**
         * @brief 复制图像，该函数调用ImageList_Copy
         * @param dst 目标图像列表
         * @param iDst 目标图像索引
         * @param src 源图像列表
         * @param iSrc 源图像索引
         * @param uFlags 复制方式标志，与ImageList_Copy的uFlags参数相同
         * @return 操作成功返回true，否则返回false
         */
        static bool Copy(const ImageList &dst, int iDst, const ImageList &src, int iSrc, UINT uFlags) noexcept;

        /**
         * @brief 锁定窗口并在指定窗口内显示拖拽图像，该函数调用ImageList_DragEnter
         * @param hwndLock 要锁定更新的窗口句柄
         * @param x 拖拽图像相对锁定窗口左上角的水平位置，单位为像素
         * @param y 拖拽图像相对锁定窗口左上角的垂直位置，单位为像素
         * @return 操作成功返回true，否则返回false
         * @note hwndLock为NULL时，坐标相对屏幕左上角。
         */
        static bool DragEnter(HWND hwndLock, int x, int y) noexcept;

        /**
         * @brief 解除窗口锁定并隐藏显示的拖拽图像，该函数调用ImageList_DragLeave
         * @param hwndLock 先前传给DragEnter的窗口句柄
         * @return 操作成功返回true，否则返回false
         */
        static bool DragLeave(HWND hwndLock) noexcept;

        /**
         * @brief 拖拽移动，一般在WM_MOUSEMOVE函数中调用，该函数调用ImageList_DragMove
         * @param x 拖拽图像相对锁定窗口左上角的新水平位置，单位为像素
         * @param y 拖拽图像相对锁定窗口左上角的新垂直位置，单位为像素
         * @return 操作成功返回true，否则返回false
         * @note 拖拽锁定窗口为NULL时，坐标相对屏幕左上角。
         */
        static bool DragMove(int x, int y) noexcept;

        /**
         * @brief 拖拽时显示或隐藏图像，该函数调用ImageList_DragShowNolock
         * @param fShow 为true时显示拖拽图像，为false时隐藏拖拽图像
         * @return 操作成功返回true，否则返回false
         */
        static bool DragShowNolock(bool fShow) noexcept;

        /**
         * @brief 结束拖拽操作并销毁临时拖拽图像列表，该函数调用ImageList_EndDrag
         */
        static void EndDrag() noexcept;

        /**
         * @brief 获取拖拽中的临时图像列表，该函数调用ImageList_GetDragImage
         * @param ppt 接收当前拖拽位置，可为NULL
         * @param pptHotspot 接收拖拽图像热点相对拖拽位置的偏移，可为NULL
         * @return 包装临时图像列表句柄的对象；该句柄由拖拽过程管理，EndDrag后失效
         */
        static ImageList GetDragImage(POINT *ppt, POINT *pptHotspot) noexcept;

        /**
         * @brief 加载图像列表，该函数调用ImageList_LoadImageA
         * @param hi 包含资源的模块实例句柄
         * @param lpbmp 位图资源名、资源ID或文件路径
         * @param cx 每个图像的宽度，单位为像素
         * @param cGrow 容量不足时每次增长的图像数量
         * @param crMask 用作透明掩码的颜色
         * @param uType 图像类型，与LoadImage的uType参数相同
         * @param uFlags 加载标志，与LoadImage的fuLoad参数相同
         * @return 拥有新图像列表句柄的对象，加载失败时句柄为NULL
         */
        static ImageList LoadImageA(HINSTANCE hi, LPCSTR lpbmp, int cx, int cGrow, COLORREF crMask, UINT uType, UINT uFlags) noexcept;

        /**
         * @brief 加载图像列表，该函数调用ImageList_LoadImageW
         * @param hi 包含资源的模块实例句柄
         * @param lpbmp 位图资源名、资源ID或文件路径
         * @param cx 每个图像的宽度，单位为像素
         * @param cGrow 容量不足时每次增长的图像数量
         * @param crMask 用作透明掩码的颜色
         * @param uType 图像类型，与LoadImage的uType参数相同
         * @param uFlags 加载标志，与LoadImage的fuLoad参数相同
         * @return 拥有新图像列表句柄的对象，加载失败时句柄为NULL
         */
        static ImageList LoadImageW(HINSTANCE hi, LPCWSTR lpbmp, int cx, int cGrow, COLORREF crMask, UINT uType, UINT uFlags) noexcept;

        /**
         * @brief 合并两个图像列表，该函数调用ImageList_Merge
         * @param iml1 第一个图像列表
         * @param i1 第一个图像列表中的图像索引
         * @param iml2 第二个图像列表
         * @param i2 第二个图像列表中的图像索引
         * @param dx 第二个图像相对第一个图像的水平偏移，单位为像素
         * @param dy 第二个图像相对第一个图像的垂直偏移，单位为像素
         * @return 拥有合并结果句柄的对象，合并失败时句柄为NULL
         */
        static ImageList Merge(const ImageList &iml1, int i1, const ImageList &iml2, int i2, int dx, int dy) noexcept;

        /**
         * @brief 读取图像列表，该函数调用ImageList_Read
         * @param pstm 要读取的流对象
         * @return 拥有读取结果句柄的对象，读取失败时句柄为NULL
         */
        static ImageList Read(IStream *pstm) noexcept;

    public:
        /**
         * @brief 获取图像列表句柄
         * @return 当前对象持有的HIMAGELIST句柄，所有权不转移
         */
        HIMAGELIST GetHandle() const noexcept;

        /**
         * @brief 判断当前对象是否为包装对象
         * @return 当前对象仅包装句柄且析构时不销毁句柄则返回true，否则返回false
         */
        bool IsWrap() const noexcept;

        /**
         * @brief 获取图像列表句柄并取消对句柄的托管
         * @return 原图像列表句柄，调用方负责在需要时销毁该句柄
         * @note 调用后当前对象的句柄被置空，析构时不会销毁原句柄。
         */
        HIMAGELIST ReleaseHandle() noexcept;

        /**
         * @brief 添加图像，该函数调用ImageList_Add
         * @param hbmImage 包含一个或多个图像的位图句柄
         * @param hbmMask 包含掩码的位图句柄，可为NULL
         * @return 新增图像中第一个图像的索引，失败返回-1
         */
        int Add(HBITMAP hbmImage, HBITMAP hbmMask) noexcept;

        /**
         * @brief 添加图标，该函数调用ImageList_AddIcon
         * @param hIcon 要添加的图标句柄
         * @return 新增图标的索引，失败返回-1
         */
        int AddIcon(HICON hIcon) noexcept;

        /**
         * @brief 添加图像，指定颜色为mask，该函数调用ImageList_AddMasked
         * @param hbmImage 包含一个或多个图像的位图句柄
         * @param crMask 用作透明掩码的颜色
         * @return 新增图像中第一个图像的索引，失败返回-1
         */
        int AddMasked(HBITMAP hbmImage, COLORREF crMask) noexcept;

        /**
         * @brief 开始拖拽图像，该函数调用ImageList_BeginDrag
         * @param iTrack 要拖拽的图像索引
         * @param dxHotspot 拖拽热点相对图像左上角的水平偏移，单位为像素
         * @param dyHotspot 拖拽热点相对图像左上角的垂直偏移，单位为像素
         * @return 操作成功返回true，否则返回false
         */
        bool BeginDrag(int iTrack, int dxHotspot, int dyHotspot) noexcept;

        /**
         * @brief 在指定上下文DC下绘制图像，该函数调用ImageList_Draw
         * @param i 要绘制的图像索引
         * @param hdcDst 目标设备上下文句柄
         * @param x 绘制位置的水平坐标，单位为像素
         * @param y 绘制位置的垂直坐标，单位为像素
         * @param fStyle 绘制样式标志，与ImageList_Draw的fStyle参数相同
         * @return 操作成功返回true，否则返回false
         */
        bool Draw(int i, HDC hdcDst, int x, int y, UINT fStyle) noexcept;

        /**
         * @brief 在指定上下文DC下绘制图像，该函数调用ImageList_DrawEx
         * @param i 要绘制的图像索引
         * @param hdcDst 目标设备上下文句柄
         * @param x 绘制位置的水平坐标，单位为像素
         * @param y 绘制位置的垂直坐标，单位为像素
         * @param dx 绘制宽度，单位为像素
         * @param dy 绘制高度，单位为像素
         * @param rgbBk 背景颜色，与ImageList_DrawEx的rgbBk参数相同
         * @param rgbFg 前景颜色，与ImageList_DrawEx的rgbFg参数相同
         * @param fStyle 绘制样式标志，与ImageList_DrawEx的fStyle参数相同
         * @return 操作成功返回true，否则返回false
         */
        bool Draw(int i, HDC hdcDst, int x, int y, int dx, int dy, COLORREF rgbBk, COLORREF rgbFg, UINT fStyle) noexcept;

        /**
         * @brief 复制当前图像列表，该函数调用ImageList_Duplicate
         * @return 拥有复制结果句柄的对象，复制失败时句柄为NULL
         */
        ImageList Duplicate() noexcept;

        /**
         * @brief 获取背景颜色，该函数调用ImageList_GetBkColor
         * @return 当前背景颜色，返回值语义与ImageList_GetBkColor相同
         */
        COLORREF GetBkColor() noexcept;

        /**
         * @brief 通过指定位置的图像创建图标句柄，该函数调用ImageList_GetIcon
         * @param i 图像索引
         * @param flags 绘制样式标志，与ImageList_GetIcon的flags参数相同
         * @return 新创建的图标句柄，失败返回NULL；调用方负责在不再使用时销毁该图标
         */
        HICON GetIcon(int i, UINT flags) noexcept;

        /**
         * @brief 获取图标大小，该函数调用ImageList_GetIconSize
         * @param cx 接收图像宽度，单位为像素
         * @param cy 接收图像高度，单位为像素
         * @return 操作成功返回true，否则返回false
         */
        bool GetIconSize(int &cx, int &cy) noexcept;

        /**
         * @brief 获取图像个数，该函数调用ImageList_GetImageCount
         * @return 图像列表中的图像数量
         */
        int GetImageCount() noexcept;

        /**
         * @brief 获取图像信息，该函数调用ImageList_GetImageInfo
         * @param i 图像索引
         * @param pImageInfo 接收图像信息的IMAGEINFO结构指针
         * @return 操作成功返回true，否则返回false
         */
        bool GetImageInfo(int i, IMAGEINFO *pImageInfo) noexcept;

        /**
         * @brief 移除指定图像，该函数调用ImageList_Remove
         * @param i 要移除的图像索引，传入-1时移除所有图像
         * @return 操作成功返回true，否则返回false
         */
        bool Remove(int i) noexcept;

        /**
         * @brief 移除所有图像，该函数调用ImageList_Remove
         * @return 操作成功返回true，否则返回false
         */
        bool RemoveAll() noexcept;

        /**
         * @brief 更换指定位置的图像，该函数调用ImageList_Replace
         * @param i 要更换的图像索引
         * @param hbmImage 新图像位图句柄
         * @param hbmMask 新掩码位图句柄，可为NULL
         * @return 操作成功返回true，否则返回false
         */
        bool Replace(int i, HBITMAP hbmImage, HBITMAP hbmMask) noexcept;

        /**
         * @brief 更换图标，该函数调用ImageList_ReplaceIcon
         * @param i 要更换的图像索引，传入-1时追加图标
         * @param hicon 新图标句柄
         * @return 被更换或追加图标的索引，失败返回-1
         */
        int ReplaceIcon(int i, HICON hicon) noexcept;

        /**
         * @brief 设置背景颜色，该函数调用ImageList_SetBkColor
         * @param clrBk 新背景颜色
         * @return 先前的背景颜色，返回值语义与ImageList_SetBkColor相同
         */
        COLORREF SetBkColor(COLORREF clrBk) noexcept;

        /**
         * @brief 设置拖拽图标为指定图标与当前拖拽图标的结合，该函数调用ImageList_SetDragCursorImage
         * @param iDrag 当前图像列表中要组合到拖拽图像上的图像索引
         * @param dxHotspot 新图像相对拖拽热点的水平偏移，单位为像素
         * @param dyHotspot 新图像相对拖拽热点的垂直偏移，单位为像素
         * @return 操作成功返回true，否则返回false
         */
        bool SetDragCursorImage(int iDrag, int dxHotspot, int dyHotspot) noexcept;

        /**
         * @brief 设置图像大小并移除所有图像，该函数调用ImageList_SetIconSize
         * @param cx 新图像宽度，单位为像素
         * @param cy 新图像高度，单位为像素
         * @return 操作成功返回true，否则返回false
         */
        bool SetIconSize(int cx, int cy) noexcept;

        /**
         * @brief 设置图像个数，该函数调用ImageList_SetImageCount
         * @param uNewCount 新图像数量
         * @return 操作成功返回true，否则返回false
         */
        bool SetImageCount(UINT uNewCount) noexcept;

        /**
         * @brief 将指定的图像添加到要用作覆盖遮罩的图像列表中，该函数调用ImageList_SetOverlayImage
         * @param iImage 要用作覆盖图像的图像索引
         * @param iOverlay 覆盖图像编号，用于INDEXTOOVERLAYMASK
         * @return 操作成功返回true，否则返回false
         */
        bool SetOverlayImage(int iImage, int iOverlay) noexcept;

        /**
         * @brief 写图像列表，该函数调用ImageList_Write
         * @param pstm 要写入的流对象
         * @return 操作成功返回true，否则返回false
         */
        bool Write(IStream *pstm) noexcept;

    private:
        /**
         * @brief 在当前对象拥有句柄时调用ImageList_Destroy
         */
        void _DestroyIfNotWrap() noexcept;
    };
}

// Internal.h


namespace sw
{
    /**
     * @brief 最佳参数类型，标量类型使用值传递，复杂类型使用常量引用传递
     */
    template <typename T>
    using _OptimalParamType = typename std::conditional<
        std::is_scalar<typename std::decay<T>::type>::value,
        typename std::decay<T>::type, const typename std::decay<T>::type &>::type;

    /**
     * @brief 判断类型是否可以显式转换的辅助模板
     */
    template <typename TFrom, typename TTo, typename = void>
    struct _IsExplicitlyConvertable : std::false_type {
    };

    /**
     * @brief _IsExplicitlyConvertable模板特化
     */
    template <typename TFrom, typename TTo>
    struct _IsExplicitlyConvertable<
        TFrom, TTo, decltype(void(static_cast<TTo>(std::declval<TFrom>())))> : std::true_type {
    };

    /**
     * @brief 用于判断是否可以通过static_cast进行转换
     */
    template <typename TFrom, typename TTo>
    using _IsStaticCastable = _IsExplicitlyConvertable<TFrom, TTo>;

    /**
     * @brief 判断类型是否可以使用dynamic_cast进行转换的辅助模板
     */
    template <typename TFrom, typename TTo, typename = void>
    struct _IsDynamicCastable : std::false_type {
    };

    /**
     * @brief _IsDynamicCastable模板特化
     */
    template <typename TFrom, typename TTo>
    struct _IsDynamicCastable<
        TFrom, TTo, decltype(void(dynamic_cast<TTo>(std::declval<TFrom>())))> : std::true_type {
    };

    /**
     * @brief 判断一个类型是否有ToString方法
     */
    template <typename T, typename = void>
    struct _HasToString : std::false_type {
    };

    /**
     * @brief _HasToString偏特化版本
     */
    template <typename T>
    struct _HasToString<
        T, decltype(void(std::declval<T>().ToString()))> : std::true_type {
    };

    /**
     * @brief 判断委托是否可以添加或删除指定类型的函数对象
     */
    template <typename TDelegate, typename TFunc, typename = void>
    struct _DelegateCanAddSubtract : std::false_type {
    };

    /**
     * @brief _DelegateCanAddSubtract偏特化版本，检查是否可以使用+=和-=操作符添加或删除函数对象
     */
    template <typename TDelegate, typename TFunc>
    struct _DelegateCanAddSubtract<
        TDelegate, TFunc,
        decltype(void(std::declval<TDelegate>() += std::declval<TFunc>()),
                 void(std::declval<TDelegate>() -= std::declval<TFunc>()))>
        : std::true_type {
    };
}

// KnownColors.h


namespace sw
{
    /**
     * @brief 已知的颜色
     */
    enum class KnownColors {
        ActiveBorder            = RGB(0xb4, 0xb4, 0xb4), ///< 活动边框颜色
        ActiveCaption           = RGB(0x99, 0xb4, 0xd1), ///< 活动标题栏颜色
        ActiveCaptionText       = RGB(0x00, 0x00, 0x00), ///< 活动标题栏文本颜色
        AppWorkspace            = RGB(0xab, 0xab, 0xab), ///< 应用程序工作区颜色
        Control                 = RGB(0xf0, 0xf0, 0xf0), ///< 控件颜色
        ControlDark             = RGB(0xa0, 0xa0, 0xa0), ///< 控件深色颜色
        ControlDarkDark         = RGB(0x69, 0x69, 0x69), ///< 控件深深色颜色
        ControlLight            = RGB(0xe3, 0xe3, 0xe3), ///< 控件浅色颜色
        ControlLightLight       = RGB(0xff, 0xff, 0xff), ///< 控件浅浅色颜色
        ControlText             = RGB(0x00, 0x00, 0x00), ///< 控件文本颜色
        Desktop                 = RGB(0x00, 0x00, 0x00), ///< 桌面颜色
        GrayText                = RGB(0x6d, 0x6d, 0x6d), ///< 灰色文本颜色
        Highlight               = RGB(0x00, 0x78, 0xd7), ///< 高亮颜色
        HighlightText           = RGB(0xff, 0xff, 0xff), ///< 高亮文本颜色
        HotTrack                = RGB(0x00, 0x66, 0xcc), ///< 热跟踪颜色
        InactiveBorder          = RGB(0xf4, 0xf7, 0xfc), ///< 非活动边框颜色
        InactiveCaption         = RGB(0xbf, 0xcd, 0xdb), ///< 非活动标题栏颜色
        InactiveCaptionText     = RGB(0x00, 0x00, 0x00), ///< 非活动标题栏文本颜色
        Info                    = RGB(0xff, 0xff, 0xe1), ///< 信息颜色
        InfoText                = RGB(0x00, 0x00, 0x00), ///< 信息文本颜色
        Menu                    = RGB(0xf0, 0xf0, 0xf0), ///< 菜单颜色
        MenuText                = RGB(0x00, 0x00, 0x00), ///< 菜单文本颜色
        ScrollBar               = RGB(0xc8, 0xc8, 0xc8), ///< 滚动条颜色
        Window                  = RGB(0xff, 0xff, 0xff), ///< 窗口颜色
        WindowFrame             = RGB(0x64, 0x64, 0x64), ///< 窗口框架颜色
        WindowText              = RGB(0x00, 0x00, 0x00), ///< 窗口文本颜色
        Transparent             = RGB(0xff, 0xff, 0xff), ///< 透明色
        AliceBlue               = RGB(0xf0, 0xf8, 0xff), ///< 爱丽丝蓝色
        AntiqueWhite            = RGB(0xfa, 0xeb, 0xd7), ///< 古董白色
        Aqua                    = RGB(0x00, 0xff, 0xff), ///< 青色
        Aquamarine              = RGB(0x7f, 0xff, 0xd4), ///< 碧绿色
        Azure                   = RGB(0xf0, 0xff, 0xff), ///< 天蓝色
        Beige                   = RGB(0xf5, 0xf5, 0xdc), ///< 米色
        Bisque                  = RGB(0xff, 0xe4, 0xc4), ///< 橘黄色
        Black                   = RGB(0x00, 0x00, 0x00), ///< 黑色
        BlanchedAlmond          = RGB(0xff, 0xeb, 0xcd), ///< 白杏色
        Blue                    = RGB(0x00, 0x00, 0xff), ///< 蓝色
        BlueViolet              = RGB(0x8a, 0x2b, 0xe2), ///< 蓝紫色
        Brown                   = RGB(0xa5, 0x2a, 0x2a), ///< 棕色
        BurlyWood               = RGB(0xde, 0xb8, 0x87), ///< 硬木色
        CadetBlue               = RGB(0x5f, 0x9e, 0xa0), ///< 军装蓝色
        Chartreuse              = RGB(0x7f, 0xff, 0x00), ///< 黄绿色
        Chocolate               = RGB(0xd2, 0x69, 0x1e), ///< 巧克力色
        Coral                   = RGB(0xff, 0x7f, 0x50), ///< 珊瑚色
        CornflowerBlue          = RGB(0x64, 0x95, 0xed), ///< 矢车菊蓝色
        Cornsilk                = RGB(0xff, 0xf8, 0xdc), ///< 米绸色
        Crimson                 = RGB(0xdc, 0x14, 0x3c), ///< 绯红色
        Cyan                    = RGB(0x00, 0xff, 0xff), ///< 青色
        DarkBlue                = RGB(0x00, 0x00, 0x8b), ///< 深蓝色
        DarkCyan                = RGB(0x00, 0x8b, 0x8b), ///< 深青色
        DarkGoldenrod           = RGB(0xb8, 0x86, 0x0b), ///< 深金黄色
        DarkGray                = RGB(0xa9, 0xa9, 0xa9), ///< 深灰色
        DarkGreen               = RGB(0x00, 0x64, 0x00), ///< 深绿色
        DarkKhaki               = RGB(0xbd, 0xb7, 0x6b), ///< 深卡其色
        DarkMagenta             = RGB(0x8b, 0x00, 0x8b), ///< 深洋红色
        DarkOliveGreen          = RGB(0x55, 0x6b, 0x2f), ///< 深橄榄绿色
        DarkOrange              = RGB(0xff, 0x8c, 0x00), ///< 深橙色
        DarkOrchid              = RGB(0x99, 0x32, 0xcc), ///< 深兰花色
        DarkRed                 = RGB(0x8b, 0x00, 0x00), ///< 深红色
        DarkSalmon              = RGB(0xe9, 0x96, 0x7a), ///< 深鲑鱼色
        DarkSeaGreen            = RGB(0x8f, 0xbc, 0x8f), ///< 深海绿色
        DarkSlateBlue           = RGB(0x48, 0x3d, 0x8b), ///< 深青蓝色
        DarkSlateGray           = RGB(0x2f, 0x4f, 0x4f), ///< 深青灰色
        DarkTurquoise           = RGB(0x00, 0xce, 0xd1), ///< 深青色
        DarkViolet              = RGB(0x94, 0x00, 0xd3), ///< 深紫罗兰色
        DeepPink                = RGB(0xff, 0x14, 0x93), ///< 深粉色
        DeepSkyBlue             = RGB(0x00, 0xbf, 0xff), ///< 深天蓝色
        DimGray                 = RGB(0x69, 0x69, 0x69), ///< 暗灰色
        DodgerBlue              = RGB(0x1e, 0x90, 0xff), ///< 道奇蓝色
        Firebrick               = RGB(0xb2, 0x22, 0x22), ///< 火砖色
        FloralWhite             = RGB(0xff, 0xfa, 0xf0), ///< 花白色
        ForestGreen             = RGB(0x22, 0x8b, 0x22), ///< 森林绿色
        Fuchsia                 = RGB(0xff, 0x00, 0xff), ///< 紫红色
        Gainsboro               = RGB(0xdc, 0xdc, 0xdc), ///< 盖茨伯勒色
        GhostWhite              = RGB(0xf8, 0xf8, 0xff), ///< 幽灵白色
        Gold                    = RGB(0xff, 0xd7, 0x00), ///< 金色
        Goldenrod               = RGB(0xda, 0xa5, 0x20), ///< 金菊黄色
        Gray                    = RGB(0x80, 0x80, 0x80), ///< 灰色
        Green                   = RGB(0x00, 0x80, 0x00), ///< 绿色
        GreenYellow             = RGB(0xad, 0xff, 0x2f), ///< 绿黄色
        Honeydew                = RGB(0xf0, 0xff, 0xf0), ///< 蜜色
        HotPink                 = RGB(0xff, 0x69, 0xb4), ///< 亮粉红色
        IndianRed               = RGB(0xcd, 0x5c, 0x5c), ///< 印度红色
        Indigo                  = RGB(0x4b, 0x00, 0x82), ///< 靛青色
        Ivory                   = RGB(0xff, 0xff, 0xf0), ///< 象牙色
        Khaki                   = RGB(0xf0, 0xe6, 0x8c), ///< 卡其色
        Lavender                = RGB(0xe6, 0xe6, 0xfa), ///< 熏衣草色
        LavenderBlush           = RGB(0xff, 0xf0, 0xf5), ///< 淡紫红色
        LawnGreen               = RGB(0x7c, 0xfc, 0x00), ///< 草绿色
        LemonChiffon            = RGB(0xff, 0xfa, 0xcd), ///< 柠檬绸色
        LightBlue               = RGB(0xad, 0xd8, 0xe6), ///< 淡蓝色
        LightCoral              = RGB(0xf0, 0x80, 0x80), ///< 淡珊瑚色
        LightCyan               = RGB(0xe0, 0xff, 0xff), ///< 淡青色
        LightGoldenrodYellow    = RGB(0xfa, 0xfa, 0xd2), ///< 浅金菊黄色
        LightGray               = RGB(0xd3, 0xd3, 0xd3), ///< 浅灰色
        LightGreen              = RGB(0x90, 0xee, 0x90), ///< 浅绿色
        LightPink               = RGB(0xff, 0xb6, 0xc1), ///< 浅粉色
        LightSalmon             = RGB(0xff, 0xa0, 0x7a), ///< 浅鲑鱼色
        LightSeaGreen           = RGB(0x20, 0xb2, 0xaa), ///< 浅海绿色
        LightSkyBlue            = RGB(0x87, 0xce, 0xfa), ///< 浅天蓝色
        LightSlateGray          = RGB(0x77, 0x88, 0x99), ///< 浅青灰色
        LightSteelBlue          = RGB(0xb0, 0xc4, 0xde), ///< 浅钢蓝色
        LightYellow             = RGB(0xff, 0xff, 0xe0), ///< 浅黄色
        Lime                    = RGB(0x00, 0xff, 0x00), ///< 酸橙色
        LimeGreen               = RGB(0x32, 0xcd, 0x32), ///< 柠檬绿色
        Linen                   = RGB(0xfa, 0xf0, 0xe6), ///< 亚麻色
        Magenta                 = RGB(0xff, 0x00, 0xff), ///< 洋红色
        Maroon                  = RGB(0x80, 0x00, 0x00), ///< 栗色
        MediumAquamarine        = RGB(0x66, 0xcd, 0xaa), ///< 中碧绿色
        MediumBlue              = RGB(0x00, 0x00, 0xcd), ///< 中蓝色
        MediumOrchid            = RGB(0xba, 0x55, 0xd3), ///< 中兰花紫色
        MediumPurple            = RGB(0x93, 0x70, 0xdb), ///< 中紫色
        MediumSeaGreen          = RGB(0x3c, 0xb3, 0x71), ///< 中海绿色
        MediumSlateBlue         = RGB(0x7b, 0x68, 0xee), ///< 中青蓝色
        MediumSpringGreen       = RGB(0x00, 0xfa, 0x9a), ///< 中春绿色
        MediumTurquoise         = RGB(0x48, 0xd1, 0xcc), ///< 中青色
        MediumVioletRed         = RGB(0xc7, 0x15, 0x85), ///< 中紫罗兰色
        MidnightBlue            = RGB(0x19, 0x19, 0x70), ///< 午夜蓝色
        MintCream               = RGB(0xf5, 0xff, 0xfa), ///< 薄荷色
        MistyRose               = RGB(0xff, 0xe4, 0xe1), ///< 浅玫瑰色
        Moccasin                = RGB(0xff, 0xe4, 0xb5), ///< 鹿皮色
        NavajoWhite             = RGB(0xff, 0xde, 0xad), ///< 纳瓦白色
        Navy                    = RGB(0x00, 0x00, 0x80), ///< 海军蓝色
        OldLace                 = RGB(0xfd, 0xf5, 0xe6), ///< 老花色
        Olive                   = RGB(0x80, 0x80, 0x00), ///< 橄榄色
        OliveDrab               = RGB(0x6b, 0x8e, 0x23), ///< 橄榄褐色
        Orange                  = RGB(0xff, 0xa5, 0x00), ///< 橙色
        OrangeRed               = RGB(0xff, 0x45, 0x00), ///< 橙红色
        Orchid                  = RGB(0xda, 0x70, 0xd6), ///< 兰花紫色
        PaleGoldenrod           = RGB(0xee, 0xe8, 0xaa), ///< 苍麒麟色
        PaleGreen               = RGB(0x98, 0xfb, 0x98), ///< 苍绿色
        PaleTurquoise           = RGB(0xaf, 0xee, 0xee), ///< 苍宝石绿色
        PaleVioletRed           = RGB(0xdb, 0x70, 0x93), ///< 苍紫罗兰红色
        PapayaWhip              = RGB(0xff, 0xef, 0xd5), ///< 番木瓜色
        PeachPuff               = RGB(0xff, 0xda, 0xb9), ///< 桃肉色
        Peru                    = RGB(0xcd, 0x85, 0x3f), ///< 秘鲁色
        Pink                    = RGB(0xff, 0xc0, 0xcb), ///< 粉红色
        Plum                    = RGB(0xdd, 0xa0, 0xdd), ///< 李紫色
        PowderBlue              = RGB(0xb0, 0xe0, 0xe6), ///< 粉蓝色
        Purple                  = RGB(0x80, 0x00, 0x80), ///< 紫色
        Red                     = RGB(0xff, 0x00, 0x00), ///< 红色
        RosyBrown               = RGB(0xbc, 0x8f, 0x8f), ///< 玫瑰棕色
        RoyalBlue               = RGB(0x41, 0x69, 0xe1), ///< 皇家蓝色
        SaddleBrown             = RGB(0x8b, 0x45, 0x13), ///< 马鞍棕色
        Salmon                  = RGB(0xfa, 0x80, 0x72), ///< 鲑鱼色
        SandyBrown              = RGB(0xf4, 0xa4, 0x60), ///< 沙棕色
        SeaGreen                = RGB(0x2e, 0x8b, 0x57), ///< 海绿色
        SeaShell                = RGB(0xff, 0xf5, 0xee), ///< 海贝色
        Sienna                  = RGB(0xa0, 0x52, 0x2d), ///< 黄土赭色
        Silver                  = RGB(0xc0, 0xc0, 0xc0), ///< 银色
        SkyBlue                 = RGB(0x87, 0xce, 0xeb), ///< 天蓝色
        SlateBlue               = RGB(0x6a, 0x5a, 0xcd), ///< 青蓝色
        SlateGray               = RGB(0x70, 0x80, 0x90), ///< 青灰色
        Snow                    = RGB(0xff, 0xfa, 0xfa), ///< 雪白色
        SpringGreen             = RGB(0x00, 0xff, 0x7f), ///< 春绿色
        SteelBlue               = RGB(0x46, 0x82, 0xb4), ///< 钢蓝色
        Tan                     = RGB(0xd2, 0xb4, 0x8c), ///< 茶色
        Teal                    = RGB(0x00, 0x80, 0x80), ///< 鸭翅绿色
        Thistle                 = RGB(0xd8, 0xbf, 0xd8), ///< 蓟色
        Tomato                  = RGB(0xff, 0x63, 0x47), ///< 番茄色
        Turquoise               = RGB(0x40, 0xe0, 0xd0), ///< 青绿色
        Violet                  = RGB(0xee, 0x82, 0xee), ///< 紫罗兰色
        Wheat                   = RGB(0xf5, 0xde, 0xb3), ///< 小麦色
        White                   = RGB(0xff, 0xff, 0xff), ///< 白色
        WhiteSmoke              = RGB(0xf5, 0xf5, 0xf5), ///< 白烟色
        Yellow                  = RGB(0xff, 0xff, 0x00), ///< 黄色
        YellowGreen             = RGB(0x9a, 0xcd, 0x32), ///< 黄绿色
        ButtonFace              = RGB(0xf0, 0xf0, 0xf0), ///< 按钮表面色
        ButtonHighlight         = RGB(0xff, 0xff, 0xff), ///< 按钮高亮色
        ButtonShadow            = RGB(0xa0, 0xa0, 0xa0), ///< 按钮阴影色
        GradientActiveCaption   = RGB(0xb9, 0xd1, 0xea), ///< 渐变激活标题栏色
        GradientInactiveCaption = RGB(0xd7, 0xe4, 0xf2), ///< 渐变非激活标题栏色
        MenuBar                 = RGB(0xf0, 0xf0, 0xf0), ///< 菜单栏色
        MenuHighlight           = RGB(0x33, 0x99, 0xff), ///< 菜单高亮色
        RebeccaPurple           = RGB(0x66, 0x33, 0x99), ///< 紫罗兰色
    };
}

// Path.h


namespace sw
{
    /**
     * @brief 用于处理文件路径的工具类
     */
    class Path
    {
    private:
        /**
         * @brief 静态类，禁止实例化
         */
        Path() = delete;

    public:
        /**
         * @brief 获取文件名
         * @param path 文件的路径
         * @return 文件名，包含扩展名
         */
        static std::wstring GetFileName(const std::wstring &path);

        /**
         * @brief 获取文件名
         * @param path 文件的路径
         * @return 文件名，不含扩展名
         */
        static std::wstring GetFileNameWithoutExt(const std::wstring &path);

        /**
         * @brief 获取扩展名
         * @param path 文件的路径
         * @return 文件的扩展名，包含前面的点（如 ".txt"）；无扩展名时返回空字符串
         */
        static std::wstring GetExtension(const std::wstring &path);

        /**
         * @brief 获取文件所在路径
         * @param path 文件的路径
         * @return 文件所在路径
         */
        static std::wstring GetDirectory(const std::wstring &path);

        /**
         * @brief 对路径进行拼接
         * @param paths 要拼接的路径
         * @return 完整的路径
         */
        static std::wstring Combine(std::initializer_list<std::wstring> paths);

        /**
         * @brief 获取路径所对应的绝对路径
         * @param path 要转换的路径
         * @return 若函数成功则返回绝对路径，否则返回空字符串
         */
        static std::wstring GetAbsolutePath(const std::wstring &path);

    public:
        /**
         * @brief 对路径进行拼接
         * @param first 第一个路径
         * @param rest 要拼接的路径
         * @return 完整的路径
         */
        template <typename... Args>
        static auto Combine(const std::wstring &first, const Args &...rest) -> std::wstring
        {
            return Path::Combine(std::initializer_list<std::wstring>{first, rest...});
        }
    };
}

// ProcMsg.h


namespace sw
{
    /**
     * @brief 对Windows窗口消息的封装
     */
    struct ProcMsg {
        /**
         * @brief 接收到消息的窗口句柄
         */
        HWND hwnd;

        /**
         * @brief 消息类型
         */
        UINT uMsg;

        /**
         * @brief 消息的附加信息
         */
        WPARAM wParam;

        /**
         * @brief 消息的附加信息
         */
        LPARAM lParam;

        /**
         * @brief 默认构造函数
         */
        ProcMsg() = default;

        /**
         * @brief 构造ProcMsg
         */
        ProcMsg(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
            : hwnd(hwnd), uMsg(uMsg), wParam(wParam), lParam(lParam) {}
    };
}

// ScrollEnums.h


namespace sw
{
    /**
     * @brief 滚动条方向
     */
    enum class ScrollOrientation {
        Horizontal, ///< 水平滚动条
        Vertical,   ///< 垂直滚动条
    };

    /**
     * @brief 滚动条事件
     */
    enum class ScrollEvent {
        LineUp        = SB_LINEUP,        ///< Scrolls one line up.
        LineLeft      = SB_LINELEFT,      ///< Scrolls left by one unit.
        LineDown      = SB_LINEDOWN,      ///< Scrolls one line down.
        LineRight     = SB_LINERIGHT,     ///< Scrolls right by one unit.
        PageUp        = SB_PAGEUP,        ///< Scrolls one page up.
        PageLeft      = SB_PAGELEFT,      ///< Scrolls left by the width of the window.
        PageDown      = SB_PAGEDOWN,      ///< Scrolls one page down.
        PageRight     = SB_PAGERIGHT,     ///< Scrolls right by the width of the window.
        ThumbPosition = SB_THUMBPOSITION, ///< The user has dragged the scroll box (thumb) and released the mouse button. The HIWORD indicates the position of the scroll box at the end of the drag operation.
        ThubmTrack    = SB_THUMBTRACK,    ///< The user is dragging the scroll box. This message is sent repeatedly until the user releases the mouse button. The HIWORD indicates the position that the scroll box has been dragged to.
        Top           = SB_TOP,           ///< Scrolls to the upper left.
        Left          = SB_LEFT,          ///< Scrolls to the upper left.
        Bottom        = SB_BOTTOM,        ///< Scrolls to the lower right.
        Right         = SB_RIGHT,         ///< Scrolls to the lower right.
        EndScroll     = SB_ENDSCROLL,     ///< Ends scroll.
    };
}

// WndMsg.h


namespace sw
{
    /**
     * @brief 包含SimpleWindow用到的一些窗口消息
     */
    enum WndMsg : UINT {
        /// SimpleWindow所用消息的起始位置
        WM_SimpleWindowBegin = WM_APP + 0x3000,

        /// 控件布局发生变化时控件所在顶级窗口将收到该消息，wParam和lParam均未使用
        WM_UpdateLayout,

        /// 在窗口线程上执行指定委托，lParam为指向sw::Action<>的指针，wParam表示是否对委托指针执行delete
        WM_InvokeAction,

        /// 在WndBase::SetParent函数中设置父窗口之前发送该消息，wParam为新的父窗口句柄，lParam未使用
        WM_PreSetParent,

        /// SimpleWindow所用消息的结束位置
        WM_SimpleWindowEnd,
    };
}

// Color.h


namespace sw
{
    /**
     * @brief 颜色
     */
    struct Color : public IToString<Color>,
                   public IEqualityComparable<Color> {
        /**
         * @brief R分量
         */
        uint8_t r;

        /**
         * @brief G分量
         */
        uint8_t g;

        /**
         * @brief B分量
         */
        uint8_t b;

        /**
         * @brief 保留字段
         */
        uint8_t _reserved;

        /**
         * @brief 默认构造函数
         */
        Color() = default;

        /**
         * @brief 通过rgb构造Color结构体
         */
        Color(uint8_t r, uint8_t g, uint8_t b) noexcept;

        /**
         * @brief 通过KnownColors构造Color结构体
         */
        Color(KnownColors knownColor) noexcept;

        /**
         * @brief 通过COLORREF构造Color结构体
         * @note 标记为explicit以避免与隐式转换operator COLORREF()联用时
         *       发生意外的隐式整数到Color的转换路径
         */
        explicit Color(COLORREF color) noexcept;

        /**
         * @brief 显式转换为COLORREF（与explicit Color(COLORREF)对称）
         */
        explicit operator COLORREF() const noexcept;

        /**
         * @brief 判断两个Color是否相等
         */
        bool Equals(const Color &other) const noexcept;

        /**
         * @brief 获取描述当前对象的字符串
         */
        std::wstring ToString() const;
    };

    // Color应为POD类型
    static_assert(
        std::is_trivial<Color>::value && std::is_standard_layout<Color>::value,
        "Color should be a POD type.");
}

// Event.h


namespace sw
{
    // 向前声明
    template <typename>
    class Event;

    /*================================================================================*/

    /**
     * @brief 成员事件初始化器
     */
    template <typename TOwner, typename TDelegate>
    class MemberEventInitializer
    {
        template <typename>
        friend class Event;

    private:
        /**
         * @brief 事件所有者
         */
        TOwner *_owner;

        /**
         * @brief 委托访问器函数指针
         */
        TDelegate &(*_accessor)(TOwner *);

    public:
        /**
         * @brief 构造成员事件初始化器
         */
        explicit MemberEventInitializer(TOwner *owner)
            : _owner(owner), _accessor(nullptr)
        {
        }

        /**
         * @brief 设置委托访问器
         */
        MemberEventInitializer &Delegate(TDelegate &(*accessor)(TOwner *))
        {
            _accessor = accessor;
            return *this;
        }

        /**
         * @brief 使用成员函数获取委托
         */
        template <TDelegate (TOwner::*accessor)()>
        MemberEventInitializer &Delegate()
        {
            return Delegate([](TOwner *owner) -> TDelegate & {
                return (owner->*accessor)();
            });
        }

        /**
         * @brief 使用成员函数获取委托
         */
        template <TDelegate (TOwner::*accessor)() const>
        MemberEventInitializer &Delegate()
        {
            return Delegate([](TOwner *owner) -> TDelegate & {
                return (owner->*accessor)();
            });
        }

        /**
         * @brief 使用指定成员字段委托
         */
        template <TDelegate TOwner::*field>
        MemberEventInitializer &Delegate()
        {
            return Delegate([](TOwner *owner) -> TDelegate & {
                return owner->*field;
            });
        }
    };

    /**
     * @brief 静态事件初始化器
     */
    template <typename TDelegate>
    class StaticEventInitializer
    {
        template <typename>
        friend class Event;

    private:
        /**
         * @brief 委托访问器函数指针
         */
        TDelegate &(*_accessor)();

    public:
        /**
         * @brief 构造静态事件初始化器
         */
        StaticEventInitializer()
            : _accessor(nullptr)
        {
        }

        /**
         * @brief 设置委托访问器
         */
        StaticEventInitializer &Delegate(TDelegate &(*accessor)())
        {
            _accessor = accessor;
            return *this;
        }
    };

    /*================================================================================*/

    /**
     * @brief 事件类
     */
    template <typename TRet, typename... Args>
    class Event<Delegate<TRet(Args...)>> final
    {
    private:
        /**
         * @brief 用于存储任意签名函数指针的通用类型
         * @note 函数指针类型间通过reinterpret_cast互转再转回原类型不丢失信息（C++标准良定义），
         *       使用统一的函数指针类型作为存储可避免函数指针与void*之间的conditionally-supported转换。
         */
        using TFuncPtr = void (*)();

        /**
         * @brief 静态偏移量，表示静态事件
         */
        static constexpr std::ptrdiff_t _STATICOFFSET =
            (std::numeric_limits<std::ptrdiff_t>::max)();

    public:
        /**
         * @brief 事件的委托类型
         */
        using TDelegate = Delegate<TRet(Args...)>;

        /**
         * @brief 当前事件类型的类型别名
         */
        using TEvent = Event<TDelegate>;

    public:
        /**
         * @brief 构造成员事件
         * @param initializer 成员事件初始化器
         */
        template <typename TOwner>
        explicit Event(const MemberEventInitializer<TOwner, TDelegate> &initializer)
        {
            assert(initializer._owner != nullptr);
            assert(initializer._accessor != nullptr);

            SetOwner(initializer._owner);
            _accessor = reinterpret_cast<TFuncPtr>(initializer._accessor);

            _extractor = [](void *owner, TFuncPtr accessor) -> TDelegate & {
                return reinterpret_cast<TDelegate &(*)(TOwner *)>(accessor)(reinterpret_cast<TOwner *>(owner));
            };
        }

        /**
         * @brief 构造静态事件
         * @param initializer 静态事件初始化器
         */
        explicit Event(const StaticEventInitializer<TDelegate> &initializer)
        {
            assert(initializer._accessor != nullptr);

            SetOwner(nullptr);
            _accessor = reinterpret_cast<TFuncPtr>(initializer._accessor);

            _extractor = [](void * /*owner*/, TFuncPtr accessor) -> TDelegate & {
                return reinterpret_cast<TDelegate &(*)()>(accessor)();
            };
        }

        /**
         * @brief 添加事件处理程序
         * @param handler 事件处理程序，可以是任意可调用对象
         */
        template <typename T>
        auto operator+=(T &&handler) const
            -> typename std::enable_if<_DelegateCanAddSubtract<TDelegate, T>::value>::type
        {
            this->GetDelegate() += std::forward<T>(handler);
        }

        /**
         * @brief 移除事件处理程序
         * @param handler 事件处理程序，可以是任意可调用对象
         */
        template <typename T>
        auto operator-=(T &&handler) const
            -> typename std::enable_if<_DelegateCanAddSubtract<TDelegate, T>::value>::type
        {
            this->GetDelegate() -= std::forward<T>(handler);
        }

    public:
        /**
         * @brief 初始化成员事件
         */
        template <typename TOwner>
        static MemberEventInitializer<TOwner, TDelegate> Init(TOwner *owner)
        {
            return MemberEventInitializer<TOwner, TDelegate>(owner);
        }

        /**
         * @brief 初始化静态事件
         */
        static StaticEventInitializer<TDelegate> Init()
        {
            return StaticEventInitializer<TDelegate>();
        }

    private:
        /**
         * @brief 事件所有者对象相对于当前事件对象的偏移量
         */
        std::ptrdiff_t _offset;

        /**
         * @brief 委托访问器指针
         */
        TFuncPtr _accessor;

        /**
         * @brief 类型擦除的委托访问器，用于获取事件的委托引用
         */
        TDelegate &(*_extractor)(void *owner, TFuncPtr accessor);

        /**
         * @brief 判断事件是否为静态事件
         */
        bool IsStatic() const noexcept
        {
            return _offset == _STATICOFFSET;
        }

        /**
         * @brief 设置事件所有者对象
         * @param owner 事件所有者对象指针，nullptr表示静态事件
         */
        void SetOwner(void *owner) noexcept
        {
            if (owner == nullptr) {
                _offset = _STATICOFFSET;
            } else {
                _offset = reinterpret_cast<uint8_t *>(owner) - reinterpret_cast<uint8_t *>(this);
            }
        }

        /**
         * @brief 获取事件所有者对象，当事件为静态事件时返回nullptr
         */
        void *GetOwner() const noexcept
        {
            if (IsStatic()) {
                return nullptr;
            } else {
                return const_cast<uint8_t *>(reinterpret_cast<const uint8_t *>(this)) + _offset;
            }
        }

        /**
         * @brief 获取事件的委托引用
         */
        TDelegate &GetDelegate() const
        {
            return _extractor(GetOwner(), _accessor);
        }
    };

    /*================================================================================*/

    /**
     * @brief 事件参数结构体
     */
    struct EventArgs {};

    /**
     * @brief 事件处理函数类型别名
     */
    template <typename TSender, typename TEventArgs = EventArgs>
    using EventHandler = Delegate<void(TSender &, TEventArgs &)>;
}

// ITag.h


namespace sw
{
    /**
     * @brief Tag接口
     */
    template <typename T>
    class ITag
    {
    public:
        /**
         * @brief 默认虚析构函数
         */
        virtual ~ITag() = default;

    public:
        /**
         * @brief 获取Tag
         */
        virtual T GetTag() const = 0;

        /**
         * @brief 设置Tag
         */
        virtual void SetTag(_OptimalParamType<T> tag) = 0;
    };
}

// IValueConverter.h


namespace sw
{
    /**
     * @brief 值转换器接口
     * @tparam TSource 源类型
     * @tparam TTarget 目标类型
     */
    template <typename TSource, typename TTarget>
    class IValueConverter
    {
    public:
        /**
         * @brief 源数据传参类型
         */
        using TSourceParam = _OptimalParamType<TSource>;

        /**
         * @brief 目标数据传参类型
         */
        using TTargetParam = _OptimalParamType<TTarget>;

        /**
         * @brief 默认析构函数
         */
        virtual ~IValueConverter() = default;

    public:
        /**
         * @brief 将源类型转换为目标类型
         * @param source 源值
         * @return 转换后的目标值
         */
        virtual TTarget Convert(TSourceParam source) = 0;

        /**
         * @brief 将目标类型转换为源类型
         * @param target 目标值
         * @return 转换后的源值
         */
        virtual TSource ConvertBack(TTargetParam target) = 0;
    };
}

// Keys.h


namespace sw
{
    /**
     * @brief https://learn.microsoft.com/en-us/windows/win32/inputdev/about-keyboard-input#keystroke-message-flags
     */
    struct KeyFlags {
        /**
         * @brief repeat count, > 0 if several keydown messages was combined into one message
         */
        uint16_t repeatCount;

        /**
         * @brief scan code
         */
        uint8_t scanCode;

        /**
         * @brief extended-key flag, 1 if scancode has 0xE0 prefix
         */
        bool isExtendedKey;

        /**
         * @brief indicates whether the ALT key was down
         */
        bool contextCode;

        /**
         * @brief indicates whether the key that generated the keystroke message was previously up or down
         */
        bool previousKeyState;

        /**
         * @brief transition-state flag, 1 on keyup
         */
        bool transitionState;

        /**
         * @brief 默认构造函数
         */
        KeyFlags() = default;

        /**
         * @brief 从lParam解析出各个字段
         */
        explicit KeyFlags(LPARAM lParam)
            : repeatCount((lParam >> 0) & 0xFFFF),
              scanCode((lParam >> 16) & 0xFF),
              isExtendedKey((lParam >> 24) & 0x01),
              contextCode((lParam >> 29) & 0x01),
              previousKeyState((lParam >> 30) & 0x01),
              transitionState((lParam >> 31) & 0x01)
        {
        }
    };

    /**
     * @brief 虚拟按键
     */
    enum class VirtualKey {
        MouseLeft   = 0x01, ///< Left mouse button
        MouseRight  = 0x02, ///< Right mouse button
        Cancel      = 0x03, ///< Control-break processing
        MouseMiddle = 0x04, ///< Middle mouse button (three-button mouse)
        MouseX1     = 0x05, ///< X1 mouse button
        MouseX2     = 0x06, ///< X2 mouse button

        //- 0x07 Undefined

        Backspace = 0x08, ///< BACKSPACE key
        Tab       = 0x09, ///< TAB key

        //- 0x0A-0B Reserved

        Clear = 0x0C, ///< CLEAR key
        Enter = 0x0D, ///< ENTER key

        //- 0x0E-0F Undefined

        Shift      = 0x10, ///< SHIFT key
        Ctrl       = 0x11, ///< CTRL key
        Alt        = 0x12, ///< ALT key
        Pause      = 0x13, ///< PAUSE key
        CapsLock   = 0x14, ///< CAPS LOCK key
        Kana       = 0x15, ///< IME Kana mode
        Hanguel    = 0x15, ///< IME Hanguel mode (maintained for compatibility; use VK_HANGUL)
        Hangul     = 0x15, ///< IME Hangul mode
        IME_On     = 0x16, ///< IME On
        Junja      = 0x17, ///< IME Junja mode
        Final      = 0x18, ///< IME final mode
        Hanja      = 0x19, ///< IME Hanja mode
        KANJI      = 0x19, ///< IME Kanji mode
        IME_Off    = 0x1A, ///< IME Off
        Esc        = 0x1B, ///< ESC key
        Convert    = 0x1C, ///< IME convert
        Nonconvert = 0x1D, ///< IME nonconvert
        Accept     = 0x1E, ///< IME accept
        ModeChange = 0x1F, ///< IME mode change request
        Space      = 0x20, ///< SPACEBAR
        PageUp     = 0x21, ///< PAGE UP key
        PageDown   = 0x22, ///< PAGE DOWN key
        End        = 0x23, ///< END key
        Home       = 0x24, ///< HOME key
        Left       = 0x25, ///< LEFT ARROW key
        Up         = 0x26, ///< UP ARROW key
        Right      = 0x27, ///< RIGHT ARROW key
        Down       = 0x28, ///< DOWN ARROW key
        Select     = 0x29, ///< SELECT key
        Print      = 0x2A, ///< PRINT key
        Execute    = 0x2B, ///< EXECUTE key
        Snapshot   = 0x2C, ///< PRINT SCREEN key
        Insert     = 0x2D, ///< INS key
        Delete     = 0x2E, ///< DEL key
        Help       = 0x2F, ///< HELP key

        Zero  = 0x30, ///< 0 key
        One   = 0x31, ///< 1 key
        Two   = 0x32, ///< 2 key
        Three = 0x33, ///< 3 key
        Four  = 0x34, ///< 4 key
        Five  = 0x35, ///< 5 key
        Six   = 0x36, ///< 6 key
        Seven = 0x37, ///< 7 key
        Eight = 0x38, ///< 8 key
        Nine  = 0x39, ///< 9 key

        //- 0x3A-40 Undefined

        A = 0x41, ///< A key
        B = 0x42, ///< B key
        C = 0x43, ///< C key
        D = 0x44, ///< D key
        E = 0x45, ///< E key
        F = 0x46, ///< F key
        G = 0x47, ///< G key
        H = 0x48, ///< H key
        I = 0x49, ///< I key
        J = 0x4A, ///< J key
        K = 0x4B, ///< K key
        L = 0x4C, ///< L key
        M = 0x4D, ///< M key
        N = 0x4E, ///< N key
        O = 0x4F, ///< O key
        P = 0x50, ///< P key
        Q = 0x51, ///< Q key
        R = 0x52, ///< R key
        S = 0x53, ///< S key
        T = 0x54, ///< T key
        U = 0x55, ///< U key
        V = 0x56, ///< V key
        W = 0x57, ///< W key
        X = 0x58, ///< X key
        Y = 0x59, ///< Y key
        Z = 0x5A, ///< Z key

        LeftWindows  = 0x5B, ///< Left Windows key (Natural keyboard)
        RightWindows = 0x5C, ///< Right Windows key (Natural keyboard)
        Applications = 0x5D, ///< Applications key (Natural keyboard)

        //- 0x5E Reserved

        Sleep     = 0x5F, ///< Computer Sleep key
        NumPad0   = 0x60, ///< Numeric keypad 0 key
        NumPad1   = 0x61, ///< Numeric keypad 1 key
        NumPad2   = 0x62, ///< Numeric keypad 2 key
        NumPad3   = 0x63, ///< Numeric keypad 3 key
        NumPad4   = 0x64, ///< Numeric keypad 4 key
        NumPad5   = 0x65, ///< Numeric keypad 5 key
        NumPad6   = 0x66, ///< Numeric keypad 6 key
        NumPad7   = 0x67, ///< Numeric keypad 7 key
        NumPad8   = 0x68, ///< Numeric keypad 8 key
        NumPad9   = 0x69, ///< Numeric keypad 9 key
        Multipy   = 0x6A, ///< Multiply key
        Add       = 0x6B, ///< Add key
        Separator = 0x6C, ///< Separator key
        Subtract  = 0x6D, ///< Subtract key
        Decimal   = 0x6E, ///< Decimal key
        Divide    = 0x6F, ///< Divide key
        F1        = 0x70, ///< F1 key
        F2        = 0x71, ///< F2 key
        F3        = 0x72, ///< F3 key
        F4        = 0x73, ///< F4 key
        F5        = 0x74, ///< F5 key
        F6        = 0x75, ///< F6 key
        F7        = 0x76, ///< F7 key
        F8        = 0x77, ///< F8 key
        F9        = 0x78, ///< F9 key
        F10       = 0x79, ///< F10 key
        F11       = 0x7A, ///< F11 key
        F12       = 0x7B, ///< F12 key
        F13       = 0x7C, ///< F13 key
        F14       = 0x7D, ///< F14 key
        F15       = 0x7E, ///< F15 key
        F16       = 0x7F, ///< F16 key
        F17       = 0x80, ///< F17 key
        F18       = 0x81, ///< F18 key
        F19       = 0x82, ///< F19 key
        F20       = 0x83, ///< F20 key
        F21       = 0x84, ///< F21 key
        F22       = 0x85, ///< F22 key
        F23       = 0x86, ///< F23 key
        F24       = 0x87, ///< F24 key

        //- 0x88-8F Unassigned

        NumLock    = 0x90, ///< NUM LOCK key
        ScrollLock = 0x91, ///< SCROLL LOCK key

        // 0x92-96 OEM specific
        //- 0x97-9F Unassigned

        LeftShift          = 0xA0, ///< Left SHIFT key
        RightShift         = 0xA1, ///< Right SHIFT key
        LeftCtrl           = 0xA2, ///< Left CONTROL key
        RightCtrl          = 0xA3, ///< Right CONTROL key
        LeftAlt            = 0xA4, ///< Left ALT key
        RightAlt           = 0xA5, ///< Right ALT key
        BrowserBack        = 0xA6, ///< Browser Back key
        BrowserForward     = 0xA7, ///< Browser Forward key
        BrowserRefresh     = 0xA8, ///< Browser Refresh key
        BrowserStop        = 0xA9, ///< Browser Stop key
        BrowserSearch      = 0xAA, ///< Browser Search key
        BrowserFavorites   = 0xAB, ///< Browser Favorites key
        BrowserHome        = 0xAC, ///< Browser Start and Home key
        VolumeMute         = 0xAD, ///< Volume Mute key
        VolumeDown         = 0xAE, ///< Volume Down key
        VolumeUp           = 0xAF, ///< Volume Up key
        MediaNextTrack     = 0xB0, ///< Next Track key
        MediaPreviousTrack = 0xB1, ///< Previous Track key
        MediaStop          = 0xB2, ///< Stop Media key
        MediaPlayPause     = 0xB3, ///< Play/Pause Media key
        LaunchMail         = 0xB4, ///< Start Mail key
        LaunchMediaSelect  = 0xB5, ///< Select Media key
        LaunchApplication1 = 0xB6, ///< Start Application 1 key
        LaunchApplication2 = 0xB7, ///< Start Application 2 key

        //- 0xB8-B9 Reserved

        OEM_1      = 0xBA, ///< Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the ';:' key
        OEM_Plus   = 0xBB, ///< For any country/region, the '+' key
        OEM_Comma  = 0xBC, ///< For any country/region, the ',' key
        OEM_Minus  = 0xBD, ///< For any country/region, the '-' key
        OEM_Period = 0xBE, ///< For any country/region, the '.' key
        OEM_2      = 0xBF, ///< Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '/?' key
        OEM_3      = 0xC0, ///< Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '`~' key

        //- 0xC1-D7 Reserved
        //- 0xD8-DA Unassigned

        OEM_4 = 0xDB, ///< Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '[{' key
        OEM_5 = 0xDC, ///< Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '\|' key
        OEM_6 = 0xDD, ///< Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the ']}' key
        OEM_7 = 0xDE, ///< Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the 'single-quote/double-quote' key
        OEM_8 = 0xDF, ///< Used for miscellaneous characters; it can vary by keyboard.

        //- 0xE0 Reserved
        // 0xE1 OEM specific

        OEM_102 = 0xE2, ///< The <> keys on the US standard keyboard, or the \\| key on the non-US 102-key keyboard

        // 0xE3-E4 OEM specific

        ProcessKey = 0xE5, ///< IME PROCESS key

        // 0xE6 OEM specific

        Packet = 0xE7, ///< Used to pass Unicode characters as if they were keystrokes. The VK_PACKET key is the low word of a 32-bit Virtual Key value used for non-keyboard input methods. For more information, see Remark in KEYBDINPUT, SendInput, WM_KEYDOWN, and WM_KEYUP

        //- 0xE8 Unassigned
        // 0xE9-F5 OEM specific

        Attn      = 0xF6, ///< Attn key
        CrSel     = 0xF7, ///< CrSel key
        ExSel     = 0xF8, ///< ExSel key
        EraseEOF  = 0xF9, ///< Erase EOF key
        Play      = 0xFA, ///< Play key
        Zoom      = 0xFB, ///< Zoom key
        NONAME    = 0xFC, ///< Reserved
        PA1       = 0xFD, ///< PA1 key
        OEM_Clear = 0xFE, ///< Clear key
    };

    /**
     * @brief 鼠标事件时用于判断按键状态
     */
    enum class MouseKey {
        Ctrl        = MK_CONTROL,  ///< The CTRL key is down.
        MouseLeft   = MK_LBUTTON,  ///< The left mouse button is down.
        MouseMiddle = MK_MBUTTON,  ///< The middle mouse button is down.
        MouseRight  = MK_RBUTTON,  ///< The right mouse button is down.
        Shift       = MK_SHIFT,    ///< The SHIFT key is down.
        MouseX1     = MK_XBUTTON1, ///< The first X button is down.
        MouseX2     = MK_XBUTTON2, ///< The second X button is down.
    };

    /**
     * @brief 标记MouseKey枚举类型支持位运算
     */
    _SW_ENUM_ENABLE_BIT_OPERATIONS(MouseKey);

    /**
     * @brief 表示热键框控件中的辅助按键，可以是一个或多个按键
     */
    enum class HotKeyModifier {
        None  = 0,                       ///< 无按键
        Shift = /*HOTKEYF_SHIFT*/ 0x1,   ///< Alt键
        Ctrl  = /*HOTKEYF_CONTROL*/ 0x2, ///< Ctrl键
        Alt   = /*HOTKEYF_ALT*/ 0x4,     ///< 扩展键
        Ext   = /*HOTKEYF_EXT*/ 0x8,     ///< Shift键
    };

    /**
     * @brief 标记HotKeyModifier枚举类型支持位运算
     */
    _SW_ENUM_ENABLE_BIT_OPERATIONS(HotKeyModifier);
}

// Point.h


namespace sw
{
    /**
     * @brief 表示相对于左上角的点坐标
     */
    struct Point : public IToString<Point>,
                   public IEqualityComparable<Point> {
        /**
         * @brief 横坐标
         */
        double x;

        /**
         * @brief 纵坐标
         */
        double y;

        /**
         * @brief 默认构造函数
         */
        Point() = default;

        /**
         * @brief 构造指定xy值的Point结构体
         */
        Point(double x, double y) noexcept;

        /**
         * @brief 从POINT构造Point结构体
         */
        Point(const POINT &point) noexcept;

        /**
         * @brief 隐式转换POINT
         */
        operator POINT() const noexcept;

        /**
         * @brief 判断两个Point是否相等
         */
        bool Equals(const Point &other) const noexcept;

        /**
         * @brief 获取描述当前对象的字符串
         */
        std::wstring ToString() const;
    };

    // Point应为POD类型
    static_assert(
        std::is_trivial<Point>::value && std::is_standard_layout<Point>::value,
        "Point should be a POD type.");
}

// Property.h


#define _SW_DEFINE_OPERATION_HELPER(NAME, OP)                                                    \
    template <typename T, typename U, typename = void>                                           \
    struct NAME : std::false_type {                                                              \
    };                                                                                           \
    template <typename T, typename U>                                                            \
    struct NAME<T, U, decltype(void(std::declval<T>() OP std::declval<U>()))> : std::true_type { \
        using type = decltype(std::declval<T>() OP std::declval<U>());                           \
    }

#define _SW_DEFINE_UNARY_OPERATION_HELPER(NAME, OP)                         \
    template <typename T, typename = void>                                  \
    struct NAME : std::false_type {                                         \
    };                                                                      \
    template <typename T>                                                   \
    struct NAME<T, decltype(void(OP std::declval<T>()))> : std::true_type { \
        using type = decltype(OP std::declval<T>());                        \
    }

namespace sw
{
    /**
     * 向前声明
     */

    template <typename T, typename TDerived>
    class PropertyBase;

    template <typename T>
    class Property;

    template <typename T>
    class ReadOnlyProperty;

    template <typename T>
    class WriteOnlyProperty;

    /*================================================================================*/

    // SFINAE templates
    _SW_DEFINE_OPERATION_HELPER(_AddOperationHelper, +);
    _SW_DEFINE_OPERATION_HELPER(_SubOperationHelper, -);
    _SW_DEFINE_OPERATION_HELPER(_MulOperationHelper, *);
    _SW_DEFINE_OPERATION_HELPER(_DivOperationHelper, /);
    _SW_DEFINE_OPERATION_HELPER(_ModOperationHelper, %);
    _SW_DEFINE_OPERATION_HELPER(_EqOperationHelper, ==);
    _SW_DEFINE_OPERATION_HELPER(_NeOperationHelper, !=);
    _SW_DEFINE_OPERATION_HELPER(_LtOperationHelper, <);
    _SW_DEFINE_OPERATION_HELPER(_LeOperationHelper, <=);
    _SW_DEFINE_OPERATION_HELPER(_GtOperationHelper, >);
    _SW_DEFINE_OPERATION_HELPER(_GeOperationHelper, >=);
    _SW_DEFINE_OPERATION_HELPER(_BitAndOperationHelper, &);
    _SW_DEFINE_OPERATION_HELPER(_BitOrOperationHelper, |);
    _SW_DEFINE_OPERATION_HELPER(_BitXorOperationHelper, ^);
    _SW_DEFINE_OPERATION_HELPER(_ShlOperationHelper, <<);
    _SW_DEFINE_OPERATION_HELPER(_ShrOperationHelper, >>);
    _SW_DEFINE_OPERATION_HELPER(_LogicAndOperationHelper, &&);
    _SW_DEFINE_OPERATION_HELPER(_LogicOrOperationHelper, ||);
    _SW_DEFINE_UNARY_OPERATION_HELPER(_LogicNotOperationHelper, !);
    _SW_DEFINE_UNARY_OPERATION_HELPER(_BitNotOperationHelper, ~);
    _SW_DEFINE_UNARY_OPERATION_HELPER(_DerefOperationHelper, *);
    // _SW_DEFINE_UNARY_OPERATION_HELPER(_AddrOperationHelper, &);
    _SW_DEFINE_UNARY_OPERATION_HELPER(_UnaryPlusOperationHelper, +);
    _SW_DEFINE_UNARY_OPERATION_HELPER(_UnaryMinusOperationHelper, -);
    // _SW_DEFINE_UNARY_OPERATION_HELPER(_PreIncOperationHelper, ++);
    // _SW_DEFINE_UNARY_OPERATION_HELPER(_PreDecOperationHelper, --);

    /**
     * @brief _IsProperty的实现
     */
    template <typename T>
    struct _IsPropertyImpl {
    private:
        template <typename U, typename V>
        static std::true_type test(const PropertyBase<U, V> *);
        static std::false_type test(...);

    public:
        using type = decltype(test(std::declval<T *>()));
    };

    /**
     * @brief 判断类型是否为属性的辅助模板
     */
    template <typename T>
    struct _IsProperty : _IsPropertyImpl<typename std::decay<T>::type>::type {
    };

    /**
     * @brief 判断类型是否有GetterImpl成员的辅助模板
     */
    template <typename, typename = void>
    struct _HasGetterImpl : std::false_type {
    };

    /**
     * @brief _HasGetterImpl模板特化
     */
    template <typename T>
    struct _HasGetterImpl<
        T, decltype(void(&T::GetterImpl))> : std::true_type {
    };

    /**
     * @brief 判断类型是否有SetterImpl成员的辅助模板
     */
    template <typename, typename = void>
    struct _HasSetterImpl : std::false_type {
    };

    /**
     * @brief _HasSetterImpl模板特化
     */
    template <typename T>
    struct _HasSetterImpl<
        T, decltype(void(&T::SetterImpl))> : std::true_type {
    };

    /**
     * @brief 判断类型是否为可读属性的辅助模板
     */
    template <typename T>
    struct _IsReadableProperty
        : std::integral_constant<bool, _IsProperty<T>::value && _HasGetterImpl<T>::value> {
    };

    /**
     * @brief 判断类型是否为可写属性的辅助模板
     */
    template <typename T>
    struct _IsWritableProperty
        : std::integral_constant<bool, _IsProperty<T>::value && _HasSetterImpl<T>::value> {
    };

    /**
     * @brief 判断类型是否可以使用[]操作符的辅助模板
     */
    template <typename T, typename U, typename = void>
    struct _BracketOperationHelper : std::false_type {
    };

    /**
     * @brief _BracketOperationHelper模板特化
     */
    template <typename T, typename U>
    struct _BracketOperationHelper<
        T, U, decltype(void(std::declval<T>()[std::declval<U>()]))> : std::true_type {
        using type = decltype(std::declval<T>()[std::declval<U>()]);
    };

    /**
     * @brief 判断类型是否有operator->的辅助模板
     */
    template <typename T, typename = void>
    struct _HasArrowOperator : std::false_type {
    };

    /**
     * @brief _HasArrowOperator模板特化
     */
    template <typename T>
    struct _HasArrowOperator<
        T, decltype(void(std::declval<T>().operator->()))> : std::true_type {
        using type = decltype(std::declval<T>().operator->());
    };

    /**
     * @brief 属性setter参数类型
     */
    template <typename T>
    using _PropertySetterParamType = _OptimalParamType<T>;

    /*================================================================================*/

    /**
     * @brief 字段访问器，用于实现使用operator->取属性字段
     */
    template <typename T>
    struct FieldsAccessor {
        /**
         * @brief 字段访问器所维护的值
         */
        T value;

        /**
         * @brief 构造字段访问器
         */
        template <typename... Args>
        FieldsAccessor(Args &&...args)
            : value(std::forward<Args>(args)...)
        {
        }

        /**
         * @brief 指针类型，直接返回值
         */
        template <typename U = T>
        auto operator->()
            -> typename std::enable_if<std::is_pointer<U>::value, U>::type
        {
            return this->value;
        }

        /**
         * @brief 非指针类型，且无operator->，返回值的地址
         */
        template <typename U = T>
        auto operator->()
            -> typename std::enable_if<!std::is_pointer<U>::value && !_HasArrowOperator<U>::value, U *>::type
        {
            return &this->value;
        }

        /**
         * @brief 非指针类型，且有operator->，转发operator->
         */
        template <typename U = T>
        auto operator->()
            -> typename std::enable_if<!std::is_pointer<U>::value && _HasArrowOperator<U>::value, typename _HasArrowOperator<U>::type>::type
        {
            return this->value.operator->();
        }
    };

    /**
     * @brief 成员属性初始化器
     */
    template <typename TOwner, typename TValue>
    class MemberPropertyInitializer
    {
        friend class Property<TValue>;
        friend class ReadOnlyProperty<TValue>;
        friend class WriteOnlyProperty<TValue>;

    private:
        /**
         * @brief 属性所有者
         */
        TOwner *_owner;

        /**
         * @brief getter函数指针
         */
        TValue (*_getter)(TOwner *);

        /**
         * @brief setter函数指针
         */
        void (*_setter)(TOwner *, _PropertySetterParamType<TValue>);

    public:
        /**
         * @brief 构造成员属性初始化器
         */
        MemberPropertyInitializer(TOwner *owner)
            : _owner(owner), _getter(nullptr), _setter(nullptr)
        {
        }

        /**
         * @brief 设置getter
         */
        MemberPropertyInitializer &Getter(TValue (*getter)(TOwner *))
        {
            this->_getter = getter;
            return *this;
        }

        /**
         * @brief 设置setter
         */
        MemberPropertyInitializer &Setter(void (*setter)(TOwner *, _PropertySetterParamType<TValue>))
        {
            this->_setter = setter;
            return *this;
        }

        /**
         * @brief 设置成员函数getter
         */
        template <TValue (TOwner::*getter)()>
        MemberPropertyInitializer &Getter()
        {
            return this->Getter(
                [](TOwner *owner) -> TValue {
                    return (owner->*getter)();
                });
        }

        /**
         * @brief 设置成员函数getter
         */
        template <TValue (TOwner::*getter)() const>
        MemberPropertyInitializer &Getter()
        {
            return this->Getter(
                [](TOwner *owner) -> TValue {
                    return (owner->*getter)();
                });
        }

        /**
         * @brief 设置成员函数setter
         */
        template <void (TOwner::*setter)(_PropertySetterParamType<TValue>)>
        MemberPropertyInitializer &Setter()
        {
            return this->Setter(
                [](TOwner *owner, _PropertySetterParamType<TValue> value) {
                    (owner->*setter)(value);
                });
        }

        /**
         * @brief 设置成员函数setter
         */
        template <void (TOwner::*setter)(_PropertySetterParamType<TValue>) const>
        MemberPropertyInitializer &Setter()
        {
            return this->Setter(
                [](TOwner *owner, _PropertySetterParamType<TValue> value) {
                    (owner->*setter)(value);
                });
        }

        /**
         * @brief 设置简单字段getter
         */
        template <TValue TOwner::*field>
        MemberPropertyInitializer &Getter()
        {
            return this->Getter(
                [](TOwner *owner) -> TValue {
                    return owner->*field;
                });
        }

        /**
         * @brief 设置简单字段setter
         */
        template <TValue TOwner::*field>
        MemberPropertyInitializer &Setter()
        {
            return this->Setter(
                [](TOwner *owner, _PropertySetterParamType<TValue> value) {
                    owner->*field = value;
                });
        }
    };

    /**
     * @brief 静态属性初始化器
     */
    template <typename TValue>
    class StaticPropertyInitializer
    {
        friend class Property<TValue>;
        friend class ReadOnlyProperty<TValue>;
        friend class WriteOnlyProperty<TValue>;

    private:
        /**
         * @brief getter函数指针
         */
        TValue (*_getter)();

        /**
         * @brief setter函数指针
         */
        void (*_setter)(_PropertySetterParamType<TValue>);

    public:
        /**
         * @brief 构造静态属性初始化器
         */
        StaticPropertyInitializer()
            : _getter(nullptr), _setter(nullptr)
        {
        }

        /**
         * @brief 设置getter
         */
        StaticPropertyInitializer &Getter(TValue (*getter)())
        {
            this->_getter = getter;
            return *this;
        }

        /**
         * @brief 设置setter
         */
        StaticPropertyInitializer &Setter(void (*setter)(_PropertySetterParamType<TValue>))
        {
            this->_setter = setter;
            return *this;
        }
    };

    /*================================================================================*/

    /**
     * @brief 属性基类模板
     */
    template <typename T, typename TDerived>
    class PropertyBase
    {
    public:
        // 属性值类型别名
        using TValue = T;

        // setter参数类型别名
        using TSetterParam = _PropertySetterParamType<T>;

        // /**
        //  * @brief 获取属性值，由子类实现
        //  */
        // T GetterImpl() const;

        // /**
        //  * @brief 设置属性值，由子类实现
        //  */
        // void SetterImpl(TSetterParam value) const;

        /**
         * @brief 访问属性字段，可由子类重写
         */
        FieldsAccessor<T> AccessFields() const
        {
            return FieldsAccessor<T>(this->Get());
        }

        /**
         * @brief 获取属性值
         */
        T Get() const
        {
            return static_cast<const TDerived *>(this)->GetterImpl();
        }

        /**
         * @brief 设置属性值
         */
        void Set(TSetterParam value) const
        {
            static_cast<const TDerived *>(this)->SetterImpl(value);
        }

        /**
         * @brief 取属性字段
         */
        auto operator->() const
        {
            return static_cast<const TDerived *>(this)->AccessFields();
        }

        /**
         * @brief 隐式转换
         */
        operator T() const
        {
            return this->Get();
        }

        /**
         * @brief 隐式转换
         */
        template <
            typename U = T,
            typename   = typename std::enable_if<!std::is_arithmetic<T>::value && std::is_convertible<T, U>::value, U>::type>
        operator U() const
        {
            return static_cast<U>(this->Get());
        }

        /**
         * @brief 显式转换
         */
        template <
            typename U = T,
            typename   = typename std::enable_if<!std::is_arithmetic<T>::value && !std::is_convertible<T, U>::value, U>::type,
            typename   = typename std::enable_if<!std::is_arithmetic<T>::value && _IsExplicitlyConvertable<T, U>::value, U>::type>
        explicit operator U() const
        {
            return static_cast<U>(this->Get());
        }

        /**
         * @brief 设置属性值
         */
        TDerived &operator=(TSetterParam value)
        {
            this->Set(value);
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 设置属性值
         */
        const TDerived &operator=(TSetterParam value) const
        {
            this->Set(value);
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 设置属性值
         */
        TDerived &operator=(const PropertyBase &prop)
        {
            this->Set(prop.Get());
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 设置属性值
         */
        const TDerived &operator=(const PropertyBase &prop) const
        {
            this->Set(prop.Get());
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 加赋值运算
         */
        template <typename U>
        auto operator+=(U &&value)
            -> typename std::enable_if<_AddOperationHelper<T, U>::value, TDerived &>::type
        {
            this->Set(this->Get() + std::forward<U>(value));
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 加赋值运算
         */
        template <typename U>
        auto operator+=(U &&value) const
            -> typename std::enable_if<_AddOperationHelper<T, U>::value, const TDerived &>::type
        {
            this->Set(this->Get() + std::forward<U>(value));
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 加赋值运算
         */
        template <typename D, typename U>
        auto operator+=(const PropertyBase<U, D> &prop)
            -> typename std::enable_if<_AddOperationHelper<T, U>::value, TDerived &>::type
        {
            this->Set(this->Get() + prop.Get());
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 加赋值运算
         */
        template <typename D, typename U>
        auto operator+=(const PropertyBase<U, D> &prop) const
            -> typename std::enable_if<_AddOperationHelper<T, U>::value, const TDerived &>::type
        {
            this->Set(this->Get() + prop.Get());
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 减赋值运算
         */
        template <typename U>
        auto operator-=(U &&value)
            -> typename std::enable_if<_SubOperationHelper<T, U>::value, TDerived &>::type
        {
            this->Set(this->Get() - std::forward<U>(value));
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 减赋值运算
         */
        template <typename U>
        auto operator-=(U &&value) const
            -> typename std::enable_if<_SubOperationHelper<T, U>::value, const TDerived &>::type
        {
            this->Set(this->Get() - std::forward<U>(value));
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 减赋值运算
         */
        template <typename D, typename U>
        auto operator-=(const PropertyBase<U, D> &prop)
            -> typename std::enable_if<_SubOperationHelper<T, U>::value, TDerived &>::type
        {
            this->Set(this->Get() - prop.Get());
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 减赋值运算
         */
        template <typename D, typename U>
        auto operator-=(const PropertyBase<U, D> &prop) const
            -> typename std::enable_if<_SubOperationHelper<T, U>::value, const TDerived &>::type
        {
            this->Set(this->Get() - prop.Get());
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 乘赋值运算
         */
        template <typename U>
        auto operator*=(U &&value)
            -> typename std::enable_if<_MulOperationHelper<T, U>::value, TDerived &>::type
        {
            this->Set(this->Get() * std::forward<U>(value));
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 乘赋值运算
         */
        template <typename U>
        auto operator*=(U &&value) const
            -> typename std::enable_if<_MulOperationHelper<T, U>::value, const TDerived &>::type
        {
            this->Set(this->Get() * std::forward<U>(value));
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 乘赋值运算
         */
        template <typename D, typename U>
        auto operator*=(const PropertyBase<U, D> &prop)
            -> typename std::enable_if<_MulOperationHelper<T, U>::value, TDerived &>::type
        {
            this->Set(this->Get() * prop.Get());
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 乘赋值运算
         */
        template <typename D, typename U>
        auto operator*=(const PropertyBase<U, D> &prop) const
            -> typename std::enable_if<_MulOperationHelper<T, U>::value, const TDerived &>::type
        {
            this->Set(this->Get() * prop.Get());
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 除赋值运算
         */
        template <typename U>
        auto operator/=(U &&value)
            -> typename std::enable_if<_DivOperationHelper<T, U>::value, TDerived &>::type
        {
            this->Set(this->Get() / std::forward<U>(value));
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 除赋值运算
         */
        template <typename U>
        auto operator/=(U &&value) const
            -> typename std::enable_if<_DivOperationHelper<T, U>::value, const TDerived &>::type
        {
            this->Set(this->Get() / std::forward<U>(value));
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 除赋值运算
         */
        template <typename D, typename U>
        auto operator/=(const PropertyBase<U, D> &prop)
            -> typename std::enable_if<_DivOperationHelper<T, U>::value, TDerived &>::type
        {
            this->Set(this->Get() / prop.Get());
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 除赋值运算
         */
        template <typename D, typename U>
        auto operator/=(const PropertyBase<U, D> &prop) const
            -> typename std::enable_if<_DivOperationHelper<T, U>::value, const TDerived &>::type
        {
            this->Set(this->Get() / prop.Get());
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 前置自增运算
         */
        template <typename U = T>
        auto operator++()
            -> typename std::enable_if<_AddOperationHelper<U, int>::value, TDerived &>::type
        {
            this->Set(this->Get() + 1);
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 前置自增运算
         */
        template <typename U = T>
        auto operator++() const
            -> typename std::enable_if<_AddOperationHelper<U, int>::value, const TDerived &>::type
        {
            this->Set(this->Get() + 1);
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 前置自减运算
         */
        template <typename U = T>
        auto operator--()
            -> typename std::enable_if<_SubOperationHelper<U, int>::value, TDerived &>::type
        {
            this->Set(this->Get() - 1);
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 前置自减运算
         */
        template <typename U = T>
        auto operator--() const
            -> typename std::enable_if<_SubOperationHelper<U, int>::value, const TDerived &>::type
        {
            this->Set(this->Get() - 1);
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 后置自增运算
         */
        template <typename U = T>
        auto operator++(int) const
            -> typename std::enable_if<_AddOperationHelper<U, int>::value, T>::type
        {
            T oldval = this->Get();
            this->Set(oldval + 1);
            return oldval;
        }

        /**
         * @brief 后置自减运算
         */
        template <typename U = T>
        auto operator--(int) const
            -> typename std::enable_if<_SubOperationHelper<U, int>::value, T>::type
        {
            T oldval = this->Get();
            this->Set(oldval - 1);
            return oldval;
        }

        /**
         * @brief 按位与赋值运算
         */
        template <typename U>
        auto operator&=(U &&value)
            -> typename std::enable_if<_BitAndOperationHelper<T, U>::value, TDerived &>::type
        {
            this->Set(this->Get() & std::forward<U>(value));
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 按位与赋值运算
         */
        template <typename U>
        auto operator&=(U &&value) const
            -> typename std::enable_if<_BitAndOperationHelper<T, U>::value, const TDerived &>::type
        {
            this->Set(this->Get() & std::forward<U>(value));
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 按位与赋值运算
         */
        template <typename D, typename U>
        auto operator&=(const PropertyBase<U, D> &prop)
            -> typename std::enable_if<_BitAndOperationHelper<T, U>::value, TDerived &>::type
        {
            this->Set(this->Get() & prop.Get());
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 按位与赋值运算
         */
        template <typename D, typename U>
        auto operator&=(const PropertyBase<U, D> &prop) const
            -> typename std::enable_if<_BitAndOperationHelper<T, U>::value, const TDerived &>::type
        {
            this->Set(this->Get() & prop.Get());
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 按位或赋值运算
         */
        template <typename U>
        auto operator|=(U &&value)
            -> typename std::enable_if<_BitOrOperationHelper<T, U>::value, TDerived &>::type
        {
            this->Set(this->Get() | std::forward<U>(value));
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 按位或赋值运算
         */
        template <typename U>
        auto operator|=(U &&value) const
            -> typename std::enable_if<_BitOrOperationHelper<T, U>::value, const TDerived &>::type
        {
            this->Set(this->Get() | std::forward<U>(value));
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 按位或赋值运算
         */
        template <typename D, typename U>
        auto operator|=(const PropertyBase<U, D> &prop)
            -> typename std::enable_if<_BitOrOperationHelper<T, U>::value, TDerived &>::type
        {
            this->Set(this->Get() | prop.Get());
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 按位或赋值运算
         */
        template <typename D, typename U>
        auto operator|=(const PropertyBase<U, D> &prop) const
            -> typename std::enable_if<_BitOrOperationHelper<T, U>::value, const TDerived &>::type
        {
            this->Set(this->Get() | prop.Get());
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 按位异或赋值运算
         */
        template <typename U>
        auto operator^=(U &&value)
            -> typename std::enable_if<_BitXorOperationHelper<T, U>::value, TDerived &>::type
        {
            this->Set(this->Get() ^ std::forward<U>(value));
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 按位异或赋值运算
         */
        template <typename U>
        auto operator^=(U &&value) const
            -> typename std::enable_if<_BitXorOperationHelper<T, U>::value, const TDerived &>::type
        {
            this->Set(this->Get() ^ std::forward<U>(value));
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 按位异或赋值运算
         */
        template <typename D, typename U>
        auto operator^=(const PropertyBase<U, D> &prop)
            -> typename std::enable_if<_BitXorOperationHelper<T, U>::value, TDerived &>::type
        {
            this->Set(this->Get() ^ prop.Get());
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 按位异或赋值运算
         */
        template <typename D, typename U>
        auto operator^=(const PropertyBase<U, D> &prop) const
            -> typename std::enable_if<_BitXorOperationHelper<T, U>::value, const TDerived &>::type
        {
            this->Set(this->Get() ^ prop.Get());
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 左移赋值运算
         */
        template <typename U>
        auto operator<<=(U &&value)
            -> typename std::enable_if<_ShlOperationHelper<T, U>::value, TDerived &>::type
        {
            this->Set(this->Get() << std::forward<U>(value));
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 左移赋值运算
         */
        template <typename U>
        auto operator<<=(U &&value) const
            -> typename std::enable_if<_ShlOperationHelper<T, U>::value, const TDerived &>::type
        {
            this->Set(this->Get() << std::forward<U>(value));
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 左移赋值运算
         */
        template <typename D, typename U>
        auto operator<<=(const PropertyBase<U, D> &prop)
            -> typename std::enable_if<_ShlOperationHelper<T, U>::value, TDerived &>::type
        {
            this->Set(this->Get() << prop.Get());
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 左移赋值运算
         */
        template <typename D, typename U>
        auto operator<<=(const PropertyBase<U, D> &prop) const
            -> typename std::enable_if<_ShlOperationHelper<T, U>::value, const TDerived &>::type
        {
            this->Set(this->Get() << prop.Get());
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 右移赋值运算
         */
        template <typename U>
        auto operator>>=(U &&value)
            -> typename std::enable_if<_ShrOperationHelper<T, U>::value, TDerived &>::type
        {
            this->Set(this->Get() >> std::forward<U>(value));
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 右移赋值运算
         */
        template <typename U>
        auto operator>>=(U &&value) const
            -> typename std::enable_if<_ShrOperationHelper<T, U>::value, const TDerived &>::type
        {
            this->Set(this->Get() >> std::forward<U>(value));
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 右移赋值运算
         */
        template <typename D, typename U>
        auto operator>>=(const PropertyBase<U, D> &prop)
            -> typename std::enable_if<_ShrOperationHelper<T, U>::value, TDerived &>::type
        {
            this->Set(this->Get() >> prop.Get());
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 右移赋值运算
         */
        template <typename D, typename U>
        auto operator>>=(const PropertyBase<U, D> &prop) const
            -> typename std::enable_if<_ShrOperationHelper<T, U>::value, const TDerived &>::type
        {
            this->Set(this->Get() >> prop.Get());
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 逻辑非运算
         */
        template <typename U = T>
        auto operator!() const
            -> typename std::enable_if<_LogicNotOperationHelper<U>::value, typename _LogicNotOperationHelper<U>::type>::type
        {
            return !this->Get();
        }

        /**
         * @brief 按位非运算
         */
        template <typename U = T>
        auto operator~() const
            -> typename std::enable_if<_BitNotOperationHelper<U>::value, typename _BitNotOperationHelper<U>::type>::type
        {
            return ~this->Get();
        }

        /**
         * @brief 解引用运算
         * @note 仅在T的operator*返回非引用类型时启用：Get()可能返回临时对象，
         *       若T的operator*返回引用，将产生悬空引用，因此不允许通过属性的
         *       operator*直接访问返回引用的重载，应先将Get()的结果保存到变量后再使用。
         */
        template <typename U = T>
        auto operator*() const
            -> typename std::enable_if<
                _DerefOperationHelper<U>::value &&
                    !std::is_reference<typename _DerefOperationHelper<U>::type>::value,
                typename _DerefOperationHelper<U>::type>::type
        {
            return *this->Get();
        }

        /**
         * @brief 指针解引用运算
         * @note T为指针时，Get()返回的指针虽是临时对象，但其指向的内存独立存在，
         *       故此处即使operator*返回引用也不会产生悬空引用，无需限制返回类型。
         */
        template <typename U = T>
        auto operator*() const
            -> typename std::enable_if<
                _DerefOperationHelper<U>::value && std::is_pointer<T>::value,
                typename _DerefOperationHelper<U>::type>::type
        {
            return *this->Get();
        }

        /**
         * @brief 正号运算
         */
        template <typename U = T>
        auto operator+() const
            -> typename std::enable_if<_UnaryPlusOperationHelper<U>::value, typename _UnaryPlusOperationHelper<U>::type>::type
        {
            return +this->Get();
        }

        /**
         * @brief 负号运算
         */
        template <typename U = T>
        auto operator-() const
            -> typename std::enable_if<_UnaryMinusOperationHelper<U>::value, typename _UnaryMinusOperationHelper<U>::type>::type
        {
            return -this->Get();
        }

        /**
         * @brief 加法运算
         */
        template <typename U>
        auto operator+(U &&value) const
            -> typename std::enable_if<_AddOperationHelper<T, U>::value, typename _AddOperationHelper<T, U>::type>::type
        {
            return this->Get() + std::forward<U>(value);
        }

        /**
         * @brief 加法运算
         */
        template <typename D, typename U>
        auto operator+(const PropertyBase<U, D> &prop) const
            -> typename std::enable_if<_AddOperationHelper<T, U>::value, typename _AddOperationHelper<T, U>::type>::type
        {
            return this->Get() + prop.Get();
        }

        /**
         * @brief 减法运算
         */
        template <typename U>
        auto operator-(U &&value) const
            -> typename std::enable_if<_SubOperationHelper<T, U>::value, typename _SubOperationHelper<T, U>::type>::type
        {
            return this->Get() - std::forward<U>(value);
        }

        /**
         * @brief 减法运算
         */
        template <typename D, typename U>
        auto operator-(const PropertyBase<U, D> &prop) const
            -> typename std::enable_if<_SubOperationHelper<T, U>::value, typename _SubOperationHelper<T, U>::type>::type
        {
            return this->Get() - prop.Get();
        }

        /**
         * @brief 乘法运算
         */
        template <typename U>
        auto operator*(U &&value) const
            -> typename std::enable_if<_MulOperationHelper<T, U>::value, typename _MulOperationHelper<T, U>::type>::type
        {
            return this->Get() * std::forward<U>(value);
        }

        /**
         * @brief 乘法运算
         */
        template <typename D, typename U>
        auto operator*(const PropertyBase<U, D> &prop) const
            -> typename std::enable_if<_MulOperationHelper<T, U>::value, typename _MulOperationHelper<T, U>::type>::type
        {
            return this->Get() * prop.Get();
        }

        /**
         * @brief 除法运算
         */
        template <typename U>
        auto operator/(U &&value) const
            -> typename std::enable_if<_DivOperationHelper<T, U>::value, typename _DivOperationHelper<T, U>::type>::type
        {
            return this->Get() / std::forward<U>(value);
        }

        /**
         * @brief 除法运算
         */
        template <typename D, typename U>
        auto operator/(const PropertyBase<U, D> &prop) const
            -> typename std::enable_if<_DivOperationHelper<T, U>::value, typename _DivOperationHelper<T, U>::type>::type
        {
            return this->Get() / prop.Get();
        }

        /**
         * @brief 取模运算
         */
        template <typename U>
        auto operator%(U &&value) const
            -> typename std::enable_if<_ModOperationHelper<T, U>::value, typename _ModOperationHelper<T, U>::type>::type
        {
            return this->Get() % std::forward<U>(value);
        }

        /**
         * @brief 取模运算
         */
        template <typename D, typename U>
        auto operator%(const PropertyBase<U, D> &prop) const
            -> typename std::enable_if<_ModOperationHelper<T, U>::value, typename _ModOperationHelper<T, U>::type>::type
        {
            return this->Get() % prop.Get();
        }

        /**
         * @brief 等于运算
         */
        template <typename U>
        auto operator==(U &&value) const
            -> typename std::enable_if<_EqOperationHelper<T, U>::value, typename _EqOperationHelper<T, U>::type>::type
        {
            return this->Get() == std::forward<U>(value);
        }

        /**
         * @brief 等于运算
         */
        template <typename D, typename U>
        auto operator==(const PropertyBase<U, D> &prop) const
            -> typename std::enable_if<_EqOperationHelper<T, U>::value, typename _EqOperationHelper<T, U>::type>::type
        {
            return this->Get() == prop.Get();
        }

        /**
         * @brief 不等于运算
         * @note 避免与c++20自动生成的!=冲突，通过==取反实现
         */
        template <typename U>
        auto operator!=(U &&value) const
            -> typename std::enable_if<_EqOperationHelper<T, U>::value, typename _EqOperationHelper<T, U>::type>::type
        {
            return !(*this == std::forward<U>(value));
        }

        /**
         * @brief 不等于运算
         * @note 避免与c++20自动生成的!=冲突，通过==取反实现
         */
        template <typename D, typename U>
        auto operator!=(const PropertyBase<U, D> &prop) const
            -> typename std::enable_if<_EqOperationHelper<T, U>::value, typename _EqOperationHelper<T, U>::type>::type
        {
            return !(*this == prop);
        }

        /**
         * @brief 小于运算
         */
        template <typename U>
        auto operator<(U &&value) const
            -> typename std::enable_if<_LtOperationHelper<T, U>::value, typename _LtOperationHelper<T, U>::type>::type
        {
            return this->Get() < std::forward<U>(value);
        }

        /**
         * @brief 小于运算
         */
        template <typename D, typename U>
        auto operator<(const PropertyBase<U, D> &prop) const
            -> typename std::enable_if<_LtOperationHelper<T, U>::value, typename _LtOperationHelper<T, U>::type>::type
        {
            return this->Get() < prop.Get();
        }

        /**
         * @brief 小于等于运算
         */
        template <typename U>
        auto operator<=(U &&value) const
            -> typename std::enable_if<_LeOperationHelper<T, U>::value, typename _LeOperationHelper<T, U>::type>::type
        {
            return this->Get() <= std::forward<U>(value);
        }

        /**
         * @brief 小于等于运算
         */
        template <typename D, typename U>
        auto operator<=(const PropertyBase<U, D> &prop) const
            -> typename std::enable_if<_LeOperationHelper<T, U>::value, typename _LeOperationHelper<T, U>::type>::type
        {
            return this->Get() <= prop.Get();
        }

        /**
         * @brief 大于运算
         */
        template <typename U>
        auto operator>(U &&value) const
            -> typename std::enable_if<_GtOperationHelper<T, U>::value, typename _GtOperationHelper<T, U>::type>::type
        {
            return this->Get() > std::forward<U>(value);
        }

        /**
         * @brief 大于运算
         */
        template <typename D, typename U>
        auto operator>(const PropertyBase<U, D> &prop) const
            -> typename std::enable_if<_GtOperationHelper<T, U>::value, typename _GtOperationHelper<T, U>::type>::type
        {
            return this->Get() > prop.Get();
        }

        /**
         * @brief 大于等于运算
         */
        template <typename U>
        auto operator>=(U &&value) const
            -> typename std::enable_if<_GeOperationHelper<T, U>::value, typename _GeOperationHelper<T, U>::type>::type
        {
            return this->Get() >= std::forward<U>(value);
        }

        /**
         * @brief 大于等于运算
         */
        template <typename D, typename U>
        auto operator>=(const PropertyBase<U, D> &prop) const
            -> typename std::enable_if<_GeOperationHelper<T, U>::value, typename _GeOperationHelper<T, U>::type>::type
        {
            return this->Get() >= prop.Get();
        }

        /**
         * @brief 按位与运算
         */
        template <typename U>
        auto operator&(U &&value) const
            -> typename std::enable_if<_BitAndOperationHelper<T, U>::value, typename _BitAndOperationHelper<T, U>::type>::type
        {
            return this->Get() & std::forward<U>(value);
        }

        /**
         * @brief 按位与运算
         */
        template <typename D, typename U>
        auto operator&(const PropertyBase<U, D> &prop) const
            -> typename std::enable_if<_BitAndOperationHelper<T, U>::value, typename _BitAndOperationHelper<T, U>::type>::type
        {
            return this->Get() & prop.Get();
        }

        /**
         * @brief 按位或运算
         */
        template <typename U>
        auto operator|(U &&value) const
            -> typename std::enable_if<_BitOrOperationHelper<T, U>::value, typename _BitOrOperationHelper<T, U>::type>::type
        {
            return this->Get() | std::forward<U>(value);
        }

        /**
         * @brief 按位或运算
         */
        template <typename D, typename U>
        auto operator|(const PropertyBase<U, D> &prop) const
            -> typename std::enable_if<_BitOrOperationHelper<T, U>::value, typename _BitOrOperationHelper<T, U>::type>::type
        {
            return this->Get() | prop.Get();
        }

        /**
         * @brief 按位异或运算
         */
        template <typename U>
        auto operator^(U &&value) const
            -> typename std::enable_if<_BitXorOperationHelper<T, U>::value, typename _BitXorOperationHelper<T, U>::type>::type
        {
            return this->Get() ^ std::forward<U>(value);
        }

        /**
         * @brief 按位异或运算
         */
        template <typename D, typename U>
        auto operator^(const PropertyBase<U, D> &prop) const
            -> typename std::enable_if<_BitXorOperationHelper<T, U>::value, typename _BitXorOperationHelper<T, U>::type>::type
        {
            return this->Get() ^ prop.Get();
        }

        /**
         * @brief 左移运算
         */
        template <typename U>
        auto operator<<(U &&value) const
            -> typename std::enable_if<_ShlOperationHelper<T, U>::value, typename _ShlOperationHelper<T, U>::type>::type
        {
            return this->Get() << std::forward<U>(value);
        }

        /**
         * @brief 左移运算
         */
        template <typename D, typename U>
        auto operator<<(const PropertyBase<U, D> &prop) const
            -> typename std::enable_if<_ShlOperationHelper<T, U>::value, typename _ShlOperationHelper<T, U>::type>::type
        {
            return this->Get() << prop.Get();
        }

        /**
         * @brief 右移运算
         */
        template <typename U>
        auto operator>>(U &&value) const
            -> typename std::enable_if<_ShrOperationHelper<T, U>::value, typename _ShrOperationHelper<T, U>::type>::type
        {
            return this->Get() >> std::forward<U>(value);
        }

        /**
         * @brief 右移运算
         */
        template <typename D, typename U>
        auto operator>>(const PropertyBase<U, D> &prop) const
            -> typename std::enable_if<_ShrOperationHelper<T, U>::value, typename _ShrOperationHelper<T, U>::type>::type
        {
            return this->Get() >> prop.Get();
        }

        /**
         * @brief 逻辑与运算
         */
        template <typename U>
        auto operator&&(U &&value) const
            -> typename std::enable_if<_LogicAndOperationHelper<T, U>::value, typename _LogicAndOperationHelper<T, U>::type>::type
        {
            return this->Get() && std::forward<U>(value);
        }

        /**
         * @brief 逻辑与运算
         */
        template <typename D, typename U>
        auto operator&&(const PropertyBase<U, D> &prop) const
            -> typename std::enable_if<_LogicAndOperationHelper<T, U>::value, typename _LogicAndOperationHelper<T, U>::type>::type
        {
            return this->Get() && prop.Get();
        }

        /**
         * @brief 逻辑或运算
         */
        template <typename U>
        auto operator||(U &&value) const
            -> typename std::enable_if<_LogicOrOperationHelper<T, U>::value, typename _LogicOrOperationHelper<T, U>::type>::type
        {
            return this->Get() || std::forward<U>(value);
        }

        /**
         * @brief 逻辑或运算
         */
        template <typename D, typename U>
        auto operator||(const PropertyBase<U, D> &prop) const
            -> typename std::enable_if<_LogicOrOperationHelper<T, U>::value, typename _LogicOrOperationHelper<T, U>::type>::type
        {
            return this->Get() || prop.Get();
        }

        /**
         * @brief 下标运算
         * @note 仅在T的operator[]返回非引用类型时启用：Get()可能返回临时对象，
         *       若T的operator[]返回引用，将产生悬空引用，因此不允许通过属性的
         *       operator[]直接访问返回引用的重载，应先将Get()的结果保存到变量后再使用。
         */
        template <typename U>
        auto operator[](U &&value) const
            -> typename std::enable_if<
                _BracketOperationHelper<T, U>::value &&
                    !std::is_reference<typename _BracketOperationHelper<T, U>::type>::value,
                typename _BracketOperationHelper<T, U>::type>::type
        {
            return this->Get()[std::forward<U>(value)];
        }

        /**
         * @brief 下标运算
         * @note 仅在T的operator[]返回非引用类型时启用：Get()可能返回临时对象，
         *       若T的operator[]返回引用，将产生悬空引用，因此不允许通过属性的
         *       operator[]直接访问返回引用的重载，应先将Get()的结果保存到变量后再使用。
         */
        template <typename D, typename U>
        auto operator[](const PropertyBase<U, D> &prop) const
            -> typename std::enable_if<
                _BracketOperationHelper<T, U>::value &&
                    !std::is_reference<typename _BracketOperationHelper<T, U>::type>::value,
                typename _BracketOperationHelper<T, U>::type>::type
        {
            return this->Get()[prop.Get()];
        }

        /**
         * @brief 指针下标运算
         * @note T为指针时，Get()返回的指针虽是临时对象，但其指向的内存独立存在，
         *       故此处即使operator[]返回引用也不会产生悬空引用，无需限制返回类型。
         */
        template <typename U>
        auto operator[](U &&value) const
            -> typename std::enable_if<
                _BracketOperationHelper<T, U>::value && std::is_pointer<T>::value,
                typename _BracketOperationHelper<T, U>::type>::type
        {
            return this->Get()[std::forward<U>(value)];
        }

        /**
         * @brief 指针下标运算
         * @note T为指针时，Get()返回的指针虽是临时对象，但其指向的内存独立存在，
         *       故此处即使operator[]返回引用也不会产生悬空引用，无需限制返回类型。
         */
        template <typename D, typename U>
        auto operator[](const PropertyBase<U, D> &prop) const
            -> typename std::enable_if<
                _BracketOperationHelper<T, U>::value && std::is_pointer<T>::value,
                typename _BracketOperationHelper<T, U>::type>::type
        {
            return this->Get()[prop.Get()];
        }

    protected:
        /**
         * @brief 用于存储任意签名函数指针的通用类型
         * @note 函数指针类型间通过reinterpret_cast互转再转回原类型不丢失信息（C++标准良定义），
         *       使用统一的函数指针类型作为存储可避免函数指针与void*之间的conditionally-supported转换。
         */
        using TFuncPtr = void (*)();

        /**
         * @brief 静态属性偏移量标记
         */
        static constexpr std::ptrdiff_t _STATICOFFSET =
            (std::numeric_limits<std::ptrdiff_t>::max)();

        /**
         * @brief 所有者对象相对于当前属性对象的偏移量
         */
        std::ptrdiff_t _offset{_STATICOFFSET};

        /**
         * @brief 判断属性是否为静态属性
         */
        bool IsStatic() const noexcept
        {
            return _offset == _STATICOFFSET;
        }

        /**
         * @brief 设置属性所有者对象，nullptr表示静态属性
         */
        void SetOwner(void *owner) noexcept
        {
            if (owner == nullptr) {
                _offset = _STATICOFFSET;
            } else {
                _offset = reinterpret_cast<uint8_t *>(owner) - reinterpret_cast<uint8_t *>(this);
            }
        }

        /**
         * @brief 获取属性所有者对象，当属性为静态属性时返回nullptr
         */
        void *GetOwner() const noexcept
        {
            if (this->IsStatic()) {
                return nullptr;
            } else {
                return const_cast<uint8_t *>(reinterpret_cast<const uint8_t *>(this)) + _offset;
            }
        }

    public:
        /**
         * @brief 获取成员属性初始化器
         */
        template <typename TOwner>
        static auto Init(TOwner *owner)
            -> MemberPropertyInitializer<TOwner, T>
        {
            return MemberPropertyInitializer<TOwner, T>(owner);
        }

        /**
         * @brief 获取静态属性初始化器
         */
        static auto Init()
            -> StaticPropertyInitializer<T>
        {
            return StaticPropertyInitializer<T>();
        }
    };

    /*================================================================================*/

    /**
     * @brief 加法运算
     */
    template <typename D, typename T, typename U>
    auto operator+(T &&left, const PropertyBase<U, D> &right)
        -> typename std::enable_if<!_IsProperty<T>::value && _AddOperationHelper<T, U>::value, typename _AddOperationHelper<T, U>::type>::type
    {
        return std::forward<T>(left) + right.Get();
    }

    /**
     * @brief 减法运算
     */
    template <typename D, typename T, typename U>
    auto operator-(T &&left, const PropertyBase<U, D> &right)
        -> typename std::enable_if<!_IsProperty<T>::value && _SubOperationHelper<T, U>::value, typename _SubOperationHelper<T, U>::type>::type
    {
        return std::forward<T>(left) - right.Get();
    }

    /**
     * @brief 乘法运算
     */
    template <typename D, typename T, typename U>
    auto operator*(T &&left, const PropertyBase<U, D> &right)
        -> typename std::enable_if<!_IsProperty<T>::value && _MulOperationHelper<T, U>::value, typename _MulOperationHelper<T, U>::type>::type
    {
        return std::forward<T>(left) * right.Get();
    }

    /**
     * @brief 除法运算
     */
    template <typename D, typename T, typename U>
    auto operator/(T &&left, const PropertyBase<U, D> &right)
        -> typename std::enable_if<!_IsProperty<T>::value && _DivOperationHelper<T, U>::value, typename _DivOperationHelper<T, U>::type>::type
    {
        return std::forward<T>(left) / right.Get();
    }

    /**
     * @brief 取模运算
     */
    template <typename D, typename T, typename U>
    auto operator%(T &&left, const PropertyBase<U, D> &right)
        -> typename std::enable_if<!_IsProperty<T>::value && _ModOperationHelper<T, U>::value, typename _ModOperationHelper<T, U>::type>::type
    {
        return std::forward<T>(left) % right.Get();
    }

    /**
     * @brief 等于运算
     */
    template <typename D, typename T, typename U>
    auto operator==(T &&left, const PropertyBase<U, D> &right)
        -> typename std::enable_if<!_IsProperty<T>::value && _EqOperationHelper<T, U>::value, typename _EqOperationHelper<T, U>::type>::type
    {
        return std::forward<T>(left) == right.Get();
    }

    /**
     * @brief 不等于运算
     * @note 避免与c++20自动生成的!=冲突，通过==取反实现
     */
    template <typename D, typename T, typename U>
    auto operator!=(T &&left, const PropertyBase<U, D> &right)
        -> typename std::enable_if<!_IsProperty<T>::value && _EqOperationHelper<T, U>::value, typename _EqOperationHelper<T, U>::type>::type
    {
        return !(std::forward<T>(left) == right);
    }

    /**
     * @brief 小于运算
     */
    template <typename D, typename T, typename U>
    auto operator<(T &&left, const PropertyBase<U, D> &right)
        -> typename std::enable_if<!_IsProperty<T>::value && _LtOperationHelper<T, U>::value, typename _LtOperationHelper<T, U>::type>::type
    {
        return std::forward<T>(left) < right.Get();
    }

    /**
     * @brief 小于等于运算
     */
    template <typename D, typename T, typename U>
    auto operator<=(T &&left, const PropertyBase<U, D> &right)
        -> typename std::enable_if<!_IsProperty<T>::value && _LeOperationHelper<T, U>::value, typename _LeOperationHelper<T, U>::type>::type
    {
        return std::forward<T>(left) <= right.Get();
    }

    /**
     * @brief 大于运算
     */
    template <typename D, typename T, typename U>
    auto operator>(T &&left, const PropertyBase<U, D> &right)
        -> typename std::enable_if<!_IsProperty<T>::value && _GtOperationHelper<T, U>::value, typename _GtOperationHelper<T, U>::type>::type
    {
        return std::forward<T>(left) > right.Get();
    }

    /**
     * @brief 大于等于运算
     */
    template <typename D, typename T, typename U>
    auto operator>=(T &&left, const PropertyBase<U, D> &right)
        -> typename std::enable_if<!_IsProperty<T>::value && _GeOperationHelper<T, U>::value, typename _GeOperationHelper<T, U>::type>::type
    {
        return std::forward<T>(left) >= right.Get();
    }

    /**
     * @brief 按位与运算
     */
    template <typename D, typename T, typename U>
    auto operator&(T &&left, const PropertyBase<U, D> &right)
        -> typename std::enable_if<!_IsProperty<T>::value && _BitAndOperationHelper<T, U>::value, typename _BitAndOperationHelper<T, U>::type>::type
    {
        return std::forward<T>(left) & right.Get();
    }

    /**
     * @brief 按位或运算
     */
    template <typename D, typename T, typename U>
    auto operator|(T &&left, const PropertyBase<U, D> &right)
        -> typename std::enable_if<!_IsProperty<T>::value && _BitOrOperationHelper<T, U>::value, typename _BitOrOperationHelper<T, U>::type>::type
    {
        return std::forward<T>(left) | right.Get();
    }

    /**
     * @brief 按位异或运算
     */
    template <typename D, typename T, typename U>
    auto operator^(T &&left, const PropertyBase<U, D> &right)
        -> typename std::enable_if<!_IsProperty<T>::value && _BitXorOperationHelper<T, U>::value, typename _BitXorOperationHelper<T, U>::type>::type
    {
        return std::forward<T>(left) ^ right.Get();
    }

    /**
     * @brief 左移运算
     */
    template <typename D, typename T, typename U>
    auto operator<<(T &&left, const PropertyBase<U, D> &right)
        -> typename std::enable_if<!_IsProperty<T>::value && _ShlOperationHelper<T, U>::value, typename _ShlOperationHelper<T, U>::type>::type
    {
        return std::forward<T>(left) << right.Get();
    }

    /**
     * @brief 右移运算
     */
    template <typename D, typename T, typename U>
    auto operator>>(T &&left, const PropertyBase<U, D> &right)
        -> typename std::enable_if<!_IsProperty<T>::value && _ShrOperationHelper<T, U>::value, typename _ShrOperationHelper<T, U>::type>::type
    {
        return std::forward<T>(left) >> right.Get();
    }

    /**
     * @brief 逻辑与运算
     */
    template <typename D, typename T, typename U>
    auto operator&&(T &&left, const PropertyBase<U, D> &right)
        -> typename std::enable_if<!_IsProperty<T>::value && _LogicAndOperationHelper<T, U>::value, typename _LogicAndOperationHelper<T, U>::type>::type
    {
        return std::forward<T>(left) && right.Get();
    }

    /**
     * @brief 逻辑或运算
     */
    template <typename D, typename T, typename U>
    auto operator||(T &&left, const PropertyBase<U, D> &right)
        -> typename std::enable_if<!_IsProperty<T>::value && _LogicOrOperationHelper<T, U>::value, typename _LogicOrOperationHelper<T, U>::type>::type
    {
        return std::forward<T>(left) || right.Get();
    }

    /*================================================================================*/

    /**
     * @brief 属性
     */
    template <typename T>
    class Property : public PropertyBase<T, Property<T>>
    {
    public:
        using TBase         = PropertyBase<T, Property<T>>;
        using TValue        = typename TBase::TValue;
        using TSetterParam  = typename TBase::TSetterParam;
        using TFuncPtr      = typename TBase::TFuncPtr;
        using TGetter       = T (*)(void *);
        using TSetter       = void (*)(void *, TSetterParam);
        using TStaticGetter = T (*)();
        using TStaticSetter = void (*)(TSetterParam);

    private:
        /**
         * @brief getter函数指针
         */
        TFuncPtr _getter;

        /**
         * @brief setter函数指针
         */
        TFuncPtr _setter;

    public:
        /**
         * @brief 继承父类operator=
         */
        using TBase::operator=;

        /**
         * @brief 构造成员属性
         */
        template <typename TOwner>
        explicit Property(const MemberPropertyInitializer<TOwner, T> &initializer)
        {
            assert(initializer._owner != nullptr);
            assert(initializer._getter != nullptr);
            assert(initializer._setter != nullptr);

            this->SetOwner(initializer._owner);
            this->_getter = reinterpret_cast<TFuncPtr>(initializer._getter);
            this->_setter = reinterpret_cast<TFuncPtr>(initializer._setter);
        }

        /**
         * @brief 构造静态属性
         */
        explicit Property(const StaticPropertyInitializer<T> &initializer)
        {
            assert(initializer._getter != nullptr);
            assert(initializer._setter != nullptr);

            this->SetOwner(nullptr);
            this->_getter = reinterpret_cast<TFuncPtr>(initializer._getter);
            this->_setter = reinterpret_cast<TFuncPtr>(initializer._setter);
        }

        /**
         * @brief 获取属性值
         */
        T GetterImpl() const
        {
            if (this->IsStatic()) {
                return reinterpret_cast<TStaticGetter>(this->_getter)();
            } else {
                return reinterpret_cast<TGetter>(this->_getter)(this->GetOwner());
            }
        }

        /**
         * @brief 设置属性值
         */
        void SetterImpl(TSetterParam value) const
        {
            if (this->IsStatic()) {
                reinterpret_cast<TStaticSetter>(this->_setter)(value);
            } else {
                reinterpret_cast<TSetter>(this->_setter)(this->GetOwner(), value);
            }
        }
    };

    /**
     * @brief 只读属性
     */
    template <typename T>
    class ReadOnlyProperty : public PropertyBase<T, ReadOnlyProperty<T>>
    {
    public:
        using TBase         = PropertyBase<T, ReadOnlyProperty<T>>;
        using TValue        = typename TBase::TValue;
        using TSetterParam  = typename TBase::TSetterParam;
        using TFuncPtr      = typename TBase::TFuncPtr;
        using TGetter       = T (*)(void *);
        using TStaticGetter = T (*)();

    private:
        /**
         * @brief getter函数指针
         */
        TFuncPtr _getter;

    public:
        /**
         * @brief 构造成员属性
         */
        template <typename TOwner>
        explicit ReadOnlyProperty(const MemberPropertyInitializer<TOwner, T> &initializer)
        {
            assert(initializer._owner != nullptr);
            assert(initializer._getter != nullptr);

            this->SetOwner(initializer._owner);
            this->_getter = reinterpret_cast<TFuncPtr>(initializer._getter);
        }

        /**
         * @brief 构造静态属性
         */
        explicit ReadOnlyProperty(const StaticPropertyInitializer<T> &initializer)
        {
            assert(initializer._getter != nullptr);

            this->SetOwner(nullptr);
            this->_getter = reinterpret_cast<TFuncPtr>(initializer._getter);
        }

        /**
         * @brief 获取属性值
         */
        T GetterImpl() const
        {
            if (this->IsStatic()) {
                return reinterpret_cast<TStaticGetter>(this->_getter)();
            } else {
                return reinterpret_cast<TGetter>(this->_getter)(this->GetOwner());
            }
        }
    };

    /**
     * @brief 只写属性
     */
    template <typename T>
    class WriteOnlyProperty : public PropertyBase<T, WriteOnlyProperty<T>>
    {
    public:
        using TBase         = PropertyBase<T, WriteOnlyProperty<T>>;
        using TValue        = typename TBase::TValue;
        using TSetterParam  = typename TBase::TSetterParam;
        using TFuncPtr      = typename TBase::TFuncPtr;
        using TSetter       = void (*)(void *, TSetterParam);
        using TStaticSetter = void (*)(TSetterParam);

    private:
        /**
         * @brief setter函数指针
         */
        TFuncPtr _setter;

    public:
        /**
         * @brief 继承父类operator=
         */
        using TBase::operator=;

        /**
         * @brief 构造成员属性
         */
        template <typename TOwner>
        explicit WriteOnlyProperty(const MemberPropertyInitializer<TOwner, T> &initializer)
        {
            assert(initializer._owner != nullptr);
            assert(initializer._setter != nullptr);

            this->SetOwner(initializer._owner);
            this->_setter = reinterpret_cast<TFuncPtr>(initializer._setter);
        }

        /**
         * @brief 构造静态属性
         */
        explicit WriteOnlyProperty(const StaticPropertyInitializer<T> &initializer)
        {
            assert(initializer._setter != nullptr);

            this->SetOwner(nullptr);
            this->_setter = reinterpret_cast<TFuncPtr>(initializer._setter);
        }

        /**
         * @brief 设置属性值
         */
        void SetterImpl(TSetterParam value) const
        {
            if (this->IsStatic()) {
                reinterpret_cast<TStaticSetter>(this->_setter)(value);
            } else {
                reinterpret_cast<TSetter>(this->_setter)(this->GetOwner(), value);
            }
        }
    };
}

/*================================================================================*/

/**
 * 将常用类型的属性类声明为外部模板实例，以减少编译时间
 */


#define _SW_DECLARE_EXTERN_PROPERTY_TEMPLATE(T)    \
    extern template class sw::Property<T>;         \
    extern template class sw::ReadOnlyProperty<T>; \
    extern template class sw::WriteOnlyProperty<T>

#define _SW_DEFINE_EXTERN_PROPERTY_TEMPLATE(T) \
    template class sw::Property<T>;            \
    template class sw::ReadOnlyProperty<T>;    \
    template class sw::WriteOnlyProperty<T>

_SW_DECLARE_EXTERN_PROPERTY_TEMPLATE(bool);
_SW_DECLARE_EXTERN_PROPERTY_TEMPLATE(float);
_SW_DECLARE_EXTERN_PROPERTY_TEMPLATE(double);
_SW_DECLARE_EXTERN_PROPERTY_TEMPLATE(int8_t);
_SW_DECLARE_EXTERN_PROPERTY_TEMPLATE(int16_t);
_SW_DECLARE_EXTERN_PROPERTY_TEMPLATE(int32_t);
_SW_DECLARE_EXTERN_PROPERTY_TEMPLATE(int64_t);
_SW_DECLARE_EXTERN_PROPERTY_TEMPLATE(uint8_t);
_SW_DECLARE_EXTERN_PROPERTY_TEMPLATE(uint16_t);
_SW_DECLARE_EXTERN_PROPERTY_TEMPLATE(uint32_t);
_SW_DECLARE_EXTERN_PROPERTY_TEMPLATE(uint64_t);
_SW_DECLARE_EXTERN_PROPERTY_TEMPLATE(std::string);
_SW_DECLARE_EXTERN_PROPERTY_TEMPLATE(std::wstring);

// Size.h


namespace sw
{
    /**
     * @brief 尺寸
     */
    struct Size : public IToString<Size>,
                  public IEqualityComparable<Size> {
        /**
         * @brief 宽度
         */
        double width;

        /**
         * @brief 高度
         */
        double height;

        /**
         * @brief 默认构造函数
         */
        Size() = default;

        /**
         * @brief 构造指定宽高的Size结构体
         */
        Size(double width, double height) noexcept;

        /**
         * @brief 从SIZE构造Size结构体
         */
        Size(const SIZE &size) noexcept;

        /**
         * @brief 隐式转换SIZE
         */
        operator SIZE() const noexcept;

        /**
         * @brief 判断两个Size是否相等
         */
        bool Equals(const Size &other) const noexcept;

        /**
         * @brief 获取描述当前对象的字符串
         */
        std::wstring ToString() const;
    };

    // Size应为POD类型
    static_assert(
        std::is_trivial<Size>::value && std::is_standard_layout<Size>::value,
        "Size should be a POD type.");
}

// Thickness.h


namespace sw
{
    /**
     * @brief 表示矩形区域周围边框的厚度
     */
    struct Thickness : public IToString<Thickness>,
                       public IEqualityComparable<Thickness> {
        /**
         * @brief 左边
         */
        double left;

        /**
         * @brief 顶边
         */
        double top;

        /**
         * @brief 右边
         */
        double right;

        /**
         * @brief 底边
         */
        double bottom;

        /**
         * @brief 默认构造函数
         */
        Thickness() = default;

        /**
         * @brief 构造一个四边都相同的Thickness结构体
         */
        Thickness(double thickness) noexcept;

        /**
         * @brief 指定横向和纵向值构造Thickness结构体
         */
        Thickness(double horizontal, double vertical) noexcept;

        /**
         * @brief 指定四边的值构造Thickness结构体
         */
        Thickness(double left, double top, double right, double bottom) noexcept;

        /**
         * @brief 从RECT结构体构造Thickness结构体
         * @note 此处RECT被解读为四个独立的边距值（left/top/right/bottom），
         *       而非角坐标矩形；这与Rect(const RECT&)按角坐标计算width/height
         *       的语义不同，故标记为explicit以避免误用
         */
        explicit Thickness(const RECT &rect) noexcept;

        /**
         * @brief 显式转换为RECT（与explicit Thickness(const RECT&)对称）
         */
        explicit operator RECT() const noexcept;

        /**
         * @brief 判断两个Thickness是否相同
         */
        bool Equals(const Thickness &other) const noexcept;

        /**
         * @brief 获取描述当前对象的字符串
         */
        std::wstring ToString() const;
    };

    // Thickness应为POD类型
    static_assert(
        std::is_trivial<Thickness>::value && std::is_standard_layout<Thickness>::value,
        "Thickness should be a POD type.");
}

// App.h


namespace sw
{
    /**
     * @brief 线程退出消息循环的方式
     */
    enum class AppQuitMode {
        Auto,   ///< 线程中所有窗口都销毁时自动退出消息循环
        Manual, ///< 需手动调用QuitMsgLoop以退出消息循环
    };

    /**
     * @brief App类
     */
    class App
    {
    private:
        /**
         * @brief 静态类，禁止实例化
         */
        App() = delete;

    public:
        /**
         * @brief 当前线程消息循环中处理空句柄消息的事件
         * @note 该事件是线程局部的，每个线程有独立的处理函数列表
         */
        static const Event<Action<MSG &>> NullHwndMsgHandler;

        /**
         * @brief 应用程序的当前实例的句柄
         */
        static const ReadOnlyProperty<HINSTANCE> Instance;

        /**
         * @brief 当前exe的文件路径
         */
        static const ReadOnlyProperty<std::wstring> ExePath;

        /**
         * @brief 当前exe所在的文件夹路径
         */
        static const ReadOnlyProperty<std::wstring> ExeDirectory;

        /**
         * @brief 当前工作路径
         */
        static const Property<std::wstring> CurrentDirectory;

        /**
         * @brief 当前线程退出消息循环的方式
         * @note 该属性是线程局部的，每个线程有各自独立的值
         */
        static const Property<AppQuitMode> QuitMode;

        /**
         * @brief 消息循环
         * @return 退出代码
         */
        static int MsgLoop();

        /**
         * @brief 退出当前线程的消息循环
         * @param exitCode 退出代码
         */
        static void QuitMsgLoop(int exitCode = 0);
    };
}

// ColorDialog.h


namespace sw
{
    /**
     * @brief https://learn.microsoft.com/en-us/windows/win32/api/commdlg/ns-commdlg-choosecolora-r1
     */
    enum class ColorDialogFlags : DWORD {
        /// Causes the dialog box to display all available colors in the set of basic colors.
        AnyColor = 0x00000100,

        /// Enables the hook procedure specified in the lpfnHook member of this structure.
        /// This flag is used only to initialize the dialog box.
        EnableHook = 0x00000010,

        /// The hInstance and lpTemplateName members specify a dialog box template to use in
        /// place of the default template. This flag is used only to initialize the dialog box.
        EnableTemplate = 0x00000020,

        /// The hInstance member identifies a data block that contains a preloaded dialog box
        /// template. The system ignores the lpTemplateName member if this flag is specified.
        /// This flag is used only to initialize the dialog box.
        EnableTemplateHandle = 0x00000040,

        /// Causes the dialog box to display the additional controls that allow the user to
        /// create custom colors. If this flag is not set, the user must click the Define Custom
        /// Color button to display the custom color controls.
        FullOpen = 0x00000002,

        /// Disables the Define Custom Color button.
        PreventFullOpen = 0x00000004,

        /// Causes the dialog box to use the color specified in the rgbResult member as the
        /// initial color selection.
        RgbInit = 0x00000001,

        /// Causes the dialog box to display the Help button. The hwndOwner member must specify
        /// the window to receive the HELPMSGSTRING registered messages that the dialog box sends
        /// when the user clicks the Help button.
        ShowHelp = 0x00000008,

        /// Causes the dialog box to display only solid colors in the set of basic colors.
        SolidColor = 0x00000080,
    };

    /**
     * @brief 标记ColorDialogFlags枚举支持位运算
     */
    _SW_ENUM_ENABLE_BIT_OPERATIONS(ColorDialogFlags);

    /**
     * @brief 颜色选择对话框
     */
    class ColorDialog : public IDialog
    {
    private:
        /**
         * @brief 颜色选择对话框的配置结构体
         */
        CHOOSECOLORW _cc{};

    public:
        /**
         * @brief 对话框标志
         */
        const Property<ColorDialogFlags> Flags;

        /**
         * @brief 选择的颜色，默认值为黑色
         */
        const Property<Color> SelectedColor;

        /**
         * @brief 是否显示完整的颜色选择界面
         */
        const Property<bool> FullOpen;

        /**
         * @brief 自定义颜色数组，包含16个COLORREF元素
         * @note 默认使用一个全局的自定义颜色数组，若需要自定义请在ShowDialog前修改该数组内容
         */
        const Property<COLORREF *> CustomColors;

    public:
        /**
         * @brief 初始化ColorDialog
         */
        ColorDialog();

        /**
         * @brief ColorDialog默认不支持该函数，调用该函数不会执行任何操作
         */
        virtual void Close() override;

        /**
         * @brief ColorDialog默认不支持该函数，调用该函数不会执行任何操作
         */
        virtual void Show() override;

        /**
         * @brief 显示对话框，并指定所有者窗口
         * @return 若用户选择了颜色则返回true，否则返回false
         */
        virtual int ShowDialog(Window *owner = nullptr) override;

        /**
         * @brief 显示对话框，并指定所有者窗口
         * @return 若用户选择了颜色则返回true，否则返回false
         */
        virtual int ShowDialog(Window &owner);

    protected:
        /**
         * @brief 获取颜色选择对话框的配置结构体指针
         */
        CHOOSECOLORW *GetChooseColorStruct();
    };
}

// Dip.h


namespace sw
{
    /**
     * @brief 用于处理设备独立像素（dip）与屏幕像素之间的转换
     * @note 该类的所有成员均为线程局部的，即缩放比例在每个线程中独立
     */
    class Dip
    {
    private:
        /**
         * @brief 静态类，禁止实例化
         */
        Dip() = delete;

    public:
        /**
         * @brief 水平缩放比例
         */
        static const ReadOnlyProperty<double> ScaleX;

        /**
         * @brief 垂直缩放比例
         */
        static const ReadOnlyProperty<double> ScaleY;

        /**
         * @brief dpi改变时调用该函数更新缩放比例
         */
        static void Update(int dpiX, int dpiY) noexcept;

        /**
         * @brief 像素转dip（水平方向）
         */
        static double PxToDipX(int px) noexcept;

        /**
         * @brief 像素转dip（垂直方向）
         */
        static double PxToDipY(int px) noexcept;

        /**
         * @brief dip转像素（水平方向）
         */
        static int DipToPxX(double dip) noexcept;

        /**
         * @brief dip转像素（垂直方向）
         */
        static int DipToPxY(double dip) noexcept;
    };
}

// FolderDialog.h


namespace sw
{
    /**
     * @brief https://learn.microsoft.com/en-us/windows/win32/api/shlobj_core/ns-shlobj_core-browseinfoa
     */
    enum class FolderDialogFlags : UINT {
        /// Only return file system directories. If the user selects folders that are not part of the
        /// file system, the OK button is grayed.
        /// Note  The OK button remains enabled for "\\server" items, as well as "\\server\share" and
        /// directory items. However, if the user selects a "\\server" item, passing the PIDL returned
        /// by SHBrowseForFolder to SHGetPathFromIDList fails.
        ReturnOnlyFileSystemDirs = 0x00000001,

        /// Do not include network folders below the domain level in the dialog box's tree view control.
        DontGoBelowDomain = 0x00000002,

        /// Include a status area in the dialog box. The callback function can set the status text by
        /// sending messages to the dialog box. This flag is not supported when BIF_NEWDIALOGSTYLE is
        /// specified.
        StatusText = 0x00000004,

        /// Only return file system ancestors. An ancestor is a subfolder that is beneath the root folder
        /// in the namespace hierarchy. If the user selects an ancestor of the root folder that is not
        /// part of the file system, the OK button is grayed.
        ReturnFileSystemAncestors = 0x00000008,

        /// Version 4.71. Include an edit control in the browse dialog box that allows the user to type
        /// the name of an item.
        EditBox = 0x00000010,

        /// Version 4.71. If the user types an invalid name into the edit box, the browse dialog box calls
        /// the application's BrowseCallbackProc with the BFFM_VALIDATEFAILED message. This flag is ignored
        /// if BIF_EDITBOX is not specified.
        Validate = 0x00000020,

        /// Version 5.0. Use the new user interface. Setting this flag provides the user with a larger
        /// dialog box that can be resized. The dialog box has several new capabilities, including:
        /// drag-and-drop capability within the dialog box, reordering, shortcut menus, new folders,
        /// delete, and other shortcut menu commands.
        /// Note  If COM is initialized through CoInitializeEx with the COINIT_MULTITHREADED flag set,
        /// SHBrowseForFolder fails if BIF_NEWDIALOGSTYLE is passed.
        NewDialogStyle = 0x00000040,

        /// Version 5.0. The browse dialog box can display URLs. The BIF_USENEWUI and BIF_BROWSEINCLUDEFILES
        /// flags must also be set. If any of these three flags are not set, the browser dialog box rejects
        /// URLs. Even when these flags are set, the browse dialog box displays URLs only if the folder that
        /// contains the selected item supports URLs. When the folder's IShellFolder::GetAttributesOf method
        /// is called to request the selected item's attributes, the folder must set the SFGAO_FOLDER attribute
        /// flag. Otherwise, the browse dialog box will not display the URL.
        BrowseIncludeUrls = 0x00000080,

        /// Version 5.0. Use the new user interface, including an edit box. This flag is equivalent to
        /// BIF_EDITBOX | BIF_NEWDIALOGSTYLE.
        /// Note  If COM is initialized through CoInitializeEx with the COINIT_MULTITHREADED flag set,
        /// SHBrowseForFolder fails if BIF_USENEWUI is passed.
        UseNewUI = 0x00000010 | 0x00000040,

        /// Version 6.0. When combined with BIF_NEWDIALOGSTYLE, adds a usage hint to the dialog box, in place
        /// of the edit box. BIF_EDITBOX overrides this flag.
        UsageHint = 0x00000100,

        /// Version 6.0. Do not include the New Folder button in the browse dialog box.
        NoNewFolderButton = 0x00000200,

        /// Version 6.0. When the selected item is a shortcut, return the PIDL of the shortcut itself rather
        /// than its target.
        NoTranslateTargets = 0x00000400,

        /// Only return computers. If the user selects anything other than a computer, the OK button is grayed.
        BrowseForComputer = 0x00001000,

        /// Only allow the selection of printers. If the user selects anything other than a printer, the OK
        /// button is grayed.
        /// In Windows XP and later systems, the best practice is to use a Windows XP-style dialog, setting
        /// the root of the dialog to the Printers and Faxes folder (CSIDL_PRINTERS).
        BrowseForPrinter = 0x00002000,

        /// Version 4.71. The browse dialog box displays files as well as folders.
        BrowseIncludeFiles = 0x00004000,

        /// Version 5.0. The browse dialog box can display sharable resources on remote systems. This is intended
        /// for applications that want to expose remote shares on a local system. The BIF_NEWDIALOGSTYLE flag must
        /// also be set.
        Sharable = 0x00008000,

        /// Windows 7 and later. Allow folder junctions such as a library or a compressed file with a .zip file
        /// name extension to be browsed.
        BrowseFileJunctions = 0x00010000,
    };

    /**
     * @brief 标记FolderDialogFlags枚举支持位运算
     */
    _SW_ENUM_ENABLE_BIT_OPERATIONS(FolderDialogFlags);

    /**
     * @brief 选择文件夹对话框
     */
    class FolderBrowserDialog : public IDialog
    {
    private:
        /**
         * @brief BROWSEINFOW结构体
         */
        BROWSEINFOW _bi{};

        /**
         * @brief 储存文件名的缓冲区
         */
        std::vector<wchar_t> _buffer;

        /**
         * @brief 对话框上方显示的描述性文本
         */
        std::wstring _description;

    public:
        /**
         * @brief 储存文件名的缓冲区大小，值不能小于MAX_PATH
         */
        const Property<int> BufferSize;

        /**
         * @brief 对话框标志
         */
        const Property<FolderDialogFlags> Flags;

        /**
         * @brief 对话框上方显示的描述性文本
         */
        const Property<std::wstring> Description;

        /**
         * @brief 选中文件夹的路径
         */
        const ReadOnlyProperty<std::wstring> SelectedPath;

        /**
         * @brief 是否显示“新建文件夹”按钮
         */
        const Property<bool> NewFolderButton;

    public:
        /**
         * @brief 初始化FolderBrowserDialog
         */
        FolderBrowserDialog();

        /**
         * @brief FolderBrowserDialog默认不支持该函数，调用该函数不会执行任何操作
         */
        virtual void Close() override;

        /**
         * @brief FolderBrowserDialog默认不支持该函数，调用该函数不会执行任何操作
         */
        virtual void Show() override;

        /**
         * @brief 显示对话框，并指定所有者窗口
         * @return 若用户选择了文件夹则返回true，否则返回false
         */
        virtual int ShowDialog(Window *owner = nullptr) override;

        /**
         * @brief 显示对话框，并指定所有者窗口
         * @return 若用户选择了文件夹则返回true，否则返回false
         */
        virtual int ShowDialog(Window &owner);

    protected:
        /**
         * @brief 获取BROWSEINFOW指针
         */
        BROWSEINFOW *GetBI();

        /**
         * @brief 获取指向缓冲区的指针
         */
        wchar_t *GetBuffer();

        /**
         * @brief 清空缓冲区
         */
        void ClearBuffer();
    };
}

// FontDialog.h


namespace sw
{
    /**
     * @brief https://learn.microsoft.com/en-us/windows/win32/api/commdlg/ns-commdlg-choosefonta
     */
    enum class FontDialogFlags : DWORD {
        /// Causes the dialog box to display the Apply button. You should provide a hook procedure to process
        /// WM_COMMAND messages for the Apply button. The hook procedure can send the WM_CHOOSEFONT_GETLOGFONT
        /// message to the dialog box to retrieve the address of the structure that contains the current selections
        /// for the font.
        Apply = 0x00000200,

        /// This flag is obsolete. To limit font selections to all scripts except those that use the OEM or Symbol
        /// character sets, use CF_SCRIPTSONLY. To get the original CF_ANSIONLY behavior, use CF_SELECTSCRIPT and
        /// specify ANSI_CHARSET in the lfCharSet member of the LOGFONT structure pointed to by lpLogFont.
        ANSIOnly = 0x00000400,

        /// This flag is ignored for font enumeration.
        /// Windows Vista and Windows XP/2000:  Causes the dialog box to list the available printer and screen fonts.
        /// The hDC member is a handle to the device context or information context associated with the printer. This
        /// flag is a combination of the CF_SCREENFONTS and CF_PRINTERFONTS flags.
        Both = 0x00000003,

        /// Causes the dialog box to display the controls that allow the user to specify strikeout, underline, and
        /// text color options. If this flag is set, you can use the rgbColors member to specify the initial text color.
        /// You can use the lfStrikeOut and lfUnderline members of the structure pointed to by lpLogFont to specify the
        /// initial settings of the strikeout and underline check boxes. ChooseFont can use these members to return the
        /// user's selections.
        Effects = 0x00000100,

        /// Enables the hook procedure specified in the lpfnHook member of this structure.
        EnableHook = 0x00000008,

        /// Indicates that the hInstance and lpTemplateName members specify a dialog box template to use in place of
        /// the default template.
        EnableTemplate = 0x00000010,

        /// Indicates that the hInstance member identifies a data block that contains a preloaded dialog box template.
        /// The system ignores the lpTemplateName member if this flag is specified.
        EnableTemplateHandle = 0x00000020,

        /// ChooseFont should enumerate and allow selection of only fixed-pitch fonts.
        FixedPitchOnly = 0x00004000,

        /// ChooseFont should indicate an error condition if the user attempts to select a font or style that is not
        /// listed in the dialog box.
        ForceFontExist = 0x00010000,

        /// ChooseFont should additionally display fonts that are set to Hide in Fonts Control Panel.
        /// Windows Vista and Windows XP/2000:  This flag is not supported until Windows 7.
        InavtiveFonts = 0x02000000,

        /// ChooseFont should use the structure pointed to by the lpLogFont member to initialize the dialog box controls.
        InitToLogFontStruct = 0x00000040,

        /// ChooseFont should select only font sizes within the range specified by the nSizeMin and nSizeMax members.
        LimitSize = 0x00002000,

        /// Same as the CF_NOVECTORFONTS flag.
        NoOemFonts = 0x00000800,

        /// When using a LOGFONT structure to initialize the dialog box controls, use this flag to prevent the dialog
        /// box from displaying an initial selection for the font name combo box. This is useful when there is no single
        /// font name that applies to the text selection.
        NoFaceSel = 0x00080000,

        /// Disables the Script combo box. When this flag is set, the lfCharSet member of the LOGFONT structure is set
        /// to DEFAULT_CHARSET when ChooseFont returns. This flag is used only to initialize the dialog box.
        NoScriptSel = 0x00800000,

        /// ChooseFont should not display or allow selection of font simulations.
        NoSimulations = 0x00001000,

        /// When using a structure to initialize the dialog box controls, use this flag to prevent the dialog box from
        /// displaying an initial selection for the Font Size combo box. This is useful when there is no single font size
        /// that applies to the text selection.
        NoSizeSel = 0x00200000,

        /// When using a LOGFONT structure to initialize the dialog box controls, use this flag to prevent the dialog box
        /// from displaying an initial selection for the Font Style combo box. This is useful when there is no single font
        /// style that applies to the text selection.
        NoStyleSel = 0x00100000,

        /// ChooseFont should not allow vector font selections.
        NoVectorFonts = 0x00000800,

        /// Causes the Font dialog box to list only horizontally oriented fonts.
        NoVertFonts = 0x01000000,

        /// This flag is ignored for font enumeration.
        /// Windows Vista and Windows XP/2000:  Causes the dialog box to list only the fonts supported by the printer
        /// associated with the device context or information context identified by the hDC member. It also causes the font
        /// type description label to appear at the bottom of the Font dialog box.
        PrinterFonts = 0x00000002,

        /// Specifies that ChooseFont should allow only the selection of scalable fonts. Scalable fonts include vector fonts,
        /// scalable printer fonts, TrueType fonts, and fonts scaled by other technologies.
        ScalableOnly = 0x00020000,

        /// This flag is ignored for font enumeration.
        /// Windows Vista and Windows XP/2000:  Causes the dialog box to list only the screen fonts supported by the system.
        ScreenFonts = 0x00000001,

        /// ChooseFont should allow selection of fonts for all non-OEM and Symbol character sets, as well as the ANSI character
        /// set. This supersedes the CF_ANSIONLY value.
        ScriptsOnly = 0x00000400,

        /// When specified on input, only fonts with the character set identified in the lfCharSet member of the LOGFONT structure
        /// are displayed. The user will not be allowed to change the character set specified in the Scripts combo box.
        SelectScript = 0x00400000,

        /// Causes the dialog box to display the Help button. The hwndOwner member must specify the window to receive the
        /// HELPMSGSTRING registered messages that the dialog box sends when the user clicks the Help button.
        ShowHelp = 0x00000004,

        /// ChooseFont should only enumerate and allow the selection of TrueType fonts.
        TrueTypeOnly = 0x00040000,

        /// The lpszStyle member is a pointer to a buffer that contains style data that ChooseFont should use to initialize the
        /// Font Style combo box. When the user closes the dialog box, ChooseFont copies style data for the user's selection to
        /// this buffer.
        /// [Note] To globalize your application, you should specify the style by using the lfWeight and lfItalic members of the
        /// LOGFONT structure pointed to by lpLogFont. The style name may change depending on the system user interface language.
        UseStyle = 0x00000080,

        /// Obsolete. ChooseFont ignores this flag.
        /// Windows Vista and Windows XP/2000: ChooseFont should allow only the selection of fonts available on both the printer
        /// and the display. If this flag is specified, the CF_SCREENSHOTS and CF_PRINTERFONTS, or CF_BOTH flags should also be
        /// specified.
        WYSIWYG = 0x00008000,
    };

    /**
     * @brief 标记FontDialogFlags枚举支持位运算
     */
    _SW_ENUM_ENABLE_BIT_OPERATIONS(FontDialogFlags);

    /**
     * @brief 字体选择对话框
     */
    class FontDialog : public IDialog
    {
    private:
        /**
         * @brief 选择的字体
         */
        sw::Font _font;

        /**
         * @brief 字体选择对话框的配置结构体
         */
        CHOOSEFONTW _cf{};

    public:
        /**
         * @brief 对话框的配置标志
         */
        const Property<FontDialogFlags> Flags;

        /**
         * @brief 选择的字体
         */
        const Property<sw::Font> Font;

        /**
         * @brief 选择的字体名称
         */
        const Property<std::wstring> FontName;

        /**
         * @brief 选择的字体大小
         */
        const Property<double> FontSize;

        /**
         * @brief 选择的字体粗细
         */
        const Property<sw::FontWeight> FontWeight;

        /**
         * @brief 是否显示效果选项（下划线、删除线、颜色）
         */
        const Property<bool> ShowEffects;

        /**
         * @brief 选择的颜色
         */
        const Property<Color> SelectedColor;

    public:
        /**
         * @brief 初始化FontDialog
         */
        FontDialog();

        /**
         * @brief FontDialog默认不支持该函数，调用该函数不会执行任何操作
         */
        virtual void Close() override;

        /**
         * @brief FontDialog默认不支持该函数，调用该函数不会执行任何操作
         */
        virtual void Show() override;

        /**
         * @brief 显示对话框，并指定所有者窗口
         * @return 若用户选择了字体则返回true，否则返回false
         */
        virtual int ShowDialog(Window *owner = nullptr) override;

        /**
         * @brief 显示对话框，并指定所有者窗口
         * @return 若用户选择了字体则返回true，否则返回false
         */
        virtual int ShowDialog(Window &owner);

    protected:
        /**
         * @brief 获取选择字体对话框的配置结构体
         */
        CHOOSEFONTW *GetChooseFontStruct();
    };
}

// INotifyObjectDead.h


namespace sw
{
    class INotifyObjectDead; // 向前声明

    /**
     * @brief 对象销毁事件处理程序类型
     */
    using ObjectDeadEventHandler =
        EventHandler<INotifyObjectDead>;

    /**
     * @brief 对象销毁通知接口
     */
    class INotifyObjectDead
    {
    public:
        /**
         * @brief 对象销毁时触发该事件
         */
        const Event<ObjectDeadEventHandler> ObjectDead{
            Event<ObjectDeadEventHandler>::Init(this)
                .Delegate([](INotifyObjectDead *self) -> ObjectDeadEventHandler & {
                    return self->GetObjectDeadEventDelegate();
                }) //
        };

        /**
         * @brief 默认析构函数
         */
        virtual ~INotifyObjectDead() = default;

    protected:
        /**
         * @brief 获取对象销毁事件委托的引用
         * @note 子类应实现该纯虚函数，并在对象销毁时调用该委托
         * @note ObjectDead事件使用该函数返回的委托来保存事件处理程序
         */
        virtual ObjectDeadEventHandler &GetObjectDeadEventDelegate() = 0;
    };
}

// Rect.h


namespace sw
{
    /**
     * @brief 表示一个矩形区域
     */
    struct Rect : public IToString<Rect>,
                  public IEqualityComparable<Rect> {
        /**
         * @brief 左边
         */
        double left;

        /**
         * @brief 顶边
         */
        double top;

        /**
         * @brief 宽度
         */
        double width;

        /**
         * @brief 高度
         */
        double height;

        /**
         * @brief 默认构造函数
         */
        Rect() = default;

        /**
         * @brief 构造Rect
         */
        Rect(double left, double top, double width, double height) noexcept;

        /**
         * @brief 从RECT构造Rect
         */
        Rect(const RECT &rect) noexcept;

        /**
         * @brief 隐式转换RECT
         * @note 由于DIP↔像素之间存在非线性舍入，对从RECT构造的Rect再转回RECT
         *       不保证与原始RECT逐字段相等：right/bottom由(left+width)/(top+height)
         *       重新换算，在非整数倍DPI缩放下可能存在±1像素偏差
         */
        operator RECT() const noexcept;

        /**
         * @brief 获取Rect左上角的位置
         */
        Point GetPos() const noexcept;

        /**
         * @brief 获取Rect的尺寸
         */
        Size GetSize() const noexcept;

        /**
         * @brief 判断两个Rect是否相等
         */
        bool Equals(const Rect &other) const noexcept;

        /**
         * @brief 获取描述当前对象的字符串
         */
        std::wstring ToString() const;
    };

    // Rect应为POD类型
    static_assert(
        std::is_trivial<Rect>::value && std::is_standard_layout<Rect>::value,
        "Rect should be a POD type.");
}

// Reflection.h

// 定义SW_DISABLE_REFLECTION可以禁用反射相关功能
// 若该宏被定义，则相关功能会抛出runtime_error异常
// #define SW_DISABLE_REFLECTION


namespace sw
{
    /**
     * @brief 装箱对象模板类声明
     */
    template <typename T, typename = void>
    class BoxedObject;

    /**
     * @brief 动态对象基类
     */
    class DynamicObject
    {
    private:
        /**
         * @brief 允许BoxedObject访问_isBoxedObject成员
         */
        template <typename, typename>
        friend class BoxedObject;

        /**
         * @brief 指示当前对象是否为装箱对象
         */
        bool _isBoxedObject;

    public:
        /**
         * @brief 默认构造函数
         */
        DynamicObject() noexcept
            : _isBoxedObject(false)
        {
        }

        /**
         * @brief 析构函数
         */
        virtual ~DynamicObject()
        {
        }

        /**
         * @brief 判断对象是否为装箱对象
         * @return 如果对象为装箱对象则返回true，否则返回false
         */
        bool IsBoxedObject() const noexcept
        {
            return _isBoxedObject;
        }

        /**
         * @brief 判断与另一DynamicObject是否引用同一对象
         * @return 引用同一对象时返回true，否则返回false
         * @note 若为引用装箱则比较被引用对象的地址
         */
        bool ReferenceEquals(const DynamicObject &other) const noexcept
        {
            if (this == &other) {
                return true;
            }
            if (IsBoxedObject() && other.IsBoxedObject()) {
                return GetBoxedRawPtr() == other.GetBoxedRawPtr();
            }
            return false;
        }

        /**
         * @brief 获取对象的类型信息
         * @return 对象的类型索引，若为装箱对象则返回被装箱类型的类型索引
         */
        std::type_index GetType() const noexcept
        {
#if defined(SW_DISABLE_REFLECTION)
            throw std::runtime_error("Reflection is disabled, cannot get type index.");
#else
            if (IsBoxedObject()) {
                return GetBoxedType();
            } else {
                return typeid(*this);
            }
#endif
        }

        /**
         * @brief 判断对象是否为指定类型
         * @tparam T 目标类型
         * @param pout 如果不为nullptr，则将转换后的指针赋值给该参数
         * @return 如果对象为指定类型则返回true，否则返回false
         */
        template <typename T>
        auto IsType(T **pout = nullptr)
            -> typename std::enable_if<std::is_base_of<DynamicObject, T>::value, bool>::type
        {
#if defined(SW_DISABLE_REFLECTION)
            throw std::runtime_error("Reflection is disabled, cannot check type.");
#else
            if (pout == nullptr) {
                return dynamic_cast<T *>(this) != nullptr;
            } else {
                *pout = dynamic_cast<T *>(this);
                return *pout != nullptr;
            }
#endif
        }

        /**
         * @brief 判断对象是否为指定类型
         * @tparam T 目标类型
         * @param pout 如果不为nullptr，则将转换后的指针赋值给该参数
         * @return 如果对象为指定类型则返回true，否则返回false
         */
        template <typename T>
        auto IsType(const T **pout = nullptr) const
            -> typename std::enable_if<std::is_base_of<DynamicObject, T>::value, bool>::type
        {
#if defined(SW_DISABLE_REFLECTION)
            throw std::runtime_error("Reflection is disabled, cannot check type.");
#else
            if (pout == nullptr) {
                return dynamic_cast<const T *>(this) != nullptr;
            } else {
                *pout = dynamic_cast<const T *>(this);
                return *pout != nullptr;
            }
#endif
        }

        /**
         * @brief 将对象动态转换为指定类型的引用
         * @tparam T 目标类型
         * @return 指定类型的引用
         * @throws std::bad_cast 如果转换失败
         */
        template <typename T>
        auto DynamicCast()
            -> typename std::enable_if<std::is_base_of<DynamicObject, T>::value, T &>::type
        {
#if defined(SW_DISABLE_REFLECTION)
            throw std::runtime_error("Reflection is disabled, cannot perform dynamic cast.");
#else
            return dynamic_cast<T &>(*this);
#endif
        }

        /**
         * @brief 将对象动态转换为指定类型的常量引用
         * @tparam T 目标类型
         * @return 指定类型的常量引用
         * @throws std::bad_cast 如果转换失败
         */
        template <typename T>
        auto DynamicCast() const
            -> typename std::enable_if<std::is_base_of<DynamicObject, T>::value, const T &>::type
        {
#if defined(SW_DISABLE_REFLECTION)
            throw std::runtime_error("Reflection is disabled, cannot perform dynamic cast.");
#else
            return dynamic_cast<const T &>(*this);
#endif
        }

        /**
         * @brief 将对象不安全地转换为指定类型的引用
         * @tparam T 目标类型
         * @return 指定类型的引用
         * @note 若目标类型与当前类型不兼容，则行为未定义
         */
        template <typename T>
        auto UnsafeCast()
            -> typename std::enable_if<
                std::is_base_of<DynamicObject, T>::value && _IsStaticCastable<DynamicObject *, T *>::value, T &>::type
        {
            return static_cast<T &>(*this);
        }

        /**
         * @brief 将对象不安全地转换为指定类型的引用
         * @tparam T 目标类型
         * @return 指定类型的引用
         * @note 若目标类型与当前类型不兼容，则行为未定义
         * @note 由于目标类型无法通过static_cast转换（如虚继承场景），该重载内部回退到DynamicCast实现
         */
        template <typename T>
        auto UnsafeCast()
            -> typename std::enable_if<
                std::is_base_of<DynamicObject, T>::value && !_IsStaticCastable<DynamicObject *, T *>::value, T &>::type
        {
            return DynamicCast<T>();
        }

        /**
         * @brief 将对象不安全地转换为指定类型的引用
         * @tparam T 目标类型
         * @return 指定类型的引用
         * @note 若目标类型与当前类型不兼容，则行为未定义
         */
        template <typename T>
        auto UnsafeCast() const
            -> typename std::enable_if<
                std::is_base_of<DynamicObject, T>::value && _IsStaticCastable<DynamicObject *, T *>::value, const T &>::type
        {
            return static_cast<const T &>(*this);
        }

        /**
         * @brief 将对象不安全地转换为指定类型的引用
         * @tparam T 目标类型
         * @return 指定类型的引用
         * @note 若目标类型与当前类型不兼容，则行为未定义
         * @note 由于目标类型无法通过static_cast转换（如虚继承场景），该重载内部回退到DynamicCast实现
         */
        template <typename T>
        auto UnsafeCast() const
            -> typename std::enable_if<
                std::is_base_of<DynamicObject, T>::value && !_IsStaticCastable<DynamicObject *, T *>::value, const T &>::type
        {
            return DynamicCast<T>();
        }

    public:
        /**
         * @brief 判断对象是否为指定类型
         * @tparam T 目标类型
         * @param pout 如果不为nullptr，则将转换后的指针赋值给该参数
         * @return 如果对象为指定类型则返回true，否则返回false
         */
        template <typename T>
        auto IsType(T **pout = nullptr)
            -> typename std::enable_if<!std::is_base_of<DynamicObject, T>::value && _IsDynamicCastable<DynamicObject *, T *>::value, bool>::type;

        /**
         * @brief 判断对象是否为指定类型
         * @tparam T 目标类型
         * @param pout 如果不为nullptr，则将转换后的指针赋值给该参数
         * @return 如果对象为指定类型则返回true，否则返回false
         */
        template <typename T>
        auto IsType(T **pout = nullptr)
            -> typename std::enable_if<!std::is_base_of<DynamicObject, T>::value && !_IsDynamicCastable<DynamicObject *, T *>::value, bool>::type;

        /**
         * @brief 判断对象是否为指定类型
         * @tparam T 目标类型
         * @param pout 如果不为nullptr，则将转换后的指针赋值给该参数
         * @return 如果对象为指定类型则返回true，否则返回false
         */
        template <typename T>
        auto IsType(const T **pout = nullptr) const
            -> typename std::enable_if<!std::is_base_of<DynamicObject, T>::value && _IsDynamicCastable<DynamicObject *, T *>::value, bool>::type;

        /**
         * @brief 判断对象是否为指定类型
         * @tparam T 目标类型
         * @param pout 如果不为nullptr，则将转换后的指针赋值给该参数
         * @return 如果对象为指定类型则返回true，否则返回false
         */
        template <typename T>
        auto IsType(const T **pout = nullptr) const
            -> typename std::enable_if<!std::is_base_of<DynamicObject, T>::value && !_IsDynamicCastable<DynamicObject *, T *>::value, bool>::type;

        /**
         * @brief 将对象动态转换为指定类型的引用
         * @tparam T 目标类型
         * @return 指定类型的引用
         * @throws std::bad_cast 如果转换失败
         */
        template <typename T>
        auto DynamicCast()
            -> typename std::enable_if<!std::is_base_of<DynamicObject, T>::value && _IsDynamicCastable<DynamicObject *, T *>::value, T &>::type;

        /**
         * @brief 将对象动态转换为指定类型的引用
         * @tparam T 目标类型
         * @return 指定类型的引用
         * @throws std::bad_cast 如果转换失败
         */
        template <typename T>
        auto DynamicCast()
            -> typename std::enable_if<!std::is_base_of<DynamicObject, T>::value && !_IsDynamicCastable<DynamicObject *, T *>::value, T &>::type;

        /**
         * @brief 将对象动态转换为指定类型的常量引用
         * @tparam T 目标类型
         * @return 指定类型的常量引用
         * @throws std::bad_cast 如果转换失败
         */
        template <typename T>
        auto DynamicCast() const
            -> typename std::enable_if<!std::is_base_of<DynamicObject, T>::value && _IsDynamicCastable<DynamicObject *, T *>::value, const T &>::type;

        /**
         * @brief 将对象动态转换为指定类型的常量引用
         * @tparam T 目标类型
         * @return 指定类型的常量引用
         * @throws std::bad_cast 如果转换失败
         */
        template <typename T>
        auto DynamicCast() const
            -> typename std::enable_if<!std::is_base_of<DynamicObject, T>::value && !_IsDynamicCastable<DynamicObject *, T *>::value, const T &>::type;

        /**
         * @brief 将对象不安全地转换为指定类型的引用
         * @tparam T 目标类型
         * @return 指定类型的引用
         * @note 若目标类型与当前类型不兼容，则行为未定义
         */
        template <typename T>
        auto UnsafeCast()
            -> typename std::enable_if<!std::is_base_of<DynamicObject, T>::value && _IsStaticCastable<DynamicObject *, T *>::value, T &>::type;

        /**
         * @brief 将对象不安全地转换为指定类型的引用
         * @tparam T 目标类型
         * @return 指定类型的引用
         * @note 若目标类型与当前类型不兼容，则行为未定义
         */
        template <typename T>
        auto UnsafeCast()
            -> typename std::enable_if<!std::is_base_of<DynamicObject, T>::value && !_IsStaticCastable<DynamicObject *, T *>::value, T &>::type;

        /**
         * @brief 将对象不安全地转换为指定类型的引用
         * @tparam T 目标类型
         * @return 指定类型的引用
         * @note 若目标类型与当前类型不兼容，则行为未定义
         */
        template <typename T>
        auto UnsafeCast() const
            -> typename std::enable_if<!std::is_base_of<DynamicObject, T>::value && _IsStaticCastable<DynamicObject *, T *>::value, const T &>::type;

        /**
         * @brief 将对象不安全地转换为指定类型的引用
         * @tparam T 目标类型
         * @return 指定类型的引用
         * @note 若目标类型与当前类型不兼容，则行为未定义
         */
        template <typename T>
        auto UnsafeCast() const
            -> typename std::enable_if<!std::is_base_of<DynamicObject, T>::value && !_IsStaticCastable<DynamicObject *, T *>::value, const T &>::type;

    private:
        /**
         * @brief 获取装箱对象的类型信息
         * @return 装箱对象中值的类型索引
         */
        virtual std::type_index GetBoxedType() const noexcept
        {
            return typeid(void);
        }

        /**
         * @brief 判断对象是否为装箱的多态类型
         * @return 如果对象为装箱的多态类型则返回true，否则返回false
         */
        virtual bool IsBoxedPolymorphic() const noexcept
        {
            return false;
        }

        /**
         * @brief 获取装箱对象的原始指针
         * @return 指向装箱对象中值的指针，若当前对象不是装箱对象或为空引用则返回nullptr
         */
        virtual void *GetBoxedRawPtr() noexcept
        {
            return nullptr;
        }

        /**
         * @brief 获取装箱对象的原始指针
         * @return 指向装箱对象中值的常量指针，若当前对象不是装箱对象或为空引用则返回nullptr
         */
        virtual const void *GetBoxedRawPtr() const noexcept
        {
            return nullptr;
        }
    };

    /*================================================================================*/

    /**
     * @brief 装箱对象，用于将任意类型封装为DynamicObject对象
     */
    template <typename T>
    class BoxedObject<T, typename std::enable_if<!std::is_base_of<DynamicObject, T>::value>::type> final
        : public DynamicObject
    {
        /**
         * @brief 内部结构体，用于标识是否为引用参数
         */
        struct _IsRefParam {
            bool val;
        };

        /**
         * @brief 内部数据联合体
         */
        template <typename U, typename = void>
        union _InternalData {
            /**
             * @brief 引用指针
             */
            U *refptr;

            /**
             * @brief 对象缓冲区
             */
            alignas(U) uint8_t objbuf[sizeof(U)];
        };

        /**
         * @brief _InternalData模板特化（对于抽象类）
         * @note 一些旧版编译器对抽象类用alignas(T)会报错无法实例化，所以需要在对应路径避免使用alignas(T)
         */
        template <typename U>
        union _InternalData<U, typename std::enable_if<std::is_abstract<U>::value>::type> {
            /**
             * @brief 引用指针
             */
            U *refptr;

            /**
             * @brief 对象缓冲区
             * @note 抽象类无法构造，objbuf作为占位符，不会使用
             */
            uint8_t objbuf[1];
        };

    private:
        /**
         * @brief 指示当前对象是否为引用对象
         */
        bool _isRef;

        /**
         * @brief 内部数据联合体
         */
        _InternalData<T> _data;

    private:
        /**
         * @brief 私有构造函数，根据是否为引用参数初始化对象
         * @param isRef 指示是否为引用参数
         */
        explicit BoxedObject(_IsRefParam isRef) noexcept
            : _isRef(isRef.val), _data()
        {
            _isBoxedObject = true;
        }

        /**
         * @brief 释放对象资源
         */
        void Release() noexcept(std::is_nothrow_destructible<T>::value)
        {
            if (!_isRef) {
                Unbox().~T();
            }
        }

        /**
         * @brief 非引用的拷贝构造实现
         * @param other 另一个装箱对象
         * @throws std::logic_error 如果T不可拷贝构造
         */
        template <typename U = T>
        auto NotRefCopyConstructImpl(const BoxedObject &other) noexcept(std::is_nothrow_copy_constructible<U>::value)
            -> typename std::enable_if<std::is_copy_constructible<U>::value>::type
        {
            new (_data.objbuf) T(other.Unbox());
        }

        /**
         * @brief 非引用的拷贝构造实现（T不可拷贝构造时，抛出异常）
         * @param other 另一个装箱对象
         * @throws std::logic_error 如果T不可拷贝构造
         */
        template <typename U = T>
        auto NotRefCopyConstructImpl(const BoxedObject &other) noexcept(false)
            -> typename std::enable_if<!std::is_copy_constructible<U>::value>::type
        {
            throw std::logic_error("Type T must be copy constructible to copy construct a non-ref BoxedObject.");
        }

        /**
         * @brief 非引用的移动构造实现
         * @param other 另一个装箱对象
         * @throws std::logic_error 如果T不可移动构造
         */
        template <typename U = T>
        auto NotRefMoveConstructImpl(BoxedObject &&other) noexcept(std::is_nothrow_move_constructible<U>::value)
            -> typename std::enable_if<std::is_move_constructible<U>::value>::type
        {
            new (_data.objbuf) T(std::move(other.Unbox()));
        }

        /**
         * @brief 非引用的移动构造实现（T不可移动构造时，抛出异常）
         * @param other 另一个装箱对象
         * @throws std::logic_error 如果T不可移动构造
         */
        template <typename U = T>
        auto NotRefMoveConstructImpl(BoxedObject &&other) noexcept(false)
            -> typename std::enable_if<!std::is_move_constructible<U>::value>::type
        {
            throw std::logic_error("Type T must be move constructible to move construct a non-ref BoxedObject.");
        }

        /**
         * @brief 非引用的拷贝赋值实现
         * @param other 另一个装箱对象
         * @throws std::logic_error 如果other为非引用装箱且T不可拷贝赋值
         */
        template <typename U = T>
        auto NotRefCopyAssignImpl(const BoxedObject &other)
            -> typename std::enable_if<std::is_copy_assignable<U>::value>::type
        {
            if (other._isRef) {
                Release();
                _data.refptr = other._data.refptr;
                _isRef       = true;
            } else {
                Unbox() = other.Unbox();
            }
        }

        /**
         * @brief 非引用的拷贝赋值实现（T不可拷贝赋值时，抛出异常）
         * @param other 另一个装箱对象
         * @throws std::logic_error 如果other为非引用装箱且T不可拷贝赋值
         */
        template <typename U = T>
        auto NotRefCopyAssignImpl(const BoxedObject &other)
            -> typename std::enable_if<!std::is_copy_assignable<U>::value>::type
        {
            if (other._isRef) {
                Release();
                _data.refptr = other._data.refptr;
                _isRef       = true;
            } else {
                throw std::logic_error("Type T must be copy assignable to copy assign a non-ref BoxedObject.");
            }
        }

        /**
         * @brief 非引用的移动赋值实现
         * @param other 另一个装箱对象
         * @throws std::logic_error 如果other为非引用装箱且T不可移动赋值
         */
        template <typename U = T>
        auto NotRefMoveAssignImpl(BoxedObject &&other)
            -> typename std::enable_if<std::is_move_assignable<U>::value>::type
        {
            if (other._isRef) {
                Release();
                _data.refptr       = other._data.refptr;
                _isRef             = true;
                other._data.refptr = nullptr;
            } else {
                Unbox() = std::move(other.Unbox());
            }
        }

        /**
         * @brief 非引用的移动赋值实现（T不可移动赋值时，抛出异常）
         * @param other 另一个装箱对象
         * @throws std::logic_error 如果other为非引用装箱且T不可移动赋值
         */
        template <typename U = T>
        auto NotRefMoveAssignImpl(BoxedObject &&other)
            -> typename std::enable_if<!std::is_move_assignable<U>::value>::type
        {
            if (other._isRef) {
                Release();
                _data.refptr       = other._data.refptr;
                _isRef             = true;
                other._data.refptr = nullptr;
            } else {
                throw std::logic_error("Type T must be move assignable to move assign a non-ref BoxedObject.");
            }
        }

    public:
        /**
         * @brief 构造值类型的装箱对象
         * @param args 构造参数列表
         */
        template <typename... Args>
        BoxedObject(Args &&...args)
            : BoxedObject(_IsRefParam{false})
        {
            new (_data.objbuf) T(std::forward<Args>(args)...);
        }

        /**
         * @brief 析构函数
         */
        virtual ~BoxedObject()
        {
            Release();
        }

        /**
         * @brief 创建引用类型的装箱对象
         * @param ptr 指向外部对象的指针
         * @return 引用类型的装箱对象
         */
        static BoxedObject<T> MakeRef(T *ptr) noexcept
        {
            BoxedObject<T> result{_IsRefParam{true}};
            result._data.refptr = ptr;
            return result;
        }

        /**
         * @brief 创建引用类型的装箱对象
         * @param ref 被引用的外部对象
         * @return 引用类型的装箱对象
         */
        static BoxedObject<T> MakeRef(T &ref) noexcept
        {
            return MakeRef(&ref);
        }

        /**
         * @brief 创建引用类型的装箱对象
         * @tparam U 目标类型
         * @param other 另一个装箱对象
         * @return 引用类型的装箱对象
         */
        template <typename U>
        static auto MakeRef(BoxedObject<U> &other) noexcept
            -> typename std::enable_if<_IsStaticCastable<U, T>::value, BoxedObject<T>>::type
        {
            BoxedObject<T> result{_IsRefParam{true}};
            result._data.refptr = other.IsNullRef() ? nullptr : static_cast<T *>(&other.Unbox());
            return result;
        }

        /**
         * @brief 拷贝构造函数
         * @param other 另一个装箱对象
         * @throws std::logic_error 如果other为非引用装箱且T不可拷贝构造
         */
        BoxedObject(const BoxedObject &other) noexcept(std::is_nothrow_copy_constructible<T>::value)
            : BoxedObject(_IsRefParam{other._isRef})
        {
            if (_isRef) {
                _data.refptr = other._data.refptr;
            } else {
                NotRefCopyConstructImpl(other);
            }
        }

        /**
         * @brief 移动构造函数
         * @param other 另一个装箱对象
         * @throws std::logic_error 如果other为非引用装箱且T不可移动构造
         */
        BoxedObject(BoxedObject &&other) noexcept(std::is_nothrow_move_constructible<T>::value)
            : BoxedObject(_IsRefParam{other._isRef})
        {
            if (_isRef) {
                _data.refptr       = other._data.refptr;
                other._data.refptr = nullptr;
            } else {
                NotRefMoveConstructImpl(std::move(other));
            }
        }

        /**
         * @brief 拷贝赋值运算符
         * @param other 另一个装箱对象
         * @return 当前装箱对象的引用
         * @throws std::logic_error 如果当前对象为引用装箱且other为非引用装箱，同时T不可拷贝构造
         * @throws std::logic_error 如果当前对象为非引用装箱且other为非引用装箱，同时T不可拷贝赋值
         */
        BoxedObject &operator=(const BoxedObject &other)
        {
            if (this == &other)
                return *this;

            if (_isRef) {
                if (other._isRef) {
                    _data.refptr = other._data.refptr;
                } else {
                    auto oldPtr = _data.refptr;
                    try {
                        NotRefCopyConstructImpl(other);
                        _isRef = false;
                    } catch (...) {
                        _data.refptr = oldPtr;
                        throw;
                    }
                }
            } else {
                NotRefCopyAssignImpl(other);
            }
            return *this;
        }

        /**
         * @brief 移动赋值运算符
         * @param other 另一个装箱对象
         * @return 当前装箱对象的引用
         * @throws std::logic_error 如果当前对象为引用装箱且other为非引用装箱，同时T不可移动构造
         * @throws std::logic_error 如果当前对象为非引用装箱且other为非引用装箱，同时T不可移动赋值
         */
        BoxedObject &operator=(BoxedObject &&other)
        {
            if (this == &other)
                return *this;

            if (_isRef) {
                if (other._isRef) {
                    _data.refptr       = other._data.refptr;
                    other._data.refptr = nullptr;
                } else {
                    auto oldPtr = _data.refptr;
                    try {
                        NotRefMoveConstructImpl(std::move(other));
                        _isRef = false;
                    } catch (...) {
                        _data.refptr = oldPtr;
                        throw;
                    }
                }
            } else {
                NotRefMoveAssignImpl(std::move(other));
            }
            return *this;
        }

        /**
         * @brief 判断当前装箱对象是否为引用类型
         * @return 若当前对象为引用类型则返回true，否则返回false
         */
        bool IsRef() const noexcept
        {
            return _isRef;
        }

        /**
         * @brief 判断当前装箱对象是否为空引用
         * @return 若当前对象为引用类型且引用指针为空则返回true，否则返回false
         */
        bool IsNullRef() const noexcept
        {
            return _isRef && (_data.refptr == nullptr);
        }

        /**
         * @brief 判断当前装箱对象是否包含有效值
         * @return 若当前对象包含有效值则返回true，否则返回false
         */
        bool HasValue() const noexcept
        {
            return !IsNullRef();
        }

        /**
         * @brief 获取装箱对象中的值
         * @return 装箱对象中的值的引用
         * @note 仅当装箱对象包含有效值时调用此方法，否则行为未定义
         */
        T &Unbox() noexcept
        {
            assert(HasValue());
            return _isRef ? *_data.refptr : *reinterpret_cast<T *>(_data.objbuf);
        }

        /**
         * @brief 获取装箱对象中的值
         * @return 装箱对象中的值的常量引用
         * @note 仅当装箱对象包含有效值时调用此方法，否则行为未定义
         */
        const T &Unbox() const noexcept
        {
            assert(HasValue());
            return _isRef ? *_data.refptr : *reinterpret_cast<const T *>(_data.objbuf);
        }

    private:
        /**
         * @brief 获取装箱对象的类型信息
         * @return 装箱对象中值的类型索引
         */
        virtual std::type_index GetBoxedType() const noexcept override
        {
            return typeid(T);
        }

        /**
         * @brief 判断对象是否为装箱的多态类型
         * @return 如果对象为装箱的多态类型则返回true，否则返回false
         */
        virtual bool IsBoxedPolymorphic() const noexcept override
        {
            return std::is_polymorphic<T>::value;
        }

        /**
         * @brief 获取装箱对象的原始指针
         * @return 指向装箱对象中值的指针，若当前对象不是装箱对象或为空引用则返回nullptr
         */
        virtual void *GetBoxedRawPtr() noexcept override
        {
            // 当T带const时，refptr无法直接转换为void*，需要先转换为const void*再去掉const
            return const_cast<void *>(
                _isRef ? static_cast<const void *>(_data.refptr) : static_cast<const void *>(_data.objbuf));
        }

        /**
         * @brief 获取装箱对象的原始指针
         * @return 指向装箱对象中值的常量指针，若当前对象不是装箱对象或为空引用则返回nullptr
         */
        virtual const void *GetBoxedRawPtr() const noexcept override
        {
            return _isRef ? static_cast<const void *>(_data.refptr) : static_cast<const void *>(_data.objbuf);
        }
    };

    /*================================================================================*/

    /**
     * @brief 判断对象是否为指定类型
     * @tparam T 目标类型
     * @param pout 如果不为nullptr，则将转换后的指针赋值给该参数
     * @return 如果对象为指定类型则返回true，否则返回false
     */
    template <typename T>
    auto DynamicObject::IsType(T **pout)
        -> typename std::enable_if<!std::is_base_of<DynamicObject, T>::value && _IsDynamicCastable<DynamicObject *, T *>::value, bool>::type
    {
#if defined(SW_DISABLE_REFLECTION)
        throw std::runtime_error("Reflection is disabled, cannot check type.");
#else
        if (!_isBoxedObject) {
            if (pout == nullptr) {
                return dynamic_cast<T *>(this) != nullptr;
            } else {
                *pout = dynamic_cast<T *>(this);
                return *pout != nullptr;
            }
        } else {
            if (GetBoxedType() == typeid(T)) {
                if (pout == nullptr) {
                    return GetBoxedRawPtr() != nullptr;
                } else {
                    *pout = static_cast<T *>(GetBoxedRawPtr());
                    return *pout != nullptr;
                }
            } else {
                if (pout != nullptr) {
                    *pout = nullptr;
                }
                return false;
            }
        }
#endif
    }

    /**
     * @brief 判断对象是否为指定类型
     * @tparam T 目标类型
     * @param pout 如果不为nullptr，则将转换后的指针赋值给该参数
     * @return 如果对象为指定类型则返回true，否则返回false
     */
    template <typename T>
    auto DynamicObject::IsType(T **pout)
        -> typename std::enable_if<!std::is_base_of<DynamicObject, T>::value && !_IsDynamicCastable<DynamicObject *, T *>::value, bool>::type
    {
#if defined(SW_DISABLE_REFLECTION)
        throw std::runtime_error("Reflection is disabled, cannot check type.");
#else
        if (!_isBoxedObject) {
            if (pout != nullptr) {
                *pout = nullptr;
            }
            return false;
        } else {
            if (GetBoxedType() == typeid(T)) {
                if (pout == nullptr) {
                    return GetBoxedRawPtr() != nullptr;
                } else {
                    *pout = static_cast<T *>(GetBoxedRawPtr());
                    return *pout != nullptr;
                }
            } else {
                if (pout != nullptr) {
                    *pout = nullptr;
                }
                return false;
            }
        }
#endif
    }

    /**
     * @brief 判断对象是否为指定类型
     * @tparam T 目标类型
     * @param pout 如果不为nullptr，则将转换后的指针赋值给该参数
     * @return 如果对象为指定类型则返回true，否则返回false
     */
    template <typename T>
    auto DynamicObject::IsType(const T **pout) const
        -> typename std::enable_if<!std::is_base_of<DynamicObject, T>::value && _IsDynamicCastable<DynamicObject *, T *>::value, bool>::type
    {
#if defined(SW_DISABLE_REFLECTION)
        throw std::runtime_error("Reflection is disabled, cannot check type.");
#else
        if (!_isBoxedObject) {
            if (pout == nullptr) {
                return dynamic_cast<const T *>(this) != nullptr;
            } else {
                *pout = dynamic_cast<const T *>(this);
                return *pout != nullptr;
            }
        } else {
            if (GetBoxedType() == typeid(T)) {
                if (pout == nullptr) {
                    return GetBoxedRawPtr() != nullptr;
                } else {
                    *pout = static_cast<const T *>(GetBoxedRawPtr());
                    return *pout != nullptr;
                }
            } else {
                if (pout != nullptr) {
                    *pout = nullptr;
                }
                return false;
            }
        }
#endif
    }

    /**
     * @brief 判断对象是否为指定类型
     * @tparam T 目标类型
     * @param pout 如果不为nullptr，则将转换后的指针赋值给该参数
     * @return 如果对象为指定类型则返回true，否则返回false
     */
    template <typename T>
    auto DynamicObject::IsType(const T **pout) const
        -> typename std::enable_if<!std::is_base_of<DynamicObject, T>::value && !_IsDynamicCastable<DynamicObject *, T *>::value, bool>::type
    {
#if defined(SW_DISABLE_REFLECTION)
        throw std::runtime_error("Reflection is disabled, cannot check type.");
#else
        if (!_isBoxedObject) {
            if (pout != nullptr) {
                *pout = nullptr;
            }
            return false;
        } else {
            if (GetBoxedType() == typeid(T)) {
                if (pout == nullptr) {
                    return GetBoxedRawPtr() != nullptr;
                } else {
                    *pout = static_cast<const T *>(GetBoxedRawPtr());
                    return *pout != nullptr;
                }
            } else {
                if (pout != nullptr) {
                    *pout = nullptr;
                }
                return false;
            }
        }
#endif
    }

    /**
     * @brief 将对象动态转换为指定类型的引用
     * @tparam T 目标类型
     * @return 指定类型的引用
     * @throws std::bad_cast 如果转换失败
     */
    template <typename T>
    auto DynamicObject::DynamicCast()
        -> typename std::enable_if<!std::is_base_of<DynamicObject, T>::value && _IsDynamicCastable<DynamicObject *, T *>::value, T &>::type
    {
#if defined(SW_DISABLE_REFLECTION)
        throw std::runtime_error("Reflection is disabled, cannot perform dynamic cast.");
#else
        if (!_isBoxedObject) {
            return dynamic_cast<T &>(*this);
        } else {
            void *rawPtr = GetBoxedRawPtr();
            if (rawPtr == nullptr || GetBoxedType() != typeid(T)) {
                throw std::bad_cast();
            } else {
                return *static_cast<T *>(rawPtr);
            }
        }
#endif
    }

    /**
     * @brief 将对象动态转换为指定类型的引用
     * @tparam T 目标类型
     * @return 指定类型的引用
     * @throws std::bad_cast 如果转换失败
     */
    template <typename T>
    auto DynamicObject::DynamicCast()
        -> typename std::enable_if<!std::is_base_of<DynamicObject, T>::value && !_IsDynamicCastable<DynamicObject *, T *>::value, T &>::type
    {
#if defined(SW_DISABLE_REFLECTION)
        throw std::runtime_error("Reflection is disabled, cannot perform dynamic cast.");
#else
        if (!_isBoxedObject) {
            throw std::bad_cast();
        } else {
            void *rawPtr = GetBoxedRawPtr();
            if (rawPtr == nullptr || GetBoxedType() != typeid(T)) {
                throw std::bad_cast();
            } else {
                return *static_cast<T *>(rawPtr);
            }
        }
#endif
    }

    /**
     * @brief 将对象动态转换为指定类型的常量引用
     * @tparam T 目标类型
     * @return 指定类型的常量引用
     * @throws std::bad_cast 如果转换失败
     */
    template <typename T>
    auto DynamicObject::DynamicCast() const
        -> typename std::enable_if<!std::is_base_of<DynamicObject, T>::value && _IsDynamicCastable<DynamicObject *, T *>::value, const T &>::type
    {
#if defined(SW_DISABLE_REFLECTION)
        throw std::runtime_error("Reflection is disabled, cannot perform dynamic cast.");
#else
        if (!_isBoxedObject) {
            return dynamic_cast<const T &>(*this);
        } else {
            const void *rawPtr = GetBoxedRawPtr();
            if (rawPtr == nullptr || GetBoxedType() != typeid(T)) {
                throw std::bad_cast();
            } else {
                return *static_cast<const T *>(rawPtr);
            }
        }
#endif
    }

    /**
     * @brief 将对象动态转换为指定类型的常量引用
     * @tparam T 目标类型
     * @return 指定类型的常量引用
     * @throws std::bad_cast 如果转换失败
     */
    template <typename T>
    auto DynamicObject::DynamicCast() const
        -> typename std::enable_if<!std::is_base_of<DynamicObject, T>::value && !_IsDynamicCastable<DynamicObject *, T *>::value, const T &>::type
    {
#if defined(SW_DISABLE_REFLECTION)
        throw std::runtime_error("Reflection is disabled, cannot perform dynamic cast.");
#else
        if (!_isBoxedObject) {
            throw std::bad_cast();
        } else {
            const void *rawPtr = GetBoxedRawPtr();
            if (rawPtr == nullptr || GetBoxedType() != typeid(T)) {
                throw std::bad_cast();
            } else {
                return *static_cast<const T *>(rawPtr);
            }
        }
#endif
    }

    /**
     * @brief 将对象不安全地转换为指定类型的引用
     * @tparam T 目标类型
     * @return 指定类型的引用
     * @note 若目标类型与当前类型不兼容，则行为未定义
     */
    template <typename T>
    auto DynamicObject::UnsafeCast()
        -> typename std::enable_if<!std::is_base_of<DynamicObject, T>::value && _IsStaticCastable<DynamicObject *, T *>::value, T &>::type
    {
        if (!_isBoxedObject) {
            return static_cast<T &>(*this);
        } else {
            return static_cast<BoxedObject<T> &>(*this).Unbox();
        }
    }

    /**
     * @brief 将对象不安全地转换为指定类型的引用
     * @tparam T 目标类型
     * @return 指定类型的引用
     * @note 若目标类型与当前类型不兼容，则行为未定义
     * @note 由于目标类型无法通过static_cast转换（如虚继承场景），当对象为非装箱对象时，内部回退到DynamicCast实现
     */
    template <typename T>
    auto DynamicObject::UnsafeCast()
        -> typename std::enable_if<!std::is_base_of<DynamicObject, T>::value && !_IsStaticCastable<DynamicObject *, T *>::value, T &>::type
    {
        if (!_isBoxedObject) {
            return DynamicCast<T>();
        } else {
            return static_cast<BoxedObject<T> &>(*this).Unbox();
        }
    }

    /**
     * @brief 将对象不安全地转换为指定类型的引用
     * @tparam T 目标类型
     * @return 指定类型的引用
     * @note 若目标类型与当前类型不兼容，则行为未定义
     */
    template <typename T>
    auto DynamicObject::UnsafeCast() const
        -> typename std::enable_if<!std::is_base_of<DynamicObject, T>::value && _IsStaticCastable<DynamicObject *, T *>::value, const T &>::type
    {
        if (!_isBoxedObject) {
            return static_cast<const T &>(*this);
        } else {
            return static_cast<const BoxedObject<T> &>(*this).Unbox();
        }
    }

    /**
     * @brief 将对象不安全地转换为指定类型的引用
     * @tparam T 目标类型
     * @return 指定类型的引用
     * @note 若目标类型与当前类型不兼容，则行为未定义
     * @note 由于目标类型无法通过static_cast转换（如虚继承场景），当对象为非装箱对象时，内部回退到DynamicCast实现
     */
    template <typename T>
    auto DynamicObject::UnsafeCast() const
        -> typename std::enable_if<!std::is_base_of<DynamicObject, T>::value && !_IsStaticCastable<DynamicObject *, T *>::value, const T &>::type
    {
        if (!_isBoxedObject) {
            return DynamicCast<T>();
        } else {
            return static_cast<const BoxedObject<T> &>(*this).Unbox();
        }
    }

    /*================================================================================*/

    /**
     * @brief 表示字段的唯一标识符
     */
    struct FieldId : public IToString<FieldId>,
                     public IComparable<FieldId, FieldId> {
        /**
         * @brief 字段ID的数值
         */
        uint32_t value;

        /**
         * @brief 默认构造函数
         */
        FieldId() = default;

        /**
         * @brief 构造指定值的字段ID
         */
        FieldId(uint32_t value)
            : value(value)
        {
        }

        /**
         * @brief 获取字段ID的字符串表示形式
         */
        std::wstring ToString() const
        {
            return std::to_wstring(value);
        }

        /**
         * @brief 比较字段ID
         * @return 值相等返回0，小于返回负数，大于返回正数
         */
        int CompareTo(FieldId other) const
        {
            if (value == other.value) {
                return 0;
            } else {
                return value < other.value ? -1 : 1;
            }
        }
    };

    /*================================================================================*/

    /**
     * @brief 提供反射相关功能
     */
    class Reflection
    {
    public:
        /**
         * @brief 静态类，不允许实例化
         */
        Reflection() = delete;

    public:
        /**
         * @brief 获取字段的唯一标识符
         * @tparam T 字段所属类类型
         * @tparam TField 字段类型
         * @param field 字段的成员指针
         * @return 对应的字段ID
         */
        template <typename T, typename TField>
        static FieldId GetFieldId(TField T::*field) noexcept
        {
            auto pfunc = &Reflection::GetFieldId<T, TField>;

            uint8_t buffer[sizeof(pfunc) + sizeof(field)];
            memcpy(buffer, &pfunc, sizeof(pfunc));
            memcpy(buffer + sizeof(pfunc), &field, sizeof(field));

            uint32_t prime = 16777619u;
            uint32_t hash  = 2166136261u;

            for (size_t i = 0; i < sizeof(buffer); ++i) {
                hash ^= static_cast<uint32_t>(buffer[i]);
                hash *= prime;
            }

            return FieldId{hash};
        }

        /**
         * @brief 获取成员函数的委托
         * @tparam T 成员函数所属类类型
         * @tparam TRet 成员函数返回值类型
         * @tparam Args 成员函数参数类型列表
         * @param method 成员函数指针
         * @return 对应的委托
         */
        template <typename T, typename TRet, typename... Args>
        static auto GetMethod(TRet (T::*method)(Args...)) -> Delegate<TRet(DynamicObject &, Args...)>
        {
            return [method](DynamicObject &obj, Args... args) -> TRet {
                return (obj.UnsafeCast<T>().*method)(std::forward<Args>(args)...);
            };
        }

        /**
         * @brief 获取常量成员函数的委托
         * @tparam T 成员函数所属类类型
         * @tparam TRet 成员函数返回值类型
         * @tparam Args 成员函数参数类型列表
         * @param method 成员函数指针
         * @return 对应的委托
         */
        template <typename T, typename TRet, typename... Args>
        static auto GetMethod(TRet (T::*method)(Args...) const) -> Delegate<TRet(DynamicObject &, Args...)>
        {
            return [method](DynamicObject &obj, Args... args) -> TRet {
                return (obj.UnsafeCast<T>().*method)(std::forward<Args>(args)...);
            };
        }

        /**
         * @brief 获取字段的访问器
         * @tparam T 字段所属类类型
         * @tparam TField 字段类型
         * @param field 字段的成员指针
         * @return 对应的访问器
         */
        template <typename T, typename TField>
        static auto GetFieldAccessor(TField T::*field) -> Delegate<TField &(DynamicObject &)>
        {
            return [field](DynamicObject &obj) -> TField & {
                return obj.UnsafeCast<T>().*field;
            };
        }

        /**
         * @brief 获取属性的Getter委托
         * @tparam T 属性所属类类型
         * @tparam TProperty 属性类型
         * @param prop 属性指针
         * @return 对应的Getter委托
         * @note 若属性不可读则返回空委托
         */
        template <typename T, typename TProperty>
        static auto GetPropertyGetter(TProperty T::*prop)
            -> typename std::enable_if<
                _IsReadableProperty<TProperty>::value,
                Delegate<typename TProperty::TValue(DynamicObject &)>>::type
        {
            return [prop](DynamicObject &obj) -> typename TProperty::TValue {
                return (obj.UnsafeCast<T>().*prop).Get();
            };
        }

        /**
         * @brief 获取属性的Getter委托
         * @tparam T 属性所属类类型
         * @tparam TProperty 属性类型
         * @param prop 属性指针
         * @return 对应的Getter委托
         * @note 若属性不可读则返回空委托
         */
        template <typename T, typename TProperty>
        static auto GetPropertyGetter(TProperty T::*prop)
            -> typename std::enable_if<
                _IsProperty<TProperty>::value && !_IsReadableProperty<TProperty>::value,
                Delegate<typename TProperty::TValue(DynamicObject &)>>::type
        {
            return nullptr;
        }

        /**
         * @brief 获取属性的Setter委托
         * @tparam T 属性所属类类型
         * @tparam TProperty 属性类型
         * @param prop 属性指针
         * @return 对应的Setter委托
         * @note 若属性不可写则返回空委托
         */
        template <typename T, typename TProperty>
        static auto GetPropertySetter(TProperty T::*prop)
            -> typename std::enable_if<
                _IsWritableProperty<TProperty>::value,
                Delegate<void(DynamicObject &, typename TProperty::TSetterParam)>>::type
        {
            return [prop](DynamicObject &obj, typename TProperty::TSetterParam value) {
                (obj.UnsafeCast<T>().*prop).Set(std::forward<typename TProperty::TSetterParam>(value));
            };
        }

        /**
         * @brief 获取属性的Setter委托
         * @tparam T 属性所属类类型
         * @tparam TProperty 属性类型
         * @param prop 属性指针
         * @return 对应的Setter委托
         * @note 若属性不可写则返回空委托
         */
        template <typename T, typename TProperty>
        static auto GetPropertySetter(TProperty T::*prop)
            -> typename std::enable_if<
                _IsProperty<TProperty>::value && !_IsWritableProperty<TProperty>::value,
                Delegate<void(DynamicObject &, typename TProperty::TSetterParam)>>::type
        {
            return nullptr;
        }

    public:
        /**
         * @brief 调用成员函数
         * @tparam T 成员函数所属类类型
         * @tparam TFunc 成员函数类型
         * @tparam Args 成员函数参数类型列表
         * @param method 成员函数的委托
         * @param obj 对象引用
         * @param args 成员函数参数列表
         * @return 成员函数返回值
         */
        template <typename T, typename TFunc, typename... Args>
        static auto InvokeMethod(const Delegate<TFunc> &method, T &obj, Args &&...args)
            -> typename std::enable_if<std::is_base_of<DynamicObject, T>::value,
                                       decltype(method(obj, std::forward<Args>(args)...))>::type
        {
            assert(method != nullptr);
            return method(obj, std::forward<Args>(args)...);
        }

        /**
         * @brief 调用成员函数
         * @tparam T 成员函数所属类类型
         * @tparam TFunc 成员函数类型
         * @tparam Args 成员函数参数类型列表
         * @param method 成员函数的委托
         * @param obj 对象引用
         * @param args 成员函数参数列表
         * @return 成员函数返回值
         */
        template <typename T, typename TFunc, typename... Args>
        static auto InvokeMethod(const Delegate<TFunc> &method, T &obj, Args &&...args)
            -> typename std::enable_if<!std::is_base_of<DynamicObject, T>::value,
                                       decltype(method(std::declval<DynamicObject &>(), std::forward<Args>(args)...))>::type
        {
            assert(method != nullptr);
            auto boxed = BoxedObject<T>::MakeRef(obj);
            return method(boxed, std::forward<Args>(args)...);
        }

        /**
         * @brief 访问字段
         * @tparam T 字段所属类类型
         * @tparam TField 字段类型
         * @param accessor 字段访问器的委托
         * @param obj 对象引用
         * @return 字段引用
         */
        template <typename T, typename TField>
        static auto AccessField(const Delegate<TField &(DynamicObject &)> &accessor, T &obj)
            -> typename std::enable_if<std::is_base_of<DynamicObject, T>::value, TField &>::type
        {
            assert(accessor != nullptr);
            return accessor(obj);
        }

        /**
         * @brief 访问字段
         * @tparam T 字段所属类类型
         * @tparam TField 字段类型
         * @param accessor 字段访问器的委托
         * @param obj 对象引用
         * @return 字段引用
         */
        template <typename T, typename TField>
        static auto AccessField(const Delegate<TField &(DynamicObject &)> &accessor, T &obj)
            -> typename std::enable_if<!std::is_base_of<DynamicObject, T>::value, TField &>::type
        {
            assert(accessor != nullptr);
            auto boxed = BoxedObject<T>::MakeRef(obj);
            return accessor(boxed);
        }

        /**
         * @brief 获取属性值
         * @tparam T 属性所属类类型
         * @tparam TValue 属性值类型
         * @param getter 属性Getter的委托
         * @param obj 对象引用
         * @return 属性值
         */
        template <typename T, typename TValue>
        static auto GetProperty(const Delegate<TValue(DynamicObject &)> &getter, T &obj)
            -> typename std::enable_if<std::is_base_of<DynamicObject, T>::value, TValue>::type
        {
            assert(getter != nullptr);
            return getter(obj);
        }

        /**
         * @brief 获取属性值
         * @tparam T 属性所属类类型
         * @tparam TValue 属性值类型
         * @param getter 属性Getter的委托
         * @param obj 对象引用
         * @return 属性值
         */
        template <typename T, typename TValue>
        static auto GetProperty(const Delegate<TValue(DynamicObject &)> &getter, T &obj)
            -> typename std::enable_if<!std::is_base_of<DynamicObject, T>::value, TValue>::type
        {
            assert(getter != nullptr);
            auto boxed = BoxedObject<T>::MakeRef(obj);
            return getter(boxed);
        }

        /**
         * @brief 设置属性值
         * @tparam T 属性所属类类型
         * @tparam TParam 属性Setter参数类型
         * @tparam TValue 属性值类型
         * @param setter 属性Setter的委托
         * @param obj 对象引用
         * @param value 属性值
         */
        template <typename T, typename TParam, typename TValue>
        static auto SetProperty(const Delegate<void(DynamicObject &, TParam)> &setter, T &obj, TValue &&value)
            -> typename std::enable_if<std::is_base_of<DynamicObject, T>::value>::type
        {
            assert(setter != nullptr);
            setter(obj, std::forward<TValue>(value));
        }

        /**
         * @brief 设置属性值
         * @tparam T 属性所属类类型
         * @tparam TParam 属性Setter参数类型
         * @tparam TValue 属性值类型
         * @param setter 属性Setter的委托
         * @param obj 对象引用
         * @param value 属性值
         */
        template <typename T, typename TParam, typename TValue>
        static auto SetProperty(const Delegate<void(DynamicObject &, TParam)> &setter, T &obj, TValue &&value)
            -> typename std::enable_if<!std::is_base_of<DynamicObject, T>::value>::type
        {
            assert(setter != nullptr);
            auto boxed = BoxedObject<T>::MakeRef(obj);
            setter(boxed, std::forward<TValue>(value));
        }
    };
}

// 为sw::FieldId特化std::hash
template <>
struct std::hash<sw::FieldId> //
{
    size_t operator()(sw::FieldId fieldId) const noexcept
    {
        return static_cast<size_t>(fieldId.value);
    }
};

// RoutedEvent.h


namespace sw
{
    /**
     * @brief 路由事件类型枚举
     */
    enum RoutedEventType : uint32_t //
    {
        /// 无效的路由事件类型，表示未指定事件类型或事件类型无效
        RoutedEventType_Null = 0,

        /// 从该值开始到RoutedEventType_UserEnd结束表示用户可以自定义路由事件的值范围
        RoutedEventType_User = 1,

        /// 用户自定义路由事件的值的最大值
        RoutedEventType_UserEnd = 0x80000000,

        /// 尺寸改变，参数类型为sw::SizeChangedEventArgs
        UIElement_SizeChanged,

        /// 位置改变，参数类型为sw::PositionChangedEventArgs
        UIElement_PositionChanged,

        /// Text属性发生变化，参数类型为sw::RoutedEventArgs
        UIElement_TextChanged,

        /// 获取到焦点，参数类型为sw::RoutedEventArgs
        UIElement_GotFocus,

        /// 失去焦点，参数类型为sw::RoutedEventArgs
        UIElement_LostFocus,

        /// 输入字符，参数类型为sw::GotCharEventArgs
        UIElement_GotChar,

        /// 键盘按键按下，参数类型为sw::KeyDownEventArgs
        UIElement_KeyDown,

        /// 键盘按键抬起，参数类型为sw::KeyUpEventArgs
        UIElement_KeyUp,

        /// 鼠标移动，参数类型为sw::MouseMoveEventArgs
        UIElement_MouseMove,

        /// 鼠标离开，参数类型为sw::RoutedEventArgs
        UIElement_MouseLeave,

        /// 鼠标滚轮滚动，参数类型为sw::MouseWheelEventArgs
        UIElement_MouseWheel,

        /// 鼠标按键按下，参数类型为sw::MouseButtonDownEventArgs
        UIElement_MouseButtonDown,

        /// 鼠标按键抬起，参数类型为sw::MouseButtonUpEventArgs
        UIElement_MouseButtonUp,

        /// 要显示用户自定义的上下文菜单前触发该事件，参数类型为sw::ShowContextMenuEventArgs
        UIElement_ShowContextMenu,

        /// 接收到文件拖放，参数类型为sw::DropFilesEventArgs
        UIElement_DropFiles,

        /// 窗口正在关闭，参数类型为sw::WindowClosingEventArgs
        Window_Closing,

        /// 窗口已关闭，参数类型为sw::RoutedEventArgs
        Window_Closed,

        /// 窗口成为前台窗口，参数类型为sw::RoutedEventArgs
        Window_Actived,

        /// 窗口成为后台窗口，参数类型为sw::RoutedEventArgs
        Window_Inactived,

        /// 按钮被单击，参数类型为sw::RoutedEventArgs
        ButtonBase_Clicked,

        /// 按钮被双击，参数类型为sw::RoutedEventArgs
        ButtonBase_DoubleClicked,

        /// 分割按钮的下拉箭头被单击，参数类型为sw::SplitButtonDropDownEventArgs
        SplitButton_DropDown,

        /// 列表视图/列表框/组合框的选中项改变，参数类型为sw::RoutedEventArgs
        ItemsControl_SelectionChanged,

        /// 列表视图某个复选框的选中状态改变，参数类型为sw::ListViewCheckStateChangedEventArgs
        ListView_CheckStateChanged,

        /// 鼠标左键单击列表视图的列标题，参数类型为sw::ListViewHeaderClickedEventArgs
        ListView_HeaderClicked,

        /// 鼠标左键双击列表视图的列标题，参数类型为sw::ListViewHeaderDoubleClickedEventArgs
        ListView_HeaderDoubleClicked,

        /// 鼠标左键单击列表视图某个项，参数类型为sw::ListViewItemClickedEventArgs
        ListView_ItemClicked,

        /// 鼠标左键单击列表视图某个项，参数类型为sw::ListViewItemDoubleClickedEventArgs
        ListView_ItemDoubleClicked,

        /// 编辑状态结束，参数类型为sw::ListViewEndEditEventArgs
        ListView_EndEdit,

        /// 滑块的值被改变，参数类型为sw::RoutedEventArgs
        Slider_ValueChanged,

        /// 滑块被释放，参数类型为sw::RoutedEventArgs
        Slider_EndTrack,

        /// 窗口/面板滚动条滚动，参数类型为sw::ScrollingEventArgs
        Layer_Scrolling,

        /// SelectedIndex属性被改变，参数类型为sw::RoutedEventArgs
        TabControl_SelectedIndexChanged,

        /// DateTimePicker控件的时间改变，参数类型为sw::DateTimePickerTimeChangedEventArgs
        DateTimePicker_TimeChanged,

        /// 月历控件的时间改变，参数类型为sw::MonthCalendarTimeChangedEventArgs
        MonthCalendar_TimeChanged,

        /// IP地址框地址被改变，参数类型为sw::RoutedEventArgs
        IPAddressControl_AddressChanged,

        /// SysLink控件链接被单击，参数类型为sw::SysLinkClickedEventArgs
        SysLink_Clicked,

        /// 热键框的值被改变，参数类型为sw::HotKeyValueChangedEventArgs
        HotKeyControl_ValueChanged,

        /// 树视图选中的节点发生改变，参数类型为sw::RoutedEventArgs
        TreeView_SelectionChanged,

        /// 树视图节点正在展开或折叠，参数类型为sw::TreeViewItemExpandingEventArgs
        TreeView_ItemExpanding,

        /// 树视图节点已展开或折叠，参数类型为sw::TreeViewItemExpandedEventArgs
        TreeView_ItemExpanded,

        /// 树视图节点的复选框选中状态被改变，参数类型为sw::TreeViewCheckStateChangedEventArgs
        TreeView_CheckStateChanged,
    };

    /*================================================================================*/

    class UIElement; // UIElement.h

    /**
     * @brief 路由事件的参数
     */
    struct RoutedEventArgs : public EventArgs //
    {
        /**
         * @brief 事件类型
         */
        RoutedEventType eventType;

        /**
         * @brief 事件是否已被处理，若将此字段设为true，则事件不会继续往上传递
         */
        bool handled = false;

        /**
         * @brief 表示是否已处理事件所对应的Windows消息，对于部分消息将字段设为true可取消对DefaultWndProc的调用，若当前事件无对应消息则该字段无意义
         */
        bool handledMsg = false;

        /**
         * @brief 事件源，指向触发当前事件的UIElement
         */
        UIElement *source = nullptr;

        /**
         * @brief 原始事件源，指向最初触发事件的UIElement
         */
        UIElement *originalSource = nullptr;

        /**
         * @brief 构造函数，初始化事件类型为RoutedEventType_Null，其他字段使用默认值
         */
        RoutedEventArgs() : eventType(RoutedEventType_Null) {}

        /**
         * @brief 构造函数，初始化事件类型为指定值，其他字段使用默认值
         */
        explicit RoutedEventArgs(RoutedEventType eventType) : eventType(eventType) {}
    };

    /**
     * @brief 路由事件类型
     * @note 第一个参数为注册事件监听器的元素，第二个参数为具体的事件参数
     */
    using RoutedEventHandler = EventHandler<UIElement, RoutedEventArgs>;
}

// Screen.h


namespace sw
{
    /**
     * @brief 屏幕相关
     */
    class Screen
    {
    private:
        /**
         * @brief 静态类，禁止实例化
         */
        Screen() = delete;

    public:
        /**
         * @brief 主屏幕宽度
         */
        static const ReadOnlyProperty<double> Width;

        /**
         * @brief 主屏幕高度
         */
        static const ReadOnlyProperty<double> Height;

        /**
         * @brief 主屏幕尺寸
         */
        static const ReadOnlyProperty<sw::Size> Size;

        /**
         * @brief 虚拟屏幕尺寸
         */
        static const ReadOnlyProperty<sw::Size> VirtualSize;

        /**
         * @brief 虚拟屏幕原点坐标
         */
        static const ReadOnlyProperty<Point> VirtualOrigin;

        /**
         * @brief 鼠标在屏幕中的位置
         */
        static const ReadOnlyProperty<Point> CursorPosition;
    };
}

// Utils.h


namespace sw
{
    /**
     * @brief 工具类
     */
    class Utils
    {
    private:
        /**
         * @brief 静态类，不允许实例化
         */
        Utils() = delete;

    public:
        /**
         * @brief 设置Utils类是否使用UTF-8编码进行字符串转换（默认启用）
         * @param useUtf8 若为true则使用UTF-8编码，否则使用系统默认编码
         */
        static void UseUtf8Encoding(bool useUtf8);

        /**
         * @brief 将窄字符串转为宽字符串
         * @param str 要转换的字符串
         * @return 转换后的字符串
         */
        static std::wstring ToWideStr(const std::string &str);

        /**
         * @brief 将宽字符串转为窄字符串
         * @param wstr 要转换的字符串
         * @return 转换后的字符串
         */
        static std::string ToMultiByteStr(const std::wstring &wstr);

        /**
         * @brief 将窄字符串转为宽字符串
         * @param str 要转换的字符串
         * @param utf8 是否使用utf8编码
         * @return 转换后的字符串
         */
        static std::wstring ToWideStr(const std::string &str, bool utf8);

        /**
         * @brief 将宽字符串转为窄字符串
         * @param wstr 要转换的字符串
         * @param utf8 是否使用utf8编码
         * @return 转换后的字符串
         */
        static std::string ToMultiByteStr(const std::wstring &wstr, bool utf8);

        /**
         * @brief 删除首尾空白字符
         * @param str 输入的字符串
         * @return 删除首位空白字符后的字符串
         */
        static std::wstring Trim(const std::wstring &str);

        /**
         * @brief 删除串首空白字符
         * @param str 输入的字符串
         * @return 删除串首空白字符后的字符串
         */
        static std::wstring TrimStart(const std::wstring &str);

        /**
         * @brief 删除串尾空白字符
         * @param str 输入的字符串
         * @return 删除串尾空白字符后的字符串
         */
        static std::wstring TrimEnd(const std::wstring &str);

        /**
         * @brief 对字符串按照指定分隔符进行拆分
         * @param str 输入的字符串
         * @param delimiter 分隔符
         * @result 包含字串的vector
         */
        static std::vector<std::wstring> Split(const std::wstring &str, const std::wstring &delimiter);

        /**
         * @brief 格式化字符串，类似于 `swprintf`，但返回一个动态分配的 `std::wstring`
         * @param fmt 格式化字符串
         * @param ... 可变参数，符合 `fmt` 格式的输入
         * @return 返回一个包含格式化结果的字符串
         */
        static std::wstring FormatStr(const wchar_t *fmt, ...);

    public:
        /**
         * @brief 取两值中的较大值
         * @note 避免std::max与windows.h中的宏冲突，框架中使用此函数作为替代
         */
        template <typename T>
        static constexpr auto Max(T a, T b) noexcept
            -> typename std::enable_if<std::is_scalar<T>::value, T>::type
        { return a > b ? a : b; }

        /**
         * @brief 取两值中的较大值
         * @note 避免std::max与windows.h中的宏冲突，框架中使用此函数作为替代
         */
        template <typename T>
        static auto Max(const T &a, const T &b)
            -> typename std::enable_if<!std::is_scalar<T>::value, T>::type
        { return a > b ? a : b; }

        /**
         * @brief 取两值中的较小值
         * @note 避免std::min与windows.h中的宏冲突，框架中使用此函数作为替代
         */
        template <typename T>
        static constexpr auto Min(T a, T b) noexcept
            -> typename std::enable_if<std::is_scalar<T>::value, T>::type
        { return a < b ? a : b; }

        /**
         * @brief 取两值中的较小值
         * @note 避免std::min与windows.h中的宏冲突，框架中使用此函数作为替代
         */
        template <typename T>
        static auto Min(const T &a, const T &b)
            -> typename std::enable_if<!std::is_scalar<T>::value, T>::type
        { return a < b ? a : b; }

        /**
         * @brief 拼接字符串，也可使用此函数将其他类型转为wstring
         */
        template <typename... Args>
        static std::wstring BuildStr(const Args &...args)
        {
            std::wstringstream wss;
            int _[]{(_BuildStr(wss, args), 0)...};
            return wss.str();
        }

    private:
        /**
         * @brief BuildStr函数内部实现
         */
        template <typename T>
        static auto _BuildStr(std::wostream &wos, const T &arg)
            -> typename std::enable_if<!_IsProperty<T>::value && !_HasToString<T>::value>::type
        { wos << arg; }

        /**
         * @brief 让BuildStr函数支持自定义类型
         */
        template <typename T>
        static auto _BuildStr(std::wostream &wos, const T &arg)
            -> typename std::enable_if<!_IsProperty<T>::value && _HasToString<T>::value>::type
        { _BuildStr(wos, arg.ToString()); }

        /**
         * @brief 让BuildStr函数支持属性
         */
        template <typename T>
        static auto _BuildStr(std::wostream &wos, const T &prop)
            -> typename std::enable_if<_IsProperty<T>::value>::type
        { _BuildStr(wos, prop.Get()); }

        /**
         * @brief 让BuildStr函数将bool类型转化为"true"或"false"而不是数字1或0
         */
        static void _BuildStr(std::wostream &wos, bool b)
        { wos << (b ? L"true" : L"false"); }

        /**
         * @brief 让BuildStr函数支持窄字符串
         */
        static void _BuildStr(std::wostream &wos, const char *str)
        { wos << ToWideStr(str); }

        /**
         * @brief 让BuildStr函数支持窄字符串
         */
        static void _BuildStr(std::wostream &wos, const std::string &str)
        { wos << ToWideStr(str); }

        /**
         * @brief 让BuildStr函数支持std::vector
         */
        template <typename T>
        static void _BuildStr(std::wostream &wos, const std::vector<T> &vec)
        {
            auto beg = vec.begin();
            auto end = vec.end();
            wos << L"[";
            for (auto it = beg; it != end; ++it) {
                if (it != beg)
                    wos << L", ";
                _BuildStr(wos, *it);
            }
            wos << L"]";
        }

        /**
         * @brief 让BildStr函数支持std::map
         */
        template <typename TKey, typename TVal>
        static void _BuildStr(std::wostream &wos, const std::map<TKey, TVal> &map)
        {
            auto beg = map.begin();
            auto end = map.end();
            wos << L"{";
            for (auto it = beg; it != end; ++it) {
                if (it != beg)
                    wos << L", ";
                _BuildStr(wos, it->first);
                wos << L":";
                _BuildStr(wos, it->second);
            }
            wos << L"}";
        }

        /**
         * @brief 让BildStr函数支持std::unordered_map
         */
        template <typename TKey, typename TVal>
        static void _BuildStr(std::wostream &wos, const std::unordered_map<TKey, TVal> &map)
        {
            auto beg = map.begin();
            auto end = map.end();
            wos << L"{";
            for (auto it = beg; it != end; ++it) {
                if (it != beg)
                    wos << L", ";
                _BuildStr(wos, it->first);
                wos << L":";
                _BuildStr(wos, it->second);
            }
            wos << L"}";
        }
    };
}

// Converters.h


// clang-format off

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4996) // make sscanf great again
#endif

// clang-format on

namespace sw
{
    /**
     * @brief 反向转换器模板声明
     * @tparam TBaseConverter 基础转换器类型
     */
    template <typename TBaseConverter, typename = void>
    class ReverseConverter;

    /**
     * @brief 反向转换器，将已有转换器的转换方向反转
     * @tparam TBaseConverter 基础转换器类型
     */
    template <typename TBaseConverter>
    class ReverseConverter<
        TBaseConverter,
        typename std::enable_if<
            std::is_base_of<
                IValueConverter<
                    typename std::decay<typename TBaseConverter::TSourceParam>::type,
                    typename std::decay<typename TBaseConverter::TTargetParam>::type>,
                TBaseConverter>::value>::type>
        : public IValueConverter<
              typename std::decay<typename TBaseConverter::TTargetParam>::type,
              typename std::decay<typename TBaseConverter::TSourceParam>::type>
    {
    public:
        using TSourceParam = typename TBaseConverter::TTargetParam;
        using TTargetParam = typename TBaseConverter::TSourceParam;
        using TSource      = typename std::decay<TSourceParam>::type;
        using TTarget      = typename std::decay<TTargetParam>::type;

        template <typename... Args>
        ReverseConverter(Args &&...args)
            : baseConverter(std::forward<Args>(args)...)
        {
        }
        virtual TTarget Convert(TSourceParam source) override
        {
            return baseConverter.ConvertBack(source);
        }
        virtual TSource ConvertBack(TTargetParam target) override
        {
            return baseConverter.Convert(target);
        }

    private:
        TBaseConverter baseConverter;
    };

    /*================================================================================*/

    /**
     * @brief 数值类型转换器模板声明
     * @tparam TSource 源数值类型
     * @tparam TTarget 目标数值类型
     */
    template <typename TSource, typename TTarget, typename = void>
    class NumericConverter;

    /**
     * @brief 数值类型转换器，将源数值类型转换为目标数值类型
     * @tparam TSource 源数值类型
     * @tparam TTarget 目标数值类型
     */
    template <typename TSource, typename TTarget>
    class NumericConverter<TSource,
                           TTarget,
                           typename std::enable_if<std::is_arithmetic<TSource>::value && std::is_arithmetic<TTarget>::value>::type>
        : public IValueConverter<TSource, TTarget>
    {
    public:
        virtual TTarget Convert(TSource source) override
        {
            return static_cast<TTarget>(source);
        }
        virtual TSource ConvertBack(TTarget target) override
        {
            return static_cast<TSource>(target);
        }
    };

    using IntToFloatConverter  = NumericConverter<int, float>;
    using IntToDoubleConverter = NumericConverter<int, double>;
    using FloatToIntConverter  = NumericConverter<float, int>;
    using DoubleToIntConverter = NumericConverter<double, int>;

    /*================================================================================*/

    /**
     * @brief 数值取反转换器模板声明
     * @tparam T 数值类型
     */
    template <typename T, typename = void>
    class NumericNegationConverter;

    /**
     * @brief 数值取反转换器
     * @tparam T 数值类型
     */
    template <typename T>
    class NumericNegationConverter<T, typename std::enable_if<std::is_arithmetic<T>::value>::type>
        : public IValueConverter<T, T>
    {
    public:
        virtual T Convert(T source) override
        {
            return -source;
        }
        virtual T ConvertBack(T target) override
        {
            return -target;
        }
    };

    /*================================================================================*/

    /**
     * @brief 布尔值取反转换器
     */
    class BoolNegationConverter : public IValueConverter<bool, bool>
    {
    public:
        virtual bool Convert(bool source) override
        {
            return !source;
        }
        virtual bool ConvertBack(bool target) override
        {
            return !target;
        }
    };

    /**
     * @brief 数值与布尔值转换器模板声明
     * @tparam TSource 数值类型
     */
    template <typename TSource, typename = void>
    class NumericToBoolConverter;

    /**
     * @brief 数值与布尔值转换器，非零数值转换为true，零数值转换为false
     * @tparam TSource 数值类型
     */
    template <typename TSource>
    class NumericToBoolConverter<TSource, typename std::enable_if<std::is_arithmetic<TSource>::value>::type>
        : public IValueConverter<TSource, bool>
    {
    public:
        virtual bool Convert(TSource source) override
        {
            return source != static_cast<TSource>(0);
        }
        virtual TSource ConvertBack(bool target) override
        {
            return target ? static_cast<TSource>(1) : static_cast<TSource>(0);
        }
    };

    /**
     * @brief 布尔值与数值转换器模板声明
     * @tparam TTarget 数值类型
     */
    template <typename TTarget, typename = void>
    class BoolToNumericConverter;

    /**
     * @brief 布尔值与数值转换器，true转换为1，false转换为0
     * @tparam TTarget 数值类型
     */
    template <typename TTarget>
    class BoolToNumericConverter<TTarget, typename std::enable_if<std::is_arithmetic<TTarget>::value>::type>
        : public IValueConverter<bool, TTarget>
    {
    public:
        virtual TTarget Convert(bool source) override
        {
            return source ? static_cast<TTarget>(1) : static_cast<TTarget>(0);
        }
        virtual bool ConvertBack(TTarget target) override
        {
            return target != static_cast<TTarget>(0);
        }
    };

    using BoolToIntConverter = BoolToNumericConverter<int>;
    using IntToBoolConverter = NumericToBoolConverter<int>;

    /*================================================================================*/

    /**
     * @brief 枚举类型与数值类型转换器模板声明
     * @tparam TSource 枚举类型
     * @tparam TTarget 数值类型
     */
    template <typename TSource, typename TTarget, typename = void>
    class EnumToNumericConverter;

    /**
     * @brief 枚举类型与数值类型转换器
     * @tparam TSource 枚举类型
     * @tparam TTarget 数值类型
     */
    template <typename TSource, typename TTarget>
    class EnumToNumericConverter<TSource,
                                 TTarget,
                                 typename std::enable_if<std::is_enum<TSource>::value && std::is_arithmetic<TTarget>::value>::type>
        : public IValueConverter<TSource, TTarget>
    {
    public:
        virtual TTarget Convert(TSource source) override
        {
            return static_cast<TTarget>(source);
        }
        virtual TSource ConvertBack(TTarget target) override
        {
            return static_cast<TSource>(target);
        }
    };

    /**
     * @brief 数值类型与枚举类型转换器模板声明
     * @tparam TSource 数值类型
     * @tparam TTarget 枚举类型
     */
    template <typename TSource, typename TTarget, typename = void>
    class NumericToEnumConverter;

    /**
     * @brief 数值类型与枚举类型转换器
     * @tparam TSource 数值类型
     * @tparam TTarget 枚举类型
     */
    template <typename TSource, typename TTarget>
    class NumericToEnumConverter<TSource,
                                 TTarget,
                                 typename std::enable_if<std::is_arithmetic<TSource>::value && std::is_enum<TTarget>::value>::type>
        : public IValueConverter<TSource, TTarget>
    {
    public:
        virtual TTarget Convert(TSource source) override
        {
            return static_cast<TTarget>(source);
        }
        virtual TSource ConvertBack(TTarget target) override
        {
            return static_cast<TSource>(target);
        }
    };

    template <typename TEnum>
    using EnumToIntConverter = EnumToNumericConverter<TEnum, int>;

    template <typename TEnum>
    using IntToEnumConverter = NumericToEnumConverter<int, TEnum>;

    /*================================================================================*/

    /**
     * @brief 整数与字符串转换器
     */
    class IntToStringConverter : public IValueConverter<int, std::wstring>
    {
    public:
        explicit IntToStringConverter(int defaultValue = 0)
            : defaultValue(defaultValue)
        {
        }
        virtual std::wstring Convert(int source) override
        {
            return std::to_wstring(source);
        }
        virtual int ConvertBack(const std::wstring &target) override
        {
            int result;
            bool success = std::swscanf(target.c_str(), L"%d", &result) == 1;
            return success ? result : defaultValue;
        }

    private:
        int defaultValue;
    };

    /**
     * @brief 浮点数与字符串转换器
     */
    class FloatToStringConverter : public IValueConverter<float, std::wstring>
    {
    public:
        explicit FloatToStringConverter(float defaultValue = 0.0f)
            : defaultValue(defaultValue)
        {
        }
        virtual std::wstring Convert(float source) override
        {
            std::wstring result(32, L'\0');
            int len = std::swprintf(&result[0], result.size(), L"%g", source);
            result.resize(len > 0 ? len : 0);
            return result;
        }
        virtual float ConvertBack(const std::wstring &target) override
        {
            float result;
            bool success = std::swscanf(target.c_str(), L"%f", &result) == 1;
            return success ? result : defaultValue;
        }

    private:
        float defaultValue;
    };

    /**
     * @brief 双精度浮点数与字符串转换器
     */
    class DoubleToStringConverter : public IValueConverter<double, std::wstring>
    {
    public:
        explicit DoubleToStringConverter(double defaultValue = 0.0)
            : defaultValue(defaultValue)
        {
        }
        virtual std::wstring Convert(double source) override
        {
            std::wstring result(32, L'\0');
            int len = std::swprintf(&result[0], result.size(), L"%g", source);
            result.resize(len > 0 ? len : 0);
            return result;
        }
        virtual double ConvertBack(const std::wstring &target) override
        {
            double result;
            bool success = std::swscanf(target.c_str(), L"%lf", &result) == 1;
            return success ? result : defaultValue;
        }

    private:
        double defaultValue;
    };

    /**
     * @brief 字符串与整数转换器
     */
    class StringToIntConverter : public IValueConverter<std::wstring, int>
    {
    public:
        explicit StringToIntConverter(int defaultValue = 0)
            : defaultValue(defaultValue)
        {
        }
        virtual int Convert(const std::wstring &source) override
        {
            int result;
            bool success = std::swscanf(source.c_str(), L"%d", &result) == 1;
            return success ? result : defaultValue;
        }
        virtual std::wstring ConvertBack(int target) override
        {
            return std::to_wstring(target);
        }

    private:
        int defaultValue;
    };

    /**
     * @brief 字符串与浮点数转换器
     */
    class StringToFloatConverter : public IValueConverter<std::wstring, float>
    {
    public:
        explicit StringToFloatConverter(float defaultValue = 0.0f)
            : defaultValue(defaultValue)
        {
        }
        virtual float Convert(const std::wstring &source) override
        {
            float result;
            bool success = std::swscanf(source.c_str(), L"%f", &result) == 1;
            return success ? result : defaultValue;
        }
        virtual std::wstring ConvertBack(float target) override
        {
            std::wstring result(32, L'\0');
            int len = std::swprintf(&result[0], result.size(), L"%g", target);
            result.resize(len > 0 ? len : 0);
            return result;
        }

    private:
        float defaultValue;
    };

    /**
     * @brief 字符串与双精度浮点数转换器
     */
    class StringToDoubleConverter : public IValueConverter<std::wstring, double>
    {
    public:
        explicit StringToDoubleConverter(double defaultValue = 0.0)
            : defaultValue(defaultValue)
        {
        }
        virtual double Convert(const std::wstring &source) override
        {
            double result;
            bool success = std::swscanf(source.c_str(), L"%lf", &result) == 1;
            return success ? result : defaultValue;
        }
        virtual std::wstring ConvertBack(double target) override
        {
            std::wstring result(32, L'\0');
            int len = std::swprintf(&result[0], result.size(), L"%g", target);
            result.resize(len > 0 ? len : 0);
            return result;
        }

    private:
        double defaultValue;
    };

    /*================================================================================*/

    /**
     * @brief 整数与窄字符串转换器
     */
    class IntToAnsiStringConverter : public IValueConverter<int, std::string>
    {
    public:
        explicit IntToAnsiStringConverter(int defaultValue = 0)
            : defaultValue(defaultValue)
        {
        }
        virtual std::string Convert(int source) override
        {
            return std::to_string(source);
        }
        virtual int ConvertBack(const std::string &target) override
        {
            int result;
            bool success = std::sscanf(target.c_str(), "%d", &result) == 1;
            return success ? result : defaultValue;
        }

    private:
        int defaultValue;
    };

    /**
     * @brief 浮点数与窄字符串转换器
     */
    class FloatToAnsiStringConverter : public IValueConverter<float, std::string>
    {
    public:
        explicit FloatToAnsiStringConverter(float defaultValue = 0.0f)
            : defaultValue(defaultValue)
        {
        }
        virtual std::string Convert(float source) override
        {
            std::string result(32, '\0');
            int len = std::snprintf(&result[0], result.size(), "%g", source);
            result.resize(len > 0 ? len : 0);
            return result;
        }
        virtual float ConvertBack(const std::string &target) override
        {
            float result;
            bool success = std::sscanf(target.c_str(), "%f", &result) == 1;
            return success ? result : defaultValue;
        }

    private:
        float defaultValue;
    };

    /**
     * @brief 双精度浮点数与窄字符串转换器
     */
    class DoubleToAnsiStringConverter : public IValueConverter<double, std::string>
    {
    public:
        explicit DoubleToAnsiStringConverter(double defaultValue = 0.0)
            : defaultValue(defaultValue)
        {
        }
        virtual std::string Convert(double source) override
        {
            std::string result(32, '\0');
            int len = std::snprintf(&result[0], result.size(), "%g", source);
            result.resize(len > 0 ? len : 0);
            return result;
        }
        virtual double ConvertBack(const std::string &target) override
        {
            double result;
            bool success = std::sscanf(target.c_str(), "%lf", &result) == 1;
            return success ? result : defaultValue;
        }

    private:
        double defaultValue;
    };

    /**
     * @brief 窄字符串与整数转换器
     */
    class AnsiStringToIntConverter : public IValueConverter<std::string, int>
    {
    public:
        explicit AnsiStringToIntConverter(int defaultValue = 0)
            : defaultValue(defaultValue)
        {
        }
        virtual int Convert(const std::string &source) override
        {
            int result;
            bool success = std::sscanf(source.c_str(), "%d", &result) == 1;
            return success ? result : defaultValue;
        }
        virtual std::string ConvertBack(int target) override
        {
            return std::to_string(target);
        }

    private:
        int defaultValue;
    };

    /**
     * @brief 窄字符串与浮点数转换器
     */
    class AnsiStringToFloatConverter : public IValueConverter<std::string, float>
    {
    public:
        explicit AnsiStringToFloatConverter(float defaultValue = 0.0f)
            : defaultValue(defaultValue)
        {
        }
        virtual float Convert(const std::string &source) override
        {
            float result;
            bool success = std::sscanf(source.c_str(), "%f", &result) == 1;
            return success ? result : defaultValue;
        }
        virtual std::string ConvertBack(float target) override
        {
            std::string result(32, '\0');
            int len = std::snprintf(&result[0], result.size(), "%g", target);
            result.resize(len > 0 ? len : 0);
            return result;
        }

    private:
        float defaultValue;
    };

    /**
     * @brief 窄字符串与双精度浮点数转换器
     */
    class AnsiStringToDoubleConverter : public IValueConverter<std::string, double>
    {
    public:
        explicit AnsiStringToDoubleConverter(double defaultValue = 0.0)
            : defaultValue(defaultValue)
        {
        }
        virtual double Convert(const std::string &source) override
        {
            double result;
            bool success = std::sscanf(source.c_str(), "%lf", &result) == 1;
            return success ? result : defaultValue;
        }
        virtual std::string ConvertBack(double target) override
        {
            std::string result(32, '\0');
            int len = std::snprintf(&result[0], result.size(), "%g", target);
            result.resize(len > 0 ? len : 0);
            return result;
        }

    private:
        double defaultValue;
    };

    /*================================================================================*/

    /**
     * @brief 宽字符串与窄字符串转换器
     */
    class StringToAnsiStringConverter : public IValueConverter<std::wstring, std::string>
    {
    public:
        virtual std::string Convert(const std::wstring &source) override
        {
            return Utils::ToMultiByteStr(source);
        }
        virtual std::wstring ConvertBack(const std::string &target) override
        {
            return Utils::ToWideStr(target);
        }
    };

    /**
     * @brief 窄字符串与宽字符串转换器
     */
    class AnsiStringToStringConverter : public IValueConverter<std::string, std::wstring>
    {
    public:
        virtual std::wstring Convert(const std::string &source) override
        {
            return Utils::ToWideStr(source);
        }
        virtual std::string ConvertBack(const std::wstring &target) override
        {
            return Utils::ToMultiByteStr(target);
        }
    };

    /**
     * @brief 宽字符串与UTF-8编码窄字符串转换器
     */
    class StringToUtf8StringConverter : public IValueConverter<std::wstring, std::string>
    {
    public:
        virtual std::string Convert(const std::wstring &source) override
        {
            return Utils::ToMultiByteStr(source, true);
        }
        virtual std::wstring ConvertBack(const std::string &target) override
        {
            return Utils::ToWideStr(target, true);
        }
    };

    /**
     * @brief UTF-8编码窄字符串与宽字符串转换器
     */
    class Utf8StringToStringConverter : public IValueConverter<std::string, std::wstring>
    {
    public:
        virtual std::wstring Convert(const std::string &source) override
        {
            return Utils::ToWideStr(source, true);
        }
        virtual std::string ConvertBack(const std::wstring &target) override
        {
            return Utils::ToMultiByteStr(target, true);
        }
    };
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif

// EventHandlerWrapper.h


namespace sw
{
    /**
     * @brief 路由事件处理函数包装类前置声明
     * @note 第二个模板参数用于SFINAE，只有当TEventArgs继承自RoutedEventArgs时才启用具体实现
     */
    template <typename TEventArgs, typename = void>
    class RoutedEventHandlerWrapper;

    /**
     * @brief 路由事件处理函数包装类，用于需要转换RoutedEventArgs为特定事件参数类型的情况
     */
    template <typename TEventArgs>
    class RoutedEventHandlerWrapper<
        TEventArgs,
        typename std::enable_if<
            std::is_base_of<RoutedEventArgs, TEventArgs>::value>::type>
        : public ICallable<void(UIElement &, RoutedEventArgs &)>
    {
    private:
        /**
         * @brief 事件处理函数
         */
        Action<UIElement &, TEventArgs &> _handler;

    public:
        /**
         * @brief 构造函数，拷贝事件处理函数
         * @param handler 事件处理函数
         */
        RoutedEventHandlerWrapper(const Action<UIElement &, TEventArgs &> &handler)
            : _handler(handler)
        {
        }

        /**
         * @brief 构造函数，移动事件处理函数
         * @param handler 事件处理函数
         */
        RoutedEventHandlerWrapper(Action<UIElement &, TEventArgs &> &&handler)
            : _handler(std::move(handler))
        {
        }

        /**
         * @brief 拷贝构造函数
         * @param other 要拷贝的路由事件处理函数包装对象
         */
        RoutedEventHandlerWrapper(const RoutedEventHandlerWrapper &other)
            : _handler(other._handler)
        {
        }

        /**
         * @brief 移动构造函数
         * @param other 要移动的路由事件处理函数包装对象
         */
        RoutedEventHandlerWrapper(RoutedEventHandlerWrapper &&other) noexcept
            : _handler(std::move(other._handler))
        {
        }

        /**
         * @brief 调用事件处理函数
         * @note 调用方根据路由事件类型保证args的实际类型为TEventArgs
         */
        virtual void Invoke(UIElement &sender, RoutedEventArgs &args) const override
        {
            if (_handler) _handler(sender, static_cast<TEventArgs &>(args));
        }

        /**
         * @brief 克隆当前可调用对象
         */
        virtual ICallable<void(UIElement &, RoutedEventArgs &)> *Clone() const override
        {
            return new RoutedEventHandlerWrapper(*this);
        }

        /**
         * @brief 获取当前可调用对象的类型信息
         */
        virtual std::type_index GetType() const override
        {
            return typeid(RoutedEventHandlerWrapper<TEventArgs>);
        }

        /**
         * @brief 判断当前可调用对象是否与另一个可调用对象相等
         * @param other 另一个可调用对象
         * @return 如果相等则返回true，否则返回false
         */
        virtual bool Equals(const ICallable<void(UIElement &, RoutedEventArgs &)> &other) const override
        {
            if (this == &other) {
                return true;
            }
            if (GetType() != other.GetType()) {
                return false;
            }
            const auto &otherWrapper = static_cast<const RoutedEventHandlerWrapper &>(other);
            return _handler.Equals(otherWrapper._handler);
        }
    };
}

// ILayout.h


namespace sw
{
    /**
     * @brief 布局接口
     */
    class ILayout
    {
    public:
        /**
         * @brief 默认虚析构函数
         */
        virtual ~ILayout() = default;

    public:
        /**
         * @brief 获取布局标记
         */
        virtual uint64_t GetLayoutTag() const = 0;

        /**
         * @brief 获取控件所需尺寸
         */
        virtual Size GetDesireSize() const = 0;

        /**
         * @brief 获取子控件的数量
         */
        virtual int GetChildLayoutCount() const = 0;

        /**
         * @brief 获取对应索引处的子控件
         * @throw std::out_of_range 如果索引超出范围
         */
        virtual ILayout &GetChildLayoutAt(int index) const = 0;

        /**
         * @brief 测量控件所需尺寸
         * @param availableSize 可用的尺寸
         */
        virtual void Measure(const Size &availableSize) = 0;

        /**
         * @brief 安排控件位置
         * @param finalPosition 最终控件所安排的位置
         */
        virtual void Arrange(const Rect &finalPosition) = 0;
    };
}

// INotifyPropertyChanged.h


namespace sw
{
    class INotifyPropertyChanged; // 向前声明

    /**
     * @brief 属性更改事件参数
     */
    struct PropertyChangedEventArgs : EventArgs {
        /**
         * @brief 发生更改的属性ID
         */
        FieldId propertyId;
    };

    /**
     * @brief 属性更改事件处理函数类型
     */
    using PropertyChangedEventHandler =
        EventHandler<INotifyPropertyChanged, PropertyChangedEventArgs>;

    /**
     * @brief 属性变更通知接口
     */
    class INotifyPropertyChanged
    {
    public:
        /**
         * @brief 当属性值更改时触发的事件
         */
        const Event<PropertyChangedEventHandler> PropertyChanged{
            Event<PropertyChangedEventHandler>::Init(this)
                .Delegate([](INotifyPropertyChanged *self) -> PropertyChangedEventHandler & {
                    return self->GetPropertyChangedEventDelegate();
                }) //
        };

        /**
         * @brief 默认析构函数
         */
        virtual ~INotifyPropertyChanged() = default;

    protected:
        /**
         * @brief 获取属性更改事件委托的引用
         * @note PropertyChanged事件使用该函数返回的委托来保存事件处理程序
         */
        virtual PropertyChangedEventHandler &GetPropertyChangedEventDelegate() = 0;
    };
}

// RoutedEventArgs.h


namespace sw
{
    /**
     * @brief 表示特定类型路由事件的事件参数类型，继承自该类的类型可以直接作为AddHandler函数的模板参数
     * @tparam Type 路由事件类型，必须为RoutedEventType枚举中的值
     * @tparam TBase 事件参数类型的基类，默认为RoutedEventArgs
     */
    template <RoutedEventType Type, typename TBase = RoutedEventArgs>
    class TypedRoutedEventArgs : public TBase
    {
        // TBase必须派生自RoutedEventArgs
        static_assert(
            std::is_base_of<RoutedEventArgs, TBase>::value,
            "TBase must be derived from RoutedEventArgs.");

    public:
        /// @brief 路由事件的类型，AddHandler模板函数使用此字段注册事件
        static constexpr RoutedEventType EventType = Type;

        /// @brief 构造函数，初始化事件类型为EventType
        TypedRoutedEventArgs()
        { this->eventType = EventType; }
    };

    /*================================================================================*/

    /**
     * @brief 结构体模板，用于检测类型T是否含有名为EventType的静态字段
     */
    template <typename T, typename = void>
    struct _HasEventType : std::false_type {
    };

    /**
     * @brief 模板特化：当T包含EventType时，将_IsTypedRoutedEventArgs<T>设为std::true_type
     */
    template <typename T>
    struct _HasEventType<T, decltype(void(T::EventType))> : std::true_type {
    };

    /**
     * @brief 结构体模板，用于检测类型T是否包含事件类型信息
     */
    template <typename T>
    struct _IsTypedRoutedEventArgs : _HasEventType<T> {
    };

    /*================================================================================*/

    /**
     * @brief 尺寸改变事件参数类型
     */
    class SizeChangedEventArgs : public TypedRoutedEventArgs<UIElement_SizeChanged>
    {
    public:
        /// @brief 用户区的新尺寸
        Size newClientSize;

        /// @brief 构造函数
        /// @param newClientSize 用户区的新尺寸
        SizeChangedEventArgs(Size newClientSize) : newClientSize(newClientSize)
        {
        }
    };

    /**
     * @brief 位置改变事件参数类型
     */
    class PositionChangedEventArgs : public TypedRoutedEventArgs<UIElement_PositionChanged>
    {
    public:
        /// @brief 移动后用户区左上角的位置
        Point newClientPosition;

        /// @brief 构造函数
        /// @param newClientPosition 移动后用户区左上角的位置
        PositionChangedEventArgs(Point newClientPosition) : newClientPosition(newClientPosition)
        {
        }
    };

    /**
     * @brief 输入字符事件类型参数
     */
    class GotCharEventArgs : public TypedRoutedEventArgs<UIElement_GotChar>
    {
    public:
        /// @brief 输入的字符
        wchar_t ch;

        /// @brief 附加信息
        KeyFlags flags;

        /// @brief 构造函数
        /// @param ch 输入的字符
        /// @param flags 附加信息
        GotCharEventArgs(wchar_t ch, KeyFlags flags) : ch(ch), flags(flags)
        {
        }
    };

    /**
     * @brief 键盘事件参数类型模板
     * @tparam Type 路由事件类型，必须为RoutedEventType枚举中的值
     */
    template <RoutedEventType Type>
    class KeyEventArgs : public TypedRoutedEventArgs<Type>
    {
    public:
        /// @brief 虚拟按键
        VirtualKey key;

        /// @brief 附加信息
        KeyFlags flags;

        /// @brief 构造函数
        /// @param key 虚拟按键
        /// @param flags 附加信息
        KeyEventArgs(VirtualKey key, KeyFlags flags) : key(key), flags(flags)
        {
        }
    };

    /**
     * @brief 键盘按键按下事件参数类型
     */
    class KeyDownEventArgs : public KeyEventArgs<UIElement_KeyDown>
    {
    public:
        /// @brief 继承基类构造函数
        using KeyEventArgs<UIElement_KeyDown>::KeyEventArgs;
    };

    /**
     * @brief 键盘按键抬起事件参数类型
     */
    class KeyUpEventArgs : public KeyEventArgs<UIElement_KeyUp>
    {
    public:
        /// @brief 继承基类构造函数
        using KeyEventArgs<UIElement_KeyUp>::KeyEventArgs;
    };

    /**
     * @brief 鼠标事件参数类型模板
     * @tparam Type 路由事件类型，必须为RoutedEventType枚举中的值
     */
    template <RoutedEventType Type>
    class MouseEventArgs : public TypedRoutedEventArgs<Type>
    {
    public:
        /// @brief 鼠标位置
        Point mousePosition;

        /// @brief 按键状态
        MouseKey keyState;

        /// @brief 构造函数
        /// @param mousePosition 鼠标位置
        /// @param keyState 按键状态
        MouseEventArgs(Point mousePosition, MouseKey keyState)
            : mousePosition(mousePosition), keyState(keyState)
        {
        }
    };

    /**
     * @brief 鼠标移动事件参数类型
     */
    class MouseMoveEventArgs : public MouseEventArgs<UIElement_MouseMove>
    {
    public:
        /// @brief 继承基类构造函数
        using MouseEventArgs<UIElement_MouseMove>::MouseEventArgs;
    };

    /**
     * @brief 鼠标滚轮滚动事件参数类型
     */
    class MouseWheelEventArgs : public MouseEventArgs<UIElement_MouseWheel>
    {
    public:
        /// @brief 滚轮滚动的距离，为120的倍数
        int wheelDelta;

        /// @brief 构造函数
        /// @param wheelDelta 滚轮滚动的距离，为120的倍数
        /// @param mousePosition 鼠标位置
        /// @param keyState 按键状态
        MouseWheelEventArgs(int wheelDelta, Point mousePosition, MouseKey keyState)
            : MouseEventArgs<UIElement_MouseWheel>(mousePosition, keyState), wheelDelta(wheelDelta)
        {
        }
    };

    /**
     * @brief 鼠标按键事件参数类型模板
     * @tparam Type 路由事件类型，必须为RoutedEventType枚举中的值
     */
    template <RoutedEventType Type>
    class MouseButtonEventArgs : public MouseEventArgs<Type>
    {
    public:
        /// @brief 按下/抬起的按键（左键、中间、右键）
        MouseKey key;

        /// @brief 构造函数
        /// @param key 按下/抬起的按键（左键、中间、右键）
        /// @param mousePosition 鼠标位置
        /// @param keyState 按键状态
        MouseButtonEventArgs(MouseKey key, Point mousePosition, MouseKey keyState)
            : MouseEventArgs<Type>(mousePosition, keyState), key(key)
        {
        }
    };

    /**
     * @brief 鼠标按键按下事件参数类型
     */
    class MouseButtonDownEventArgs : public MouseButtonEventArgs<UIElement_MouseButtonDown>
    {
    public:
        /// @brief 继承基类构造函数
        using MouseButtonEventArgs<UIElement_MouseButtonDown>::MouseButtonEventArgs;
    };

    /**
     * @brief 鼠标按键抬起事件参数类型
     */
    class MouseButtonUpEventArgs : public MouseButtonEventArgs<UIElement_MouseButtonUp>
    {
    public:
        /// @brief 继承基类构造函数
        using MouseButtonEventArgs<UIElement_MouseButtonUp>::MouseButtonEventArgs;
    };

    /**
     * @brief 可取消事件参数类型，包含一个cancel字段用于指示是否取消事件
     */
    class CancelableEventArgs : public RoutedEventArgs
    {
    public:
        /// @brief 是否取消事件，默认为false
        bool cancel = false;
    };

    /**
     * @brief 显示用户自定义上下文菜单的事件参数类型
     */
    class ShowContextMenuEventArgs : public TypedRoutedEventArgs<UIElement_ShowContextMenu, CancelableEventArgs>
    {
    public:
        /// @brief 消息是否由按下快捷键（Shift+F10、VK_APPS）产生
        bool isKeyboardMsg;

        /// @brief 鼠标在屏幕中的位置
        Point mousePosition;

        /// @brief 构造函数
        /// @param isKeyboardMsg 消息是否由按下快捷键（Shift+F10、VK_APPS）产生
        /// @param mousePosition 鼠标在屏幕中的位置
        ShowContextMenuEventArgs(bool isKeyboardMsg, Point mousePosition)
            : isKeyboardMsg(isKeyboardMsg), mousePosition(mousePosition)
        {
        }
    };

    /**
     * @brief 文件拖放事件参数类型
     */
    class DropFilesEventArgs : public TypedRoutedEventArgs<UIElement_DropFiles>
    {
    public:
        /// @brief 描述拖入文件的句柄
        HDROP hDrop;

        /// @brief 构造函数
        /// @param hDrop 描述拖入文件的句柄
        DropFilesEventArgs(HDROP hDrop) : hDrop(hDrop)
        {
        }
    };

    /**
     * @brief 窗口正在关闭事件参数类型
     */
    class WindowClosingEventArgs : public TypedRoutedEventArgs<Window_Closing, CancelableEventArgs>
    {
    };

    /**
     * @brief 窗口/面板滚动条滚动事件参数类型
     */
    class ScrollingEventArgs : public TypedRoutedEventArgs<Layer_Scrolling, CancelableEventArgs>
    {
    public:
        /// @brief 滚动条类型
        ScrollOrientation scrollbar;

        /// @brief 滚动条事件
        ScrollEvent event;

        /// @brief 当event为ThumbPosition或ThubmTrack时表示当前滚动条位置，其他情况固定为0
        double pos;

        /// @brief 构造函数
        /// @param scrollbar 滚动条类型
        /// @param event 滚动条事件
        /// @param pos 当event为ThumbPosition或ThubmTrack时表示当前滚动条位置，其他情况固定为0
        ScrollingEventArgs(ScrollOrientation scrollbar, ScrollEvent event, double pos)
            : scrollbar(scrollbar), event(event), pos(pos)
        {
        }
    };

    /**
     * @brief 列表视图某个项事件参数类型模板，适用于单击、双击等事件
     * @tparam Type 路由事件类型，必须为RoutedEventType枚举中的值
     * @tparam TBase 事件参数类型的基类，默认为RoutedEventArgs
     */
    template <RoutedEventType Type, typename TBase = RoutedEventArgs>
    class ListViewItemEventArgs : public TypedRoutedEventArgs<Type, TBase>
    {
    public:
        /// @brief 发生事件的项的索引
        int index;

        /// @brief 构造函数
        /// @param index 发生事件的项的索引
        ListViewItemEventArgs(int index) : index(index)
        {
        }
    };

    /**
     * @brief 列表视图某个子项事件参数类型模板，适用于单击、双击等事件
     * @tparam Type 路由事件类型，必须为RoutedEventType枚举中的值
     * @tparam TBase 事件参数类型的基类，默认为RoutedEventArgs
     */
    template <RoutedEventType Type, typename TBase = RoutedEventArgs>
    class ListViewSubItemEventArgs : public ListViewItemEventArgs<Type, TBase>
    {
    public:
        /// @brief 发生事件的子项的索引
        int subIndex;

        /// @brief 构造函数
        /// @param index 发生事件的项的索引
        /// @param subIndex 发生事件的子项的索引
        ListViewSubItemEventArgs(int index, int subIndex)
            : ListViewItemEventArgs<Type, TBase>(index), subIndex(subIndex)
        {
        }
    };

    /**
     * @brief 列表视图某个复选框选中状态改变的事件参数类型
     */
    class ListViewCheckStateChangedEventArgs : public ListViewItemEventArgs<ListView_CheckStateChanged>
    {
    public:
        /// @brief 继承基类构造函数
        using ListViewItemEventArgs<ListView_CheckStateChanged>::ListViewItemEventArgs;
    };

    /**
     * @brief 列表视图的列标题单击事件参数类型
     */
    class ListViewHeaderClickedEventArgs : public ListViewItemEventArgs<ListView_HeaderClicked>
    {
    public:
        /// @brief 继承基类构造函数
        using ListViewItemEventArgs<ListView_HeaderClicked>::ListViewItemEventArgs;
    };

    /**
     * @brief 列表视图的列标题双击事件参数类型
     */
    class ListViewHeaderDoubleClickedEventArgs : public ListViewItemEventArgs<ListView_HeaderDoubleClicked>
    {
    public:
        /// @brief 继承基类构造函数
        using ListViewItemEventArgs<ListView_HeaderDoubleClicked>::ListViewItemEventArgs;
    };

    /**
     * @brief 列表视图项单击事件参数类型
     */
    class ListViewItemClickedEventArgs : public ListViewSubItemEventArgs<ListView_ItemClicked>
    {
    public:
        /// @brief 继承基类构造函数
        using ListViewSubItemEventArgs<ListView_ItemClicked>::ListViewSubItemEventArgs;
    };

    /**
     * @brief 列表视图项双击事件参数类型
     */
    class ListViewItemDoubleClickedEventArgs : public ListViewSubItemEventArgs<ListView_ItemDoubleClicked>
    {
    public:
        /// @brief 继承基类构造函数
        using ListViewSubItemEventArgs<ListView_ItemDoubleClicked>::ListViewSubItemEventArgs;
    };

    /**
     * @brief 列表视图编辑状态结束事件参数类型
     */
    class ListViewEndEditEventArgs : public ListViewItemEventArgs<ListView_EndEdit, CancelableEventArgs>
    {
    public:
        /// @brief 新的文本
        std::wstring newText;

        /// @brief 构造函数
        /// @param index 发生事件的项的索引
        /// @param newText 新的文本
        ListViewEndEditEventArgs(int index, const std::wstring &newText)
            : ListViewItemEventArgs<ListView_EndEdit, CancelableEventArgs>(index), newText(newText)
        {
        }
    };

    /**
     * @brief 时间改变事件参数类型模板，适用于DateTimePicker和MonthCalendar控件
     * @tparam Type 路由事件类型，必须为RoutedEventType枚举中的值
     */
    template <RoutedEventType Type>
    class TimeChangedEventArgs : public TypedRoutedEventArgs<Type>
    {
    public:
        /// @brief 时间的新值
        SYSTEMTIME time;

        /// @brief 构造函数
        /// @param time 时间的新值
        TimeChangedEventArgs(const SYSTEMTIME &time) : time(time)
        {
        }
    };

    /**
     * @brief DateTimePicker控件时间改变事件参数类型
     */
    class DateTimePickerTimeChangedEventArgs : public TimeChangedEventArgs<DateTimePicker_TimeChanged>
    {
    public:
        /// @brief 继承基类构造函数
        using TimeChangedEventArgs<DateTimePicker_TimeChanged>::TimeChangedEventArgs;
    };

    /**
     * @brief 月历控件时间改变事件参数类型
     */
    class MonthCalendarTimeChangedEventArgs : public TimeChangedEventArgs<MonthCalendar_TimeChanged>
    {
    public:
        /// @brief 继承基类构造函数
        using TimeChangedEventArgs<MonthCalendar_TimeChanged>::TimeChangedEventArgs;
    };

    /**
     * @brief SysLink控件链接被单击事件参数类型
     */
    class SysLinkClickedEventArgs : public TypedRoutedEventArgs<SysLink_Clicked>
    {
    public:
        /// @brief 被单击链接的id
        std::wstring id;

        /// @brief 被单击链接的url（即href）
        std::wstring url;

        /// @brief 构造函数
        /// @param id 被单击链接的id
        /// @param url 被单击链接的url（即href）
        SysLinkClickedEventArgs(const std::wstring &id, const std::wstring &url) : id(id), url(url)
        {
        }
    };

    /**
     * @brief 热键框值改变事件参数类型
     */
    class HotKeyValueChangedEventArgs : public TypedRoutedEventArgs<HotKeyControl_ValueChanged>
    {
    public:
        /// @brief 按键
        VirtualKey key;

        /// @brief 辅助按键
        HotKeyModifier modifier;

        /// @brief 构造函数
        /// @param key 按键
        /// @param modifier 辅助按键
        HotKeyValueChangedEventArgs(VirtualKey key, HotKeyModifier modifier) : key(key), modifier(modifier)
        {
        }
    };

    /**
     * @brief 分割按钮的下拉箭头单击事件参数类型
     */
    class SplitButtonDropDownEventArgs : public TypedRoutedEventArgs<SplitButton_DropDown, CancelableEventArgs>
    {
    };
}

// Variant.h


namespace sw
{
    /**
     * @brief 动态对象引用标记类型
     * @note 当Variant内部以BoxedObject<ObjectRef>形式装箱时表示对DynamicObject的引用语义。
     *       Variant的Object()/IsType/DynamicCast/UnsafeCast会自动透视此标记，
     *       使引用对象的访问与值语义一致。一般通过Variant::MakeRef构造，无需直接使用。
     */
    struct ObjectRef final {
        /**
         * @brief 被引用的DynamicObject指针
         */
        DynamicObject *ptr;
    };

    /**
     * @brief 通用变体类型容器，类型擦除地持有任意类型对象
     * @note Variant自身是原生C++值类型，而非DynamicObject派生类。
     *       它可持有任意类型对象 —— 既包括DynamicObject派生类型，也包括原生C++类型。
     *       内部按持入类型与语义自动选择存储策略：
     *       - 值语义 + DynamicObject派生类型：直接以unique_ptr<DynamicObject>持有；
     *       - 值语义 + 其他原生C++类型：装箱为BoxedObject<T>持有；
     *       - 引用语义 + DynamicObject派生类型：装箱为BoxedObject<ObjectRef>持有；
     *       - 引用语义 + 其他原生C++类型：装箱为引用模式的BoxedObject<T>持有；
     *       这些差异对Object()/IsType/DynamicCast/UnsafeCast等查询接口透明。
     * @note 默认为值语义（拷贝/赋值会通过克隆函数指针深拷贝内部对象）；
     *       通过Variant::MakeRef或显式构造ObjectRef可获得引用语义。
     * @note 持入Variant的类型其析构函数不应抛出异常 —— C++11起析构函数默认即为
     *       noexcept，Variant的多个noexcept移动操作依赖此前提。
     */
    class Variant final
    {
    private:
        /**
         * @brief 内部动态对象指针
         */
        std::unique_ptr<DynamicObject> _obj;

        /**
         * @brief 克隆对象函数指针
         */
        DynamicObject *(*_cloner)(const DynamicObject &) = nullptr;

    public:
        /**
         * @brief 默认构造函数，创建一个空的Variant对象
         */
        Variant() = default;

        /**
         * @brief 从nullptr构造一个空的Variant对象
         */
        Variant(std::nullptr_t) noexcept
        {
        }

        /**
         * @brief 以引用语义构造Variant
         * @param ref 引用标记
         * @note 与Variant::MakeRef等价的低层入口，内部以装箱的ObjectRef表示。
         *       使用方需保证被引用对象在Variant存活期间始终有效。
         * @note 若ref.ptr为nullptr则构造为空Variant，行为与Variant(std::nullptr_t)一致。
         */
        Variant(ObjectRef ref)
        {
            if (ref.ptr != nullptr) {
                _obj.reset(new BoxedObject<ObjectRef>(ref));
                ResetCloner<ObjectRef>();
            }
        }

        /**
         * @brief 通用构造函数，接受任意类型的对象（值语义）
         * @warning 若T为基类引用且实际指向派生对象，会发生对象切片；
         *          如需保留对外部对象的引用语义，请改用Variant::MakeRef<T>。
         */
        template <
            typename T,
            typename std::enable_if<!std::is_same<Variant, typename std::decay<T>::type>::value, int>::type = 0>
        Variant(T &&obj)
        {
            Reset(std::forward<T>(obj));
        }

        /**
         * @brief 拷贝构造函数
         * @throws std::runtime_error 如果对象不可拷贝
         */
        Variant(const Variant &other)
        {
            if (other._obj != nullptr) {
                _obj.reset(other._cloner(*other._obj));
                _cloner = other._cloner;
            }
        }

        /**
         * @brief 移动构造函数
         */
        Variant(Variant &&other) noexcept
            : _obj(std::move(other._obj)), _cloner(other._cloner)
        {
            other._cloner = nullptr;
        }

        /**
         * @brief 拷贝赋值运算符
         * @throws std::runtime_error 如果对象不可拷贝
         */
        Variant &operator=(const Variant &other)
        {
            Reset(other);
            return *this;
        }

        /**
         * @brief 移动赋值运算符
         */
        Variant &operator=(Variant &&other) noexcept
        {
            Reset(std::move(other));
            return *this;
        }

        /**
         * @brief 布尔转换运算符，判断Variant对象是否为空
         * @return 若Variant对象不为空则返回true，否则返回false
         */
        explicit operator bool() const noexcept
        {
            return _obj != nullptr;
        }

        /**
         * @brief 判断Variant对象是否为空
         * @return 若Variant对象为空则返回true，否则返回false
         */
        bool IsNull() const noexcept
        {
            return _obj == nullptr;
        }

        /**
         * @brief 判断Variant对象是否包含值
         * @return 若Variant对象不为空则返回true，否则返回false
         */
        bool HasValue() const noexcept
        {
            return _obj != nullptr;
        }

        /**
         * @brief 判断两Variant是否引用同一对象（指针身份比较）
         * @return 若两Variant的有效对象指针相同（含都为空）则返回true，否则返回false
         * @note 这是引用相等而非值相等。比较的是Object()返回的指针，因此引用语义的
         *       Variant与持有同一原始对象的Variant会被视为相等。若需值相等比较，
         *       请通过Object()或DynamicCast<T>()获取内部对象后自行比较。
         */
        bool ReferenceEquals(const Variant &other) const noexcept
        {
            const DynamicObject *a = Object();
            const DynamicObject *b = other.Object();

            if (a == nullptr && b == nullptr) {
                return true;
            }
            if (a == nullptr || b == nullptr) {
                return false;
            }
            return a->ReferenceEquals(*b);
        }

        /**
         * @brief 重置Variant对象为空
         */
        void Reset() noexcept
        {
            _obj.reset();
            _cloner = nullptr;
        }

        /**
         * @brief 重置Variant对象为空
         */
        void Reset(std::nullptr_t) noexcept
        {
            Reset();
        }

        /**
         * @brief 重置Variant对象为指定的引用标记
         * @param ref 引用标记
         * @note 内部以装箱的ObjectRef表示，使Variant以引用语义持有DynamicObject。
         *       使用方需保证被引用对象在Variant存活期间始终有效。
         * @note 若ref.ptr为nullptr则置为空Variant，行为与Reset(std::nullptr_t)一致。
         */
        void Reset(ObjectRef ref)
        {
            if (ref.ptr == nullptr) {
                Reset();
            } else {
                _obj.reset(new BoxedObject<ObjectRef>(ref));
                ResetCloner<ObjectRef>();
            }
        }

        /**
         * @brief 重置Variant对象为另一个Variant对象的值
         * @throws std::runtime_error 如果对象不可拷贝
         */
        void Reset(const Variant &other)
        {
            if (this == &other) {
                return;
            }

            if (other._obj == nullptr) {
                Reset();
            } else {
                _obj.reset(other._cloner(*other._obj));
                _cloner = other._cloner;
            }
        }

        /**
         * @brief 重置Variant对象为另一个Variant对象的值
         */
        void Reset(Variant &&other) noexcept
        {
            if (this != &other) {
                _obj          = std::move(other._obj);
                _cloner       = other._cloner;
                other._cloner = nullptr;
            }
        }

        /**
         * @brief 重置Variant对象为指定类型的对象
         * @tparam T 对象类型
         * @param obj 对象
         */
        template <typename T>
        auto Reset(T &&obj)
            -> typename std::enable_if<
                !std::is_same<Variant, typename std::decay<T>::type>::value &&
                std::is_base_of<DynamicObject, typename std::decay<T>::type>::value>::type
        {
            using U = typename std::decay<T>::type;
            _obj.reset(new U(std::forward<T>(obj)));
            ResetCloner<U>();
        }

        /**
         * @brief 重置Variant对象为指定类型的对象
         * @tparam T 对象类型
         * @param obj 对象
         */
        template <typename T>
        auto Reset(T &&obj)
            -> typename std::enable_if<
                !std::is_same<Variant, typename std::decay<T>::type>::value &&
                !std::is_base_of<DynamicObject, typename std::decay<T>::type>::value>::type
        {
            using U = typename std::decay<T>::type;
            _obj.reset(new BoxedObject<U>(std::forward<T>(obj)));
            ResetCloner<U>();
        }

        /**
         * @brief 获取内部动态对象指针
         * @return 内部动态对象指针，若Variant为空则返回nullptr
         * @note 若Variant以引用语义持有DynamicObject（即装箱的ObjectRef），
         *       返回被引用对象的指针；否则返回内部对象指针。
         */
        DynamicObject *Object() noexcept
        {
            if (_obj == nullptr) {
                return nullptr;
            }

            ObjectRef *ref = nullptr;

            if (_obj->IsType<ObjectRef>(&ref)) {
                return ref->ptr;
            }
            return _obj.get();
        }

        /**
         * @brief 获取内部动态对象的常量指针
         * @return 内部动态对象的常量指针，若Variant为空则返回nullptr
         * @note 若Variant以引用语义持有DynamicObject（即装箱的ObjectRef），
         *       返回被引用对象的指针；否则返回内部对象指针。
         */
        const DynamicObject *Object() const noexcept
        {
            if (_obj == nullptr) {
                return nullptr;
            }

            const ObjectRef *ref = nullptr;

            if (_obj->IsType<ObjectRef>(&ref)) {
                return ref->ptr;
            }
            return _obj.get();
        }

        /**
         * @brief 获取内部对象的类型信息
         * @return 内部对象的类型索引；若Variant为空则返回typeid(void)
         * @note 语义与DynamicObject::GetType一致 —— 返回内部对象的最派生类型。
         *       对引用语义的Variant会自动透视到被引用对象。
         */
        std::type_index GetType() const
        {
            const DynamicObject *p = Object();
            return p == nullptr ? typeid(void) : p->GetType();
        }

        /**
         * @brief 判断当前Variant存储的对象是否为指定类型
         * @tparam T 目标类型
         * @param pout 如果不为nullptr，则将转换后的指针赋值给该参数
         * @return 如果Variant对象为指定类型则返回true，否则返回false
         * @note 对引用语义的Variant会自动透视到被引用对象，调用方无需关心存储形式。
         */
        template <typename T>
        bool IsType(T **pout = nullptr)
        {
            DynamicObject *p = Object();

            if (p == nullptr) {
                if (pout != nullptr) {
                    *pout = nullptr;
                }
                return false;
            }
            return p->IsType<T>(pout);
        }

        /**
         * @brief 判断当前Variant存储的对象是否为指定类型
         * @tparam T 目标类型
         * @param pout 如果不为nullptr，则将转换后的指针赋值给该参数
         * @return 如果Variant对象为指定类型则返回true，否则返回false
         * @note 对引用语义的Variant会自动透视到被引用对象，调用方无需关心存储形式。
         */
        template <typename T>
        bool IsType(const T **pout = nullptr) const
        {
            const DynamicObject *p = Object();

            if (p == nullptr) {
                if (pout != nullptr) {
                    *pout = nullptr;
                }
                return false;
            }
            return p->IsType<T>(pout);
        }

        /**
         * @brief 将Variant对象动态转换为指定类型的引用
         * @tparam T 目标类型
         * @return 指定类型的引用
         * @throws std::bad_cast 如果转换失败或Variant为空
         * @note 对引用语义的Variant会自动透视到被引用对象。
         */
        template <typename T>
        T &DynamicCast()
        {
            DynamicObject *p = Object();

            if (p == nullptr) {
                throw std::bad_cast();
            }
            return p->DynamicCast<T>();
        }

        /**
         * @brief 将Variant对象动态转换为指定类型的常量引用
         * @tparam T 目标类型
         * @return 指定类型的常量引用
         * @throws std::bad_cast 如果转换失败或Variant为空
         * @note 对引用语义的Variant会自动透视到被引用对象。
         */
        template <typename T>
        const T &DynamicCast() const
        {
            const DynamicObject *p = Object();

            if (p == nullptr) {
                throw std::bad_cast();
            }
            return p->DynamicCast<T>();
        }

        /**
         * @brief 将Variant对象转换为指定类型的引用
         * @tparam T 目标类型
         * @return 指定类型的引用
         * @throws std::bad_cast 如果Variant为空
         * @note 若目标类型与存储类型不匹配，则行为未定义
         * @note 对引用语义的Variant会自动透视到被引用对象。
         */
        template <typename T>
        T &UnsafeCast()
        {
            DynamicObject *p = Object();

            if (p == nullptr) {
                throw std::bad_cast();
            }
            return p->UnsafeCast<T>();
        }

        /**
         * @brief 将Variant对象转换为指定类型的常量引用
         * @tparam T 目标类型
         * @return 指定类型的常量引用
         * @throws std::bad_cast 如果Variant为空
         * @note 若目标类型与存储类型不匹配，则行为未定义
         * @note 对引用语义的Variant会自动透视到被引用对象。
         */
        template <typename T>
        const T &UnsafeCast() const
        {
            const DynamicObject *p = Object();

            if (p == nullptr) {
                throw std::bad_cast();
            }
            return p->UnsafeCast<T>();
        }

    private:
        /**
         * @brief 初始化克隆函数指针
         * @tparam T 对象类型
         */
        template <typename T>
        auto ResetCloner()
            -> typename std::enable_if<std::is_copy_constructible<T>::value && std::is_base_of<DynamicObject, T>::value>::type
        {
            _cloner = [](const DynamicObject &other) -> DynamicObject * {
                return new T(other.UnsafeCast<T>());
            };
        }

        /**
         * @brief 初始化克隆函数指针
         * @tparam T 对象类型
         */
        template <typename T>
        auto ResetCloner()
            -> typename std::enable_if<std::is_copy_constructible<T>::value && !std::is_base_of<DynamicObject, T>::value>::type
        {
            _cloner = [](const DynamicObject &other) -> DynamicObject * {
                return new BoxedObject<T>(other.UnsafeCast<BoxedObject<T>>());
            };
        }

        /**
         * @brief 初始化克隆函数指针
         * @tparam T 对象类型
         * @note 当T不可拷贝时，克隆函数指针在被调用时抛出std::runtime_error，
         *       而非在编译期失败 —— 以便不可拷贝类型仍可被存放和移动。
         */
        template <typename T>
        auto ResetCloner()
            -> typename std::enable_if<!std::is_copy_constructible<T>::value>::type
        {
            _cloner = [](const DynamicObject &other) -> DynamicObject * {
                throw std::runtime_error("Object is not copy constructible.");
            };
        }

    public:
        /**
         * @brief 就地构造一个值语义的Variant（DynamicObject派生类型重载）
         * @tparam T 要构造的对象类型，必须为DynamicObject的派生类
         * @tparam Args 构造参数类型包
         * @param args 转发给T构造函数的参数
         * @return 持有就地构造的T对象的Variant
         * @note 相较于Variant{T{args...}}，避免了一次T的移动构造，
         *       且支持不可移动/不可拷贝但可构造的类型。
         */
        template <typename T, typename... Args>
        static auto MakeVal(Args &&...args)
            -> typename std::enable_if<
                !std::is_same<T, Variant>::value &&
                    std::is_base_of<DynamicObject, T>::value,
                Variant>::type
        {
            Variant v;
            v._obj.reset(new T(std::forward<Args>(args)...));
            v.ResetCloner<T>();
            return v;
        }

        /**
         * @brief 就地构造一个值语义的Variant（非DynamicObject类型重载）
         * @tparam T 要构造的对象类型
         * @tparam Args 构造参数类型包
         * @param args 转发给T构造函数的参数
         * @return 持有就地装箱构造的T对象的Variant
         * @note 相较于Variant{T{args...}}，避免了一次T的移动构造，
         *       且支持不可移动/不可拷贝但可构造的类型。
         */
        template <typename T, typename... Args>
        static auto MakeVal(Args &&...args)
            -> typename std::enable_if<
                !std::is_same<T, Variant>::value &&
                    !std::is_base_of<DynamicObject, T>::value,
                Variant>::type
        {
            Variant v;
            v._obj.reset(new BoxedObject<T>(std::forward<Args>(args)...));
            v.ResetCloner<T>();
            return v;
        }

        /**
         * @brief 创建一个对另一Variant内部对象的引用Variant（Variant转发重载）
         * @param v 源Variant
         * @return 一个引用语义的Variant，引用v当前承载的对象；若v为空则返回空Variant
         * @note 避免对Variant调用泛型MakeRef时产生BoxedObject<Variant>嵌套包装。
         *       结果直接引用v.Object()返回的指针，因此当v本身已是引用语义时，
         *       结果与v共享同一被引用对象，不会形成多层引用链。
         */
        static Variant MakeRef(Variant &v)
        {
            DynamicObject *p = v.Object();
            return p == nullptr ? Variant{} : MakeRef(*p);
        }

        /**
         * @brief 创建一个对外部对象的引用Variant（DynamicObject派生类型重载）
         * @tparam T 被引用的对象类型，必须为DynamicObject的派生类
         * @param obj 被引用的对象
         * @return 一个引用语义的Variant，与obj共享生命周期
         * @note 内部以装箱的ObjectRef表示，Variant的类型查询接口对此透明。
         *       使用方需保证obj在Variant存活期间始终有效。
         */
        template <typename T>
        static auto MakeRef(T &obj)
            -> typename std::enable_if<
                !std::is_same<T, Variant>::value &&
                    std::is_base_of<DynamicObject, T>::value,
                Variant>::type
        {
            Variant v;
            v._obj.reset(new BoxedObject<ObjectRef>(ObjectRef{&obj}));
            v.ResetCloner<ObjectRef>();
            return v;
        }

        /**
         * @brief 创建一个对外部对象的引用Variant（非DynamicObject类型重载）
         * @tparam T 被引用的对象类型
         * @param obj 被引用的对象
         * @return 一个引用语义的Variant，与obj共享生命周期
         * @note 内部以引用模式的BoxedObject<T>表示。
         *       使用方需保证obj在Variant存活期间始终有效。
         */
        template <typename T>
        static auto MakeRef(T &obj)
            -> typename std::enable_if<
                !std::is_same<T, Variant>::value &&
                    !std::is_base_of<DynamicObject, T>::value,
                Variant>::type
        {
            Variant v;
            v._obj.reset(new BoxedObject<T>(BoxedObject<T>::MakeRef(obj)));
            v.ResetCloner<T>();
            return v;
        }
    };
}

// BindingCastHelper.h


namespace sw
{
    /**
     * @brief Binding默认转换辅助模板
     * @tparam TSource 源属性值类型
     * @tparam TTarget 目标属性值类型
     * @note 当Binding未显式指定IValueConverter时，会使用该模板判断是否存在默认转换：
     *       - 若BindingCastHelper<TSource, TTarget>::value为true，则必须提供Convert和ConvertBack函数；
     *       - Convert用于将源属性值转换为目标属性值；
     *       - ConvertBack用于将目标属性值转换回源属性值；
     *       - 若value为false，则对应Binding必须显式提供IValueConverter。
     * @note 可在namespace sw中为自定义类型特化该模板，并继承std::true_type、实现Convert和ConvertBack，
     *       从而扩展Binding的默认转换能力。
     */
    template <typename TSource, typename TTarget, typename = void>
    struct BindingCastHelper : std::false_type {
    };

    /**
     * @brief 相同类型之间的默认转换
     * @tparam TSource 源属性值类型
     * @tparam TTarget 目标属性值类型
     */
    template <typename TSource, typename TTarget>
    struct BindingCastHelper<
        TSource, TTarget,
        typename std::enable_if<
            std::is_same<TSource, TTarget>::value>::type>
        : std::true_type //
    {
        /**
         * @brief 将源属性值转换为目标属性值
         * @param value 源属性值
         * @return 目标属性值
         */
        static TTarget Convert(const TSource &value)
        {
            return value;
        }

        /**
         * @brief 将源属性值移动转换为目标属性值
         * @param value 源属性值
         * @return 目标属性值
         */
        static TTarget Convert(TSource &&value)
        {
            return std::move(value);
        }

        /**
         * @brief 将目标属性值转换回源属性值
         * @param value 目标属性值
         * @return 源属性值
         */
        static TSource ConvertBack(const TTarget &value)
        {
            return value;
        }

        /**
         * @brief 将目标属性值移动转换回源属性值
         * @param value 目标属性值
         * @return 源属性值
         */
        static TSource ConvertBack(TTarget &&value)
        {
            return std::move(value);
        }
    };

    /**
     * @brief 可双向static_cast的类型之间的默认转换
     * @tparam TSource 源属性值类型
     * @tparam TTarget 目标属性值类型
     */
    template <typename TSource, typename TTarget>
    struct BindingCastHelper<
        TSource, TTarget,
        typename std::enable_if<
            !std::is_same<TSource, TTarget>::value &&
            _IsStaticCastable<TSource, TTarget>::value &&
            _IsStaticCastable<TTarget, TSource>::value>::type>
        : std::true_type //
    {
        /**
         * @brief 通过static_cast将源属性值转换为目标属性值
         * @param value 源属性值
         * @return 目标属性值
         */
        static TTarget Convert(const TSource &value)
        {
            return static_cast<TTarget>(value);
        }

        /**
         * @brief 通过static_cast将源属性值移动转换为目标属性值
         * @param value 源属性值
         * @return 目标属性值
         */
        static TTarget Convert(TSource &&value)
        {
            return static_cast<TTarget>(std::move(value));
        }

        /**
         * @brief 通过static_cast将目标属性值转换回源属性值
         * @param value 目标属性值
         * @return 源属性值
         */
        static TSource ConvertBack(const TTarget &value)
        {
            return static_cast<TSource>(value);
        }

        /**
         * @brief 通过static_cast将目标属性值移动转换回源属性值
         * @param value 目标属性值
         * @return 源属性值
         */
        static TSource ConvertBack(TTarget &&value)
        {
            return static_cast<TSource>(std::move(value));
        }
    };

    /**
     * @brief 任意非Variant源类型到Variant目标类型的默认转换
     * @tparam TSource 源属性值类型
     */
    template <typename TSource>
    struct BindingCastHelper<
        TSource, Variant,
        typename std::enable_if<!std::is_same<TSource, Variant>::value>::type> : std::true_type //
    {
        /**
         * @brief 将源属性值装箱为Variant
         * @param value 源属性值
         * @return 保存源属性值副本的Variant
         */
        static Variant Convert(const TSource &value)
        {
            return Variant{value};
        }

        /**
         * @brief 将源属性值移动装箱为Variant
         * @param value 源属性值
         * @return 保存源属性值的Variant
         */
        static Variant Convert(TSource &&value)
        {
            return Variant{std::move(value)};
        }

        /**
         * @brief 从Variant中动态转换回源属性值类型
         * @param value Variant属性值
         * @return 源属性值
         * @throws std::bad_cast 当Variant为空或内部对象无法转换为TSource时抛出
         */
        static TSource ConvertBack(const Variant &value)
        {
            return value.DynamicCast<TSource>();
        }
    };

    /**
     * @brief Variant源类型到任意非Variant目标类型的默认转换
     * @tparam TTarget 目标属性值类型
     */
    template <typename TTarget>
    struct BindingCastHelper<
        Variant, TTarget,
        typename std::enable_if<!std::is_same<TTarget, Variant>::value>::type> : std::true_type //
    {
        /**
         * @brief 从Variant中动态转换为目标属性值类型
         * @param value Variant属性值
         * @return 目标属性值
         * @throws std::bad_cast 当Variant为空或内部对象无法转换为TTarget时抛出
         */
        static TTarget Convert(const Variant &value)
        {
            return value.DynamicCast<TTarget>();
        }

        /**
         * @brief 将目标属性值转换回Variant
         * @param value 目标属性值
         * @return 保存目标属性值副本的Variant
         */
        static Variant ConvertBack(const TTarget &value)
        {
            return Variant{value};
        }

        /**
         * @brief 将目标属性值移动转换回Variant
         * @param value 目标属性值
         * @return 保存目标属性值的Variant
         */
        static Variant ConvertBack(TTarget &&value)
        {
            return Variant{std::move(value)};
        }
    };
}

// IList.h


namespace sw
{
    /**
     * @brief 非模板列表接口，提供基于Variant的元素访问
     */
    class IList
    {
    public:
        /**
         * @brief 虚析构函数
         */
        virtual ~IList() = default;

    public:
        /**
         * @brief 获取列表元素的类型信息
         * @return 元素类型的std::type_index
         */
        virtual std::type_index GetElementType() const noexcept = 0;

        /**
         * @brief 返回列表中的元素数量
         * @return 元素数量
         */
        virtual int Count() const noexcept = 0;

        /**
         * @brief 获取指定索引处元素的Variant引用
         * @param index 元素索引
         * @return 元素的Variant引用
         * @throws std::out_of_range 索引超出范围
         */
        virtual Variant GetVariantAt(int index) = 0;

        /**
         * @brief 获取指定索引处元素的Variant副本
         * @param index 元素索引
         * @return 元素的Variant副本
         * @throws std::out_of_range 索引超出范围
         * @throws std::logic_error 元素类型不可拷贝构造时
         */
        virtual Variant GetVariantAt(int index) const = 0;

        /**
         * @brief 设置指定索引处的元素值
         * @param index 元素索引
         * @param value 要设置的值
         * @throws std::out_of_range 索引超出范围
         */
        virtual void SetVariantAt(int index, const Variant &value) = 0;

        /**
         * @brief 设置指定索引处的元素值（移动语义）
         * @param index 元素索引
         * @param value 要设置的值
         * @throws std::out_of_range 索引超出范围
         */
        virtual void MoveVariantAt(int index, Variant &value) = 0;
    };

    /**
     * @brief 类型安全的列表接口，继承IList并提供类型化的元素访问
     * @tparam T 列表元素类型
     */
    template <typename T>
    class IListT : public IList
    {
    public:
        /**
         * @brief 虚析构函数
         */
        virtual ~IListT() = default;

        /**
         * @brief 获取列表元素的类型信息
         * @return 元素类型的std::type_index
         */
        virtual std::type_index GetElementType() const noexcept override final
        {
            return typeid(T);
        }

        /**
         * @brief 获取指定索引处元素的Variant引用
         * @param index 元素索引
         * @return 元素的Variant引用
         * @throws std::out_of_range 索引超出范围
         */
        virtual Variant GetVariantAt(int index) override final
        {
            return Variant::MakeRef(GetAt(index));
        }

        /**
         * @brief 获取指定索引处元素的Variant副本
         * @param index 元素索引
         * @return 元素的Variant副本
         * @throws std::out_of_range 索引超出范围
         * @throws std::logic_error T不可拷贝构造时
         */
        virtual Variant GetVariantAt(int index) const override final
        {
            return GetVariantValueAtImpl(index);
        }

        /**
         * @brief 设置指定索引处的元素值
         * @param index 元素索引
         * @param value 要设置的值
         * @throws std::out_of_range 索引超出范围
         * @throws std::logic_error T不可拷贝赋值时
         * @throws std::invalid_argument value为null（T不为Variant时）
         * @throws std::bad_cast Variant类型与T不匹配（T不为Variant时）
         */
        virtual void SetVariantAt(int index, const Variant &value) override final
        {
            SetVariantAtImpl(index, value);
        }

        /**
         * @brief 设置指定索引处的元素值（移动语义）
         * @param index 元素索引
         * @param value 要设置的值
         * @throws std::out_of_range 索引超出范围
         * @throws std::logic_error T不可移动赋值时
         * @throws std::invalid_argument value为null（T不为Variant时）
         * @throws std::bad_cast Variant类型与T不匹配（T不为Variant时）
         */
        virtual void MoveVariantAt(int index, Variant &value) override final
        {
            MoveVariantAtImpl(index, value);
        }

    private:
        /**
         * @brief 获取Variant副本的实现（T可拷贝构造时）
         * @param index 元素索引
         * @return 元素的Variant副本
         * @throws std::out_of_range 索引超出范围
         */
        template <typename U = T>
        auto GetVariantValueAtImpl(int index) const
            -> typename std::enable_if<std::is_copy_constructible<U>::value, Variant>::type
        {
            return Variant{GetAt(index)};
        }

        /**
         * @brief 获取Variant副本的实现（T不可拷贝构造时）
         * @throws std::logic_error T不可拷贝构造
         */
        template <typename U = T>
        auto GetVariantValueAtImpl(int) const
            -> typename std::enable_if<!std::is_copy_constructible<U>::value, Variant>::type
        {
            throw std::logic_error(
                "Type T must be copy constructible to get Variant by value in IListT.");
        }

        /**
         * @brief 设置Variant值的实现（T为Variant时，直接赋值）
         * @param index 元素索引
         * @param value 要设置的Variant值
         * @throws std::out_of_range 索引超出范围
         */
        template <typename U = T>
        auto SetVariantAtImpl(int index, const Variant &value)
            -> typename std::enable_if<std::is_same<U, Variant>::value>::type
        {
            SetAt(index, value);
        }

        /**
         * @brief 设置Variant值的实现（T不为Variant时，提取类型并赋值）
         * @param index 元素索引
         * @param value 要设置的Variant值
         * @throws std::out_of_range 索引超出范围
         * @throws std::invalid_argument value为null
         * @throws std::bad_cast Variant类型与T不匹配
         */
        template <typename U = T>
        auto SetVariantAtImpl(int index, const Variant &value)
            -> typename std::enable_if<!std::is_same<U, Variant>::value>::type
        {
            if (value.IsNull()) {
                throw std::invalid_argument(
                    "Cannot set a null Variant to a non-Variant IListT.");
            }
            SetAt(index, value.DynamicCast<T>());
        }

        /**
         * @brief 设置指定索引处的元素值（移动语义）
         * @param index 元素索引
         * @param value 要设置的值
         * @throws std::out_of_range 索引超出范围
         */
        template <typename U = T>
        auto MoveVariantAtImpl(int index, Variant &value)
            -> typename std::enable_if<std::is_same<U, Variant>::value>::type
        {
            SetAt(index, std::move(value));
        }

        /**
         * @brief 设置指定索引处的元素值（移动语义）
         * @param index 元素索引
         * @param value 要设置的值
         * @throws std::out_of_range 索引超出范围
         * @throws std::invalid_argument value为null
         * @throws std::bad_cast Variant类型与T不匹配
         */
        template <typename U = T>
        auto MoveVariantAtImpl(int index, Variant &value)
            -> typename std::enable_if<!std::is_same<U, Variant>::value>::type
        {
            if (value.IsNull()) {
                throw std::invalid_argument(
                    "Cannot set a null Variant to a non-Variant IListT.");
            }
            SetAt(index, std::move(value.DynamicCast<T>()));
        }

    public:
        /**
         * @brief 返回列表中的元素数量
         * @return 元素数量
         */
        virtual int Count() const noexcept = 0;

        /**
         * @brief 获取指定索引处的元素引用
         * @param index 元素索引
         * @return 元素引用
         * @throws std::out_of_range 索引超出范围
         */
        virtual T &GetAt(int index) = 0;

        /**
         * @brief 获取指定索引处的const元素引用
         * @param index 元素索引
         * @return const元素引用
         * @throws std::out_of_range 索引超出范围
         */
        virtual const T &GetAt(int index) const = 0;

        /**
         * @brief 设置指定索引处的元素值
         * @param index 元素索引
         * @param value 要设置的值
         * @throws std::out_of_range 索引超出范围
         * @throws std::logic_error T不可拷贝赋值时
         */
        virtual void SetAt(int index, const T &value) = 0;

        /**
         * @brief 设置指定索引处的元素值（移动语义）
         * @param index 元素索引
         * @param value 要设置的值
         * @throws std::out_of_range 索引超出范围
         * @throws std::logic_error T不可移动赋值时
         */
        virtual void SetAt(int index, T &&value) = 0;
    };
}

// LayoutHost.h


namespace sw
{
    /**
     * @brief 用于托管元素的布局方式的对象类型，是所有布局方式类型的基类
     */
    class LayoutHost
    {
    private:
        /**
         * @brief 关联的对象
         */
        ILayout *_associatedObj = nullptr;

    public:
        /**
         * @brief 默认虚析构函数
         */
        virtual ~LayoutHost() = default;

        /**
         * @brief 设置关联的对象，每个LayoutHost只能关联一个对象
         * @param obj 要关联的对象
         */
        void Associate(ILayout *obj);

        /**
         * @brief 判断当前LayoutHost是否关联了对象
         * @param obj 若传入值为nullptr，则判断是否有任何对象关联，否则判断是否关联了指定对象
         */
        bool IsAssociated(ILayout *obj = nullptr) const;

        /**
         * @brief 获取关联对象子控件的数量
         */
        int GetChildLayoutCount() const;

        /**
         * @brief 获取关联对象对应索引处的子控件
         * @throw std::out_of_range 如果索引超出范围
         */
        ILayout &GetChildLayoutAt(int index) const;

    public:
        /**
         * @brief 测量元素所需尺寸，无需考虑边框和边距
         * @param availableSize 可用的尺寸
         * @return 返回元素需要占用的尺寸
         */
        virtual Size MeasureOverride(const Size &availableSize) = 0;

        /**
         * @brief 安排子元素的位置，可重写该函数以实现自定义布局
         * @param finalSize 可用于排列子元素的最终尺寸
         */
        virtual void ArrangeOverride(const Size &finalSize) = 0;
    };
}

// Macros.h


/*================================================================================*/

/**
 * @file Macros.h
 * @brief 属性定义辅助宏，用于在类中以单行声明的方式生成 sw::Property 系列成员
 *
 * @par SW_DEFINE_*_PROPERTY 系列（基于字段）
 * 对类内的非静态数据成员进行属性封装。若类中定义了 Get_{field}/Set_{field} 函数，
 * 生成的属性会优先调用这些函数；否则直接访问字段本身。
 *  - SW_DEFINE_PROPERTY(name, field)
 *  - SW_DEFINE_READONLY_PROPERTY(name, field)
 *  - SW_DEFINE_WRITEONLY_PROPERTY(name, field)
 *  - SW_DEFINE_NOTIFY_PROPERTY(name, field)
 *
 * @par SW_DEFINE_EXPR_*_PROPERTY 系列（基于表达式）
 * 对形如 this->{expr} 的表达式进行属性封装。当表达式结果是另一个属性时，
 * Get/Set 会转发到该属性；否则直接读写表达式。该系列不支持自定义 Getter/Setter。
 *  - SW_DEFINE_EXPR_PROPERTY(name, expr)
 *  - SW_DEFINE_EXPR_READONLY_PROPERTY(name, expr)
 *  - SW_DEFINE_EXPR_WRITEONLY_PROPERTY(name, expr)
 *  - SW_DEFINE_EXPR_NOTIFY_PROPERTY(name, expr)
 */

/*================================================================================*/

/**
 * @brief 在类内定义读取字段值的静态分发函数 _Get_##field（及配套 trait _HasUserGetter_##field）
 * @param field 类中已声明的非静态数据成员名
 * @note 若类中存在自定义 Get_##field()，_Get_##field 转发调用之；否则直接返回 self.field 的引用
 */
#define _SW_DEFINE_STATIC_GETTER(field)                                                                  \
    template <typename T, typename = void>                                                               \
    struct _HasUserGetter_##field : std::false_type {                                                    \
    };                                                                                                   \
    template <typename T>                                                                                \
    struct _HasUserGetter_##field<T, decltype(void(&T::Get_##field))> : std::true_type {                 \
    };                                                                                                   \
    template <typename T>                                                                                \
    static auto _Get_##field(T &self)                                                                    \
        -> typename std::enable_if<_HasUserGetter_##field<T>::value, decltype(self.Get_##field())>::type \
    {                                                                                                    \
        return self.Get_##field();                                                                       \
    }                                                                                                    \
    template <typename T>                                                                                \
    static auto _Get_##field(T &self)                                                                    \
        -> typename std::enable_if<!_HasUserGetter_##field<T>::value, decltype(T::field) &>::type        \
    {                                                                                                    \
        return self.field;                                                                               \
    }

/**
 * @brief 在类内定义写入字段值的静态分发函数 _Set_##field（及配套 trait _HasUserSetter_##field）
 * @param field 类中已声明的非静态数据成员名
 * @note 若类中存在自定义 Set_##field()，_Set_##field 转发调用之；否则直接对 self.field 赋值
 */
#define _SW_DEFINE_STATIC_SETTER(field)                                                  \
    template <typename T, typename = void>                                               \
    struct _HasUserSetter_##field : std::false_type {                                    \
    };                                                                                   \
    template <typename T>                                                                \
    struct _HasUserSetter_##field<T, decltype(void(&T::Set_##field))> : std::true_type { \
    };                                                                                   \
    template <typename T, typename U>                                                    \
    static auto _Set_##field(T &self, U &&value)                                         \
        -> typename std::enable_if<_HasUserSetter_##field<T>::value>::type               \
    {                                                                                    \
        self.Set_##field(std::forward<U>(value));                                        \
    }                                                                                    \
    template <typename T, typename U>                                                    \
    static auto _Set_##field(T &self, U &&value)                                         \
        -> typename std::enable_if<!_HasUserSetter_##field<T>::value>::type              \
    {                                                                                    \
        self.field = std::forward<U>(value);                                             \
    }

/*================================================================================*/

/**
 * @brief 定义基于字段的可读写属性
 * @param name 生成的 sw::Property 成员名
 * @param field 类中已声明的非静态数据成员名
 * @note 类中存在自定义 Get_##field/Set_##field 时优先调用，否则直接访问字段
 */
#define SW_DEFINE_PROPERTY(name, field)           \
    _SW_DEFINE_STATIC_GETTER(field);              \
    _SW_DEFINE_STATIC_SETTER(field);              \
    sw::Property<decltype(field)> name            \
    {                                             \
        sw::Property<decltype(field)>::Init(this) \
            .Getter([](auto self) {               \
                return _Get_##field(*self);       \
            })                                    \
            .Setter([](auto self, auto value) {   \
                _Set_##field(*self, value);       \
            })                                    \
    }

/**
 * @brief 定义基于字段的只读属性
 * @param name 生成的 sw::ReadOnlyProperty 成员名
 * @param field 类中已声明的非静态数据成员名
 * @note 类中存在自定义 Get_##field 时优先调用，否则直接访问字段
 */
#define SW_DEFINE_READONLY_PROPERTY(name, field)  \
    _SW_DEFINE_STATIC_GETTER(field);              \
    sw::ReadOnlyProperty<decltype(field)> name    \
    {                                             \
        sw::Property<decltype(field)>::Init(this) \
            .Getter([](auto self) {               \
                return _Get_##field(*self);       \
            })                                    \
    }

/**
 * @brief 定义基于字段的只写属性
 * @param name 生成的 sw::WriteOnlyProperty 成员名
 * @param field 类中已声明的非静态数据成员名
 * @note 类中存在自定义 Set_##field 时优先调用，否则直接访问字段
 */
#define SW_DEFINE_WRITEONLY_PROPERTY(name, field) \
    _SW_DEFINE_STATIC_SETTER(field);              \
    sw::WriteOnlyProperty<decltype(field)> name   \
    {                                             \
        sw::Property<decltype(field)>::Init(this) \
            .Setter([](auto self, auto value) {   \
                _Set_##field(*self, value);       \
            })                                    \
    }

/**
 * @brief 定义基于字段的通知属性，赋值时通过 PropertyChanged 派发变更通知
 * @param name 生成的 sw::Property 成员名
 * @param field 类中已声明的非静态数据成员名
 * @note 应在派生自 sw::INotifyPropertyChanged 的类中使用。
 *       类中存在自定义 Get_##field/Set_##field 时优先调用，否则直接访问字段。
 *       未自定义 Setter 时：若字段类型支持 == 比较则先比较，仅在值发生变更时触发 PropertyChanged；
 *       不支持 == 比较时每次赋值都触发。自定义 Setter 时由用户负责通知（宏不主动触发）。
 */
#define SW_DEFINE_NOTIFY_PROPERTY(name, field)                                                    \
    _SW_DEFINE_STATIC_GETTER(field);                                                              \
    template <typename T, typename = void>                                                        \
    struct _HasUserSetter_##field : std::false_type {                                             \
    };                                                                                            \
    template <typename T>                                                                         \
    struct _HasUserSetter_##field<T, decltype(void(&T::Set_##field))> : std::true_type {          \
    };                                                                                            \
    template <typename T, typename U>                                                             \
    static auto _Set_##field(T &self, U &&value)                                                  \
        -> typename std::enable_if<_HasUserSetter_##field<T>::value>::type                        \
    {                                                                                             \
        self.Set_##field(std::forward<U>(value));                                                 \
    }                                                                                             \
    template <typename T, typename U>                                                             \
    static auto _Set_##field(T &self, U &&value)                                                  \
        -> typename std::enable_if<                                                               \
            !_HasUserSetter_##field<T>::value &&                                                  \
            sw::_EqOperationHelper<decltype(_Get_##field(std::declval<T &>())), U>::value>::type  \
    {                                                                                             \
        if (!(_Get_##field(self) == value)) {                                                     \
            self.field = std::forward<U>(value);                                                  \
            if (auto &_d = self.GetPropertyChangedEventDelegate()) {                              \
                sw::PropertyChangedEventArgs _a{};                                                \
                _a.propertyId = sw::Reflection::GetFieldId(&T::name);                             \
                _d(self, _a);                                                                     \
            }                                                                                     \
        }                                                                                         \
    }                                                                                             \
    template <typename T, typename U>                                                             \
    static auto _Set_##field(T &self, U &&value)                                                  \
        -> typename std::enable_if<                                                               \
            !_HasUserSetter_##field<T>::value &&                                                  \
            !sw::_EqOperationHelper<decltype(_Get_##field(std::declval<T &>())), U>::value>::type \
    {                                                                                             \
        self.field = std::forward<U>(value);                                                      \
        if (auto &_d = self.GetPropertyChangedEventDelegate()) {                                  \
            sw::PropertyChangedEventArgs _a{};                                                    \
            _a.propertyId = sw::Reflection::GetFieldId(&T::name);                                 \
            _d(self, _a);                                                                         \
        }                                                                                         \
    }                                                                                             \
    sw::Property<decltype(field)> name                                                            \
    {                                                                                             \
        sw::Property<decltype(field)>::Init(this)                                                 \
            .Getter([](auto self) {                                                               \
                return _Get_##field(*self);                                                       \
            })                                                                                    \
            .Setter([](auto self, auto value) {                                                   \
                _Set_##field(*self, value);                                                       \
            })                                                                                    \
    }

/*================================================================================*/

/**
 * @brief 定义计算表达式属性值类型的辅助模板 _ExprPropertyValueTypeHelper_##propname
 * @param propname 属性名
 * @note 表达式类型为属性时 type 取属性的 TValue；否则取 std::decay 后的表达式类型
 */
#define _SW_DEFINE_EXPR_PROPERTY_VALUETYPE_HELPER(propname)                    \
    template <typename TExpr, typename = void>                                 \
    struct _ExprPropertyValueTypeHelper_##propname {                           \
        using type = typename std::decay<TExpr>::type;                         \
    };                                                                         \
    template <typename TExpr>                                                  \
    struct _ExprPropertyValueTypeHelper_##propname<                            \
        TExpr, typename std::enable_if<sw::_IsProperty<TExpr>::value>::type> { \
        using type = typename TExpr::TValue;                                   \
    }

/**
 * @brief 取出表达式对应的属性值类型（属性时为其 TValue，否则为 decay 后的表达式类型）
 * @param propname 属性名
 * @param expr 形如 self->{expr} 的表达式片段
 */
#define _SW_EXPR_PROPERTY_VALUETYPE(propname, expr) \
    typename _ExprPropertyValueTypeHelper_##propname<decltype(expr)>::type

/**
 * @brief 在类内定义读取表达式值的静态分发函数 _Get_##propname
 * @param propname 属性名
 * @param expr 形如 self->{expr} 的表达式片段
 * @note 表达式结果为属性时调用其 Get()，否则直接读取表达式
 */
#define _SW_DEFINE_EXPR_STATIC_GETTER(propname, expr)                                   \
    template <typename T, typename U = decltype(expr)>                                  \
    static auto _Get_##propname(T &self)                                                \
        -> typename std::enable_if<sw::_IsProperty<U>::value, typename U::TValue>::type \
    {                                                                                   \
        return (self.expr).Get();                                                       \
    }                                                                                   \
    template <typename T, typename U = decltype(expr)>                                  \
    static auto _Get_##propname(T &self)                                                \
        -> typename std::enable_if<!sw::_IsProperty<U>::value, U>::type                 \
    {                                                                                   \
        return (self.expr);                                                             \
    }

/**
 * @brief 在类内定义写入表达式值的静态分发函数 _Set_##propname
 * @param propname 属性名
 * @param expr 形如 self->{expr} 的表达式片段
 * @note 表达式结果为属性时调用其 Set()，否则直接对表达式赋值
 */
#define _SW_DEFINE_EXPR_STATIC_SETTER(propname, expr)                \
    template <typename T, typename U, typename V = decltype(expr)>   \
    static auto _Set_##propname(T &self, U &&value)                  \
        -> typename std::enable_if<sw::_IsProperty<V>::value>::type  \
    {                                                                \
        (self.expr).Set(std::forward<U>(value));                     \
    }                                                                \
    template <typename T, typename U, typename V = decltype(expr)>   \
    static auto _Set_##propname(T &self, U &&value)                  \
        -> typename std::enable_if<!sw::_IsProperty<V>::value>::type \
    {                                                                \
        (self.expr) = std::forward<U>(value);                        \
    }

/*================================================================================*/

/**
 * @brief 定义基于表达式 self->{expr} 的可读写属性，不支持自定义 Getter/Setter
 * @param name 生成的 sw::Property 成员名
 * @param expr 形如 self->{expr} 的表达式片段
 * @note 表达式结果为属性时转发到其 Get/Set，否则直接访问表达式
 */
#define SW_DEFINE_EXPR_PROPERTY(name, expr)                               \
    _SW_DEFINE_EXPR_PROPERTY_VALUETYPE_HELPER(name);                      \
    _SW_DEFINE_EXPR_STATIC_GETTER(name, expr);                            \
    _SW_DEFINE_EXPR_STATIC_SETTER(name, expr);                            \
    sw::Property<_SW_EXPR_PROPERTY_VALUETYPE(name, expr)> name            \
    {                                                                     \
        sw::Property<_SW_EXPR_PROPERTY_VALUETYPE(name, expr)>::Init(this) \
            .Getter([](auto self) {                                       \
                return _Get_##name(*self);                                \
            })                                                            \
            .Setter([](auto self, auto value) {                           \
                _Set_##name(*self, value);                                \
            })                                                            \
    }

/**
 * @brief 定义基于表达式 self->{expr} 的只读属性，不支持自定义 Getter
 * @param name 生成的 sw::ReadOnlyProperty 成员名
 * @param expr 形如 self->{expr} 的表达式片段
 * @note 表达式结果为属性时调用其 Get()，否则直接读取表达式
 */
#define SW_DEFINE_EXPR_READONLY_PROPERTY(name, expr)                      \
    _SW_DEFINE_EXPR_PROPERTY_VALUETYPE_HELPER(name);                      \
    _SW_DEFINE_EXPR_STATIC_GETTER(name, expr);                            \
    sw::ReadOnlyProperty<_SW_EXPR_PROPERTY_VALUETYPE(name, expr)> name    \
    {                                                                     \
        sw::Property<_SW_EXPR_PROPERTY_VALUETYPE(name, expr)>::Init(this) \
            .Getter([](auto self) {                                       \
                return _Get_##name(*self);                                \
            })                                                            \
    }

/**
 * @brief 定义基于表达式 self->{expr} 的只写属性，不支持自定义 Setter
 * @param name 生成的 sw::WriteOnlyProperty 成员名
 * @param expr 形如 self->{expr} 的表达式片段
 * @note 表达式结果为属性时调用其 Set()，否则直接对表达式赋值
 */
#define SW_DEFINE_EXPR_WRITEONLY_PROPERTY(name, expr)                     \
    _SW_DEFINE_EXPR_PROPERTY_VALUETYPE_HELPER(name);                      \
    _SW_DEFINE_EXPR_STATIC_SETTER(name, expr);                            \
    sw::WriteOnlyProperty<_SW_EXPR_PROPERTY_VALUETYPE(name, expr)> name   \
    {                                                                     \
        sw::Property<_SW_EXPR_PROPERTY_VALUETYPE(name, expr)>::Init(this) \
            .Setter([](auto self, auto value) {                           \
                _Set_##name(*self, value);                                \
            })                                                            \
    }

/**
 * @brief 定义基于表达式 self->{expr} 的通知属性，赋值时通过 PropertyChanged 派发变更通知
 * @param name 生成的 sw::Property 成员名
 * @param expr 形如 self->{expr} 的表达式片段
 * @note 应在派生自 sw::INotifyPropertyChanged 的类中使用。
 *       表达式结果为属性时转发到其 Get/Set，否则直接访问表达式；
 *       Setter 在类型支持 == 比较时先比较，仅在值发生变更时触发 PropertyChanged；
 *       不支持 == 比较时每次赋值都触发。
 */
#define SW_DEFINE_EXPR_NOTIFY_PROPERTY(name, expr)                        \
    _SW_DEFINE_EXPR_PROPERTY_VALUETYPE_HELPER(name);                      \
    _SW_DEFINE_EXPR_STATIC_GETTER(name, expr);                            \
    template <typename T, typename U, typename V = decltype(expr)>        \
    static auto _Set_##name(T &self, U &&value)                           \
        -> typename std::enable_if<                                       \
            sw::_IsProperty<V>::value &&                                  \
            sw::_EqOperationHelper<typename V::TValue, U>::value>::type   \
    {                                                                     \
        if (!((self.expr).Get() == value)) {                              \
            (self.expr).Set(std::forward<U>(value));                      \
            if (auto &_d = self.GetPropertyChangedEventDelegate()) {      \
                sw::PropertyChangedEventArgs _a{};                        \
                _a.propertyId = sw::Reflection::GetFieldId(&T::name);     \
                _d(self, _a);                                             \
            }                                                             \
        }                                                                 \
    }                                                                     \
    template <typename T, typename U, typename V = decltype(expr)>        \
    static auto _Set_##name(T &self, U &&value)                           \
        -> typename std::enable_if<                                       \
            sw::_IsProperty<V>::value &&                                  \
            !sw::_EqOperationHelper<typename V::TValue, U>::value>::type  \
    {                                                                     \
        (self.expr).Set(std::forward<U>(value));                          \
        if (auto &_d = self.GetPropertyChangedEventDelegate()) {          \
            sw::PropertyChangedEventArgs _a{};                            \
            _a.propertyId = sw::Reflection::GetFieldId(&T::name);         \
            _d(self, _a);                                                 \
        }                                                                 \
    }                                                                     \
    template <typename T, typename U, typename V = decltype(expr)>        \
    static auto _Set_##name(T &self, U &&value)                           \
        -> typename std::enable_if<                                       \
            !sw::_IsProperty<V>::value &&                                 \
            sw::_EqOperationHelper<V, U>::value>::type                    \
    {                                                                     \
        if (!((self.expr) == value)) {                                    \
            (self.expr) = std::forward<U>(value);                         \
            if (auto &_d = self.GetPropertyChangedEventDelegate()) {      \
                sw::PropertyChangedEventArgs _a{};                        \
                _a.propertyId = sw::Reflection::GetFieldId(&T::name);     \
                _d(self, _a);                                             \
            }                                                             \
        }                                                                 \
    }                                                                     \
    template <typename T, typename U, typename V = decltype(expr)>        \
    static auto _Set_##name(T &self, U &&value)                           \
        -> typename std::enable_if<                                       \
            !sw::_IsProperty<V>::value &&                                 \
            !sw::_EqOperationHelper<V, U>::value>::type                   \
    {                                                                     \
        (self.expr) = std::forward<U>(value);                             \
        if (auto &_d = self.GetPropertyChangedEventDelegate()) {          \
            sw::PropertyChangedEventArgs _a{};                            \
            _a.propertyId = sw::Reflection::GetFieldId(&T::name);         \
            _d(self, _a);                                                 \
        }                                                                 \
    }                                                                     \
    sw::Property<_SW_EXPR_PROPERTY_VALUETYPE(name, expr)> name            \
    {                                                                     \
        sw::Property<_SW_EXPR_PROPERTY_VALUETYPE(name, expr)>::Init(this) \
            .Getter([](auto self) {                                       \
                return _Get_##name(*self);                                \
            })                                                            \
            .Setter([](auto self, auto value) {                           \
                _Set_##name(*self, value);                                \
            })                                                            \
    }

/*================================================================================*/

// ObservableObject.h


namespace sw
{
    /**
     * @brief 可观察对象基类，实现属性更改通知功能
     */
    class ObservableObject : public DynamicObject,
                             public INotifyObjectDead,
                             public INotifyPropertyChanged
    {
    private:
        /**
         * @brief 属性更改事件委托
         */
        PropertyChangedEventHandler _propertyChanged;

        /**
         * @brief 对象销毁事件委托
         */
        ObjectDeadEventHandler _objectDead;

    public:
        /**
         * @brief 默认构造函数
         */
        ObservableObject() = default;

        // 删除拷贝构造函数
        ObservableObject(const ObservableObject &) = delete;

        // 删除移动构造函数
        ObservableObject(ObservableObject &&) = delete;

        // 删除拷贝赋值运算符
        ObservableObject &operator=(const ObservableObject &) = delete;

        // 删除移动赋值运算符
        ObservableObject &operator=(ObservableObject &&) = delete;

        /**
         * @brief 析构时触发对象销毁事件
         */
        virtual ~ObservableObject()
        {
            if (_objectDead) {
                EventArgs args{};
                _objectDead(*this, args);
            }
        }

    protected:
        /**
         * @brief 获取属性更改事件委托的引用
         * @note PropertyChanged事件使用该函数返回的委托来保存事件处理程序
         */
        virtual PropertyChangedEventHandler &GetPropertyChangedEventDelegate() override final
        {
            return _propertyChanged;
        }

        /**
         * @brief 获取对象销毁事件委托的引用
         * @note ObjectDead事件使用该函数返回的委托来保存事件处理程序
         */
        virtual ObjectDeadEventHandler &GetObjectDeadEventDelegate() override final
        {
            return _objectDead;
        }

        /**
         * @brief 触发属性更改通知事件
         * @param propertyId 更改的属性ID
         */
        void RaisePropertyChanged(FieldId propertyId)
        {
            if (_propertyChanged) {
                PropertyChangedEventArgs args{};
                args.propertyId = propertyId;
                _propertyChanged(*this, args);
            }
        }

        /**
         * @brief 触发属性更改通知事件
         * @tparam T 属性所属类类型
         * @tparam TProperty 属性类型
         * @param property 更改的属性成员指针
         */
        template <typename T, typename TProperty>
        void RaisePropertyChanged(TProperty T::*property)
        {
            FieldId id = Reflection::GetFieldId(property);
            RaisePropertyChanged(id);
        }
    };
}

// Binding.h


namespace sw
{
    /**
     * @brief 绑定模式枚举
     */
    enum class BindingMode {
        /// 一次性绑定，在绑定创建时更新目标属性值
        OneTime,

        /// 单向，从源到目标
        OneWay,

        /// 单向，从目标到源
        OneWayToSource,

        /// 双向，源和目标相互更新
        TwoWay,
    };

    /**
     * @brief 数据绑定基类
     */
    class BindingBase
    {
    public:
        /**
         * @brief 默认析构函数
         */
        virtual ~BindingBase() = default;

    public:
        /**
         * @brief 更新目标属性的值
         * @return 如果更新成功则返回true，否则返回false
         */
        virtual bool UpdateTarget() = 0;

        /**
         * @brief 更新源属性的值
         * @return 如果更新成功则返回true，否则返回false
         */
        virtual bool UpdateSource() = 0;

        /**
         * @brief 获取目标属性ID
         */
        virtual FieldId GetTargetPropertyId() const = 0;

        /**
         * @brief 获取源属性ID
         */
        virtual FieldId GetSourcePropertyId() const = 0;
    };

    /**
     * @brief 数据绑定类
     */
    class Binding final : public BindingBase
    {
    private:
        /**
         * @brief 绑定模式
         */
        BindingMode _mode;

        /**
         * @brief 目标对象
         */
        DynamicObject *_targetObject;

        /**
         * @brief 源对象
         */
        DynamicObject *_sourceObject;

        /**
         * @brief 目标属性ID
         */
        FieldId _targetPropertyId;

        /**
         * @brief 源属性ID
         */
        FieldId _sourcePropertyId;

        /**
         * @brief 值转换器
         */
        void *_converter;

        /**
         * @brief 值转换器删除函数
         */
        void (*_converterDeleter)(void *);

        /**
         * @brief 更新目标属性值委托
         */
        Func<Binding *, bool> _updateTargetFunc;

        /**
         * @brief 更新源属性值委托
         */
        Func<Binding *, bool> _updateSourceFunc;

    private:
        /**
         * @brief 默认构造函数
         */
        Binding() = default;

        // 删除拷贝构造函数
        Binding(const Binding &) = delete;

        // 删除移动构造函数
        Binding(Binding &&) = delete;

        // 删除拷贝赋值运算符
        Binding &operator=(const Binding &) = delete;

        // 删除移动赋值运算符
        Binding &operator=(Binding &&) = delete;

    public:
        /**
         * @brief 析构函数
         */
        virtual ~Binding()
        {
            UnregisterNotifications();

            if (_converterDeleter && _converter) {
                _converterDeleter(_converter);
            }
        }

        /**
         * @brief 更新目标属性的值
         * @return 如果更新成功则返回true，否则返回false
         */
        virtual bool UpdateTarget() override
        {
            bool result = false;

            if (_updateTargetFunc) {
                result = _updateTargetFunc(this);
            }
            return result;
        }

        /**
         * @brief 更新源属性的值
         * @return 如果更新成功则返回true，否则返回false
         */
        virtual bool UpdateSource() override
        {
            bool result = false;

            if (_updateSourceFunc) {
                result = _updateSourceFunc(this);
            }
            return result;
        }

        /**
         * @brief 获取目标属性ID
         */
        virtual FieldId GetTargetPropertyId() const override
        {
            return _targetPropertyId;
        }

        /**
         * @brief 获取源属性ID
         */
        virtual FieldId GetSourcePropertyId() const override
        {
            return _sourcePropertyId;
        }

        /**
         * @brief 获取绑定模式
         */
        BindingMode GetBindingMode() const
        {
            return _mode;
        }

        /**
         * @brief 获取目标对象
         */
        DynamicObject *GetTargetObject() const
        {
            return _targetObject;
        }

        /**
         * @brief 获取源对象
         */
        DynamicObject *GetSourceObject() const
        {
            return _sourceObject;
        }

        /**
         * @brief 修改绑定模式
         */
        void SetBindingMode(BindingMode mode)
        {
            if (_mode != mode) {
                _mode = mode;
                OnBindingChanged();
            }
        }

        /**
         * @brief 修改目标对象
         */
        void SetTargetObject(DynamicObject *target)
        {
            if (_targetObject != target) {
                UnregisterNotifications();
                _targetObject = target;
                RegisterNotifications();
                OnBindingChanged();
            }
        }

        /**
         * @brief 修改源对象
         */
        void SetSourceObject(DynamicObject *source)
        {
            if (_sourceObject != source) {
                UnregisterNotifications();
                _sourceObject = source;
                RegisterNotifications();
                OnBindingChanged();
            }
        }

        /**
         * @brief 修改目标对象和源对象
         */
        void SetBindingObjects(DynamicObject *target, DynamicObject *source)
        {
            if (_targetObject != target ||
                _sourceObject != source) //
            {
                UnregisterNotifications();
                _targetObject = target;
                _sourceObject = source;
                RegisterNotifications();
                OnBindingChanged();
            }
        }

    private:
        /**
         * @brief 注册属性更改通知
         */
        void RegisterNotifications()
        {
            INotifyPropertyChanged *targetNotifObj = nullptr;
            INotifyPropertyChanged *sourceNotifObj = nullptr;

            if (_targetObject != nullptr && _targetObject->IsType(&targetNotifObj)) {
                targetNotifObj->PropertyChanged +=
                    PropertyChangedEventHandler(*this, &Binding::OnTargetPropertyChanged);
            }
            if (_sourceObject != nullptr && _sourceObject->IsType(&sourceNotifObj)) {
                sourceNotifObj->PropertyChanged +=
                    PropertyChangedEventHandler(*this, &Binding::OnSourcePropertyChanged);
            }

            INotifyObjectDead *targetNotifObjDead = nullptr;
            INotifyObjectDead *sourceNotifObjDead = nullptr;

            if (_targetObject != nullptr && _targetObject->IsType(&targetNotifObjDead)) {
                targetNotifObjDead->ObjectDead +=
                    ObjectDeadEventHandler(*this, &Binding::OnTargetObjectDead);
            }
            if (_sourceObject != nullptr && _sourceObject->IsType(&sourceNotifObjDead)) {
                sourceNotifObjDead->ObjectDead +=
                    ObjectDeadEventHandler(*this, &Binding::OnSourceObjectDead);
            }
        }

        /**
         * @brief 注销属性更改通知
         */
        void UnregisterNotifications()
        {
            INotifyPropertyChanged *targetNotifObj = nullptr;
            INotifyPropertyChanged *sourceNotifObj = nullptr;

            if (_targetObject != nullptr && _targetObject->IsType(&targetNotifObj)) {
                targetNotifObj->PropertyChanged -=
                    PropertyChangedEventHandler(*this, &Binding::OnTargetPropertyChanged);
            }
            if (_sourceObject != nullptr && _sourceObject->IsType(&sourceNotifObj)) {
                sourceNotifObj->PropertyChanged -=
                    PropertyChangedEventHandler(*this, &Binding::OnSourcePropertyChanged);
            }

            INotifyObjectDead *targetNotifObjDead = nullptr;
            INotifyObjectDead *sourceNotifObjDead = nullptr;

            if (_targetObject != nullptr && _targetObject->IsType(&targetNotifObjDead)) {
                targetNotifObjDead->ObjectDead -=
                    ObjectDeadEventHandler(*this, &Binding::OnTargetObjectDead);
            }
            if (_sourceObject != nullptr && _sourceObject->IsType(&sourceNotifObjDead)) {
                sourceNotifObjDead->ObjectDead -=
                    ObjectDeadEventHandler(*this, &Binding::OnSourceObjectDead);
            }
        }

        /**
         * @brief 目标属性更改处理函数
         */
        void OnTargetPropertyChanged(INotifyPropertyChanged &sender, PropertyChangedEventArgs &e)
        {
            if (e.propertyId != _targetPropertyId) {
                return;
            }

            if (_mode == BindingMode::TwoWay ||
                _mode == BindingMode::OneWayToSource) {
                UpdateSource();
            }
        }

        /**
         * @brief 源属性更改处理函数
         */
        void OnSourcePropertyChanged(INotifyPropertyChanged &sender, PropertyChangedEventArgs &e)
        {
            if (e.propertyId != _sourcePropertyId) {
                return;
            }

            if (_mode == BindingMode::TwoWay ||
                _mode == BindingMode::OneWay) {
                UpdateTarget();
            }
        }

        /**
         * @brief 目标对象销毁处理函数
         */
        void OnTargetObjectDead(INotifyObjectDead &sender, EventArgs &e)
        {
            SetTargetObject(nullptr);
        }

        /**
         * @brief 源对象销毁处理函数
         */
        void OnSourceObjectDead(INotifyObjectDead &sender, EventArgs &e)
        {
            SetSourceObject(nullptr);
        }

        /**
         * @brief 绑定创建和发生更改时调用
         */
        void OnBindingChanged()
        {
            switch (_mode) {
                case BindingMode::OneTime:
                case BindingMode::OneWay:
                case BindingMode::TwoWay: {
                    UpdateTarget();
                    break;
                }
                case BindingMode::OneWayToSource: {
                    UpdateSource();
                    break;
                }
            }
        }

        /**
         * @brief 内部创建绑定对象函数
         * @param target 目标对象指针
         * @param targetPropertyId 目标属性ID
         * @param source 源对象指针
         * @param sourcePropertyId 源属性ID
         * @param mode 绑定模式
         * @param converter 值转换器指针
         * @return 绑定对象指针
         */
        template <typename TTargetValue, typename TSourceValue>
        static Binding *Create(DynamicObject *target, FieldId targetPropertyId,
                               DynamicObject *source, FieldId sourcePropertyId,
                               BindingMode mode, IValueConverter<TSourceValue, TTargetValue> *converter)
        {
            auto binding = new Binding;

            binding->_targetObject     = target;
            binding->_sourceObject     = source;
            binding->_targetPropertyId = targetPropertyId;
            binding->_sourcePropertyId = sourcePropertyId;

            binding->_mode      = mode;
            binding->_converter = converter;

            binding->_converterDeleter = [](void *ptr) {
                delete reinterpret_cast<IValueConverter<TSourceValue, TTargetValue> *>(ptr);
            };
            return binding;
        }

    public:
        /**
         * @brief 创建绑定对象
         * @param target 目标对象指针
         * @param targetProperty 目标属性成员指针
         * @param source 源对象指针
         * @param sourceProperty 源属性成员指针
         * @param mode 绑定模式
         * @param converter 值转换器指针
         * @return 绑定对象指针
         * @note 转换器的生命周期将由绑定对象管理，请勿与其他对象共享
         */
        template <
            typename TTargetObject,
            typename TTargetProperty,
            typename TSourceObject,
            typename TSourceProperty>
        static auto Create(DynamicObject *target, TTargetProperty TTargetObject::*targetProperty,
                           DynamicObject *source, TSourceProperty TSourceObject::*sourceProperty,
                           BindingMode mode,
                           IValueConverter<typename TSourceProperty::TValue, typename TTargetProperty::TValue> *converter = nullptr)
            -> typename std::enable_if<
                _IsProperty<TTargetProperty>::value &&
                    _IsProperty<TSourceProperty>::value &&
                    std::is_base_of<DynamicObject, TTargetObject>::value &&
                    std::is_base_of<DynamicObject, TSourceObject>::value &&
                    BindingCastHelper<typename TSourceProperty::TValue, typename TTargetProperty::TValue>::value,
                Binding *>::type
        {
            using TTargetValue  = typename TTargetProperty::TValue;
            using TSourceValue  = typename TSourceProperty::TValue;
            using DefaultCaster = BindingCastHelper<TSourceValue, TTargetValue>;

            auto binding = Create(
                target, Reflection::GetFieldId(targetProperty),
                source, Reflection::GetFieldId(sourceProperty), mode, converter);

            // update target action
            binding->_updateTargetFunc = [targetSetter = Reflection::GetPropertySetter(targetProperty),
                                          sourceGetter = Reflection::GetPropertyGetter(sourceProperty)](Binding *binding) -> bool //
            {
                IValueConverter<TSourceValue, TTargetValue> *converter =
                    reinterpret_cast<IValueConverter<TSourceValue, TTargetValue> *>(binding->_converter);

                if (targetSetter == nullptr ||
                    sourceGetter == nullptr ||
                    binding->_targetObject == nullptr ||
                    binding->_sourceObject == nullptr) {
                    return false;
                }

                if (converter) {
                    targetSetter(
                        *binding->_targetObject,
                        converter->Convert(sourceGetter(*binding->_sourceObject)));
                } else {
                    targetSetter(
                        *binding->_targetObject,
                        DefaultCaster::Convert(sourceGetter(*binding->_sourceObject)));
                }
                return true;
            };

            // update source action
            binding->_updateSourceFunc = [targetGetter = Reflection::GetPropertyGetter(targetProperty),
                                          sourceSetter = Reflection::GetPropertySetter(sourceProperty)](Binding *binding) -> bool //
            {
                IValueConverter<TSourceValue, TTargetValue> *converter =
                    reinterpret_cast<IValueConverter<TSourceValue, TTargetValue> *>(binding->_converter);

                if (targetGetter == nullptr ||
                    sourceSetter == nullptr ||
                    binding->_targetObject == nullptr ||
                    binding->_sourceObject == nullptr) {
                    return false;
                }

                if (converter) {
                    sourceSetter(
                        *binding->_sourceObject,
                        converter->ConvertBack(targetGetter(*binding->_targetObject)));
                } else {
                    sourceSetter(
                        *binding->_sourceObject,
                        DefaultCaster::ConvertBack(targetGetter(*binding->_targetObject)));
                }
                return true;
            };

            binding->RegisterNotifications();
            binding->OnBindingChanged();
            return binding;
        }

        /**
         * @brief 创建延迟绑定对象
         * @param targetProperty 目标属性成员指针
         * @param source 源对象指针
         * @param sourceProperty 源属性成员指针
         * @param mode 绑定模式
         * @param converter 值转换器指针
         * @return 绑定对象指针
         * @note 转换器的生命周期将由绑定对象管理，请勿与其他对象共享
         */
        template <
            typename TTargetObject,
            typename TTargetProperty,
            typename TSourceObject,
            typename TSourceProperty>
        static auto Create(TTargetProperty TTargetObject::*targetProperty,
                           DynamicObject *source,
                           TSourceProperty TSourceObject::*sourceProperty,
                           BindingMode mode,
                           IValueConverter<typename TSourceProperty::TValue, typename TTargetProperty::TValue> *converter = nullptr)
            -> typename std::enable_if<
                _IsProperty<TTargetProperty>::value &&
                    _IsProperty<TSourceProperty>::value &&
                    std::is_base_of<DynamicObject, TTargetObject>::value &&
                    std::is_base_of<DynamicObject, TSourceObject>::value &&
                    BindingCastHelper<typename TSourceProperty::TValue, typename TTargetProperty::TValue>::value,
                Binding *>::type
        {
            return Create(nullptr, targetProperty, source, sourceProperty, mode, converter);
        }

        /**
         * @brief 创建延迟绑定对象
         * @param targetProperty 目标属性成员指针
         * @param sourceProperty 源属性成员指针
         * @param mode 绑定模式
         * @param converter 值转换器指针
         * @return 绑定对象指针
         * @note 转换器的生命周期将由绑定对象管理，请勿与其他对象共享
         */
        template <
            typename TTargetObject,
            typename TTargetProperty,
            typename TSourceObject,
            typename TSourceProperty>
        static auto Create(TTargetProperty TTargetObject::*targetProperty,
                           TSourceProperty TSourceObject::*sourceProperty,
                           BindingMode mode,
                           IValueConverter<typename TSourceProperty::TValue, typename TTargetProperty::TValue> *converter = nullptr)
            -> typename std::enable_if<
                _IsProperty<TTargetProperty>::value &&
                    _IsProperty<TSourceProperty>::value &&
                    std::is_base_of<DynamicObject, TTargetObject>::value &&
                    std::is_base_of<DynamicObject, TSourceObject>::value &&
                    BindingCastHelper<typename TSourceProperty::TValue, typename TTargetProperty::TValue>::value,
                Binding *>::type
        {
            return Create(nullptr, targetProperty, nullptr, sourceProperty, mode, converter);
        }

        /**
         * @brief 创建绑定对象
         * @param target 目标对象指针
         * @param targetProperty 目标属性成员指针
         * @param source 源对象指针
         * @param sourceProperty 源属性成员指针
         * @param mode 绑定模式
         * @param converter 值转换器指针
         * @return 绑定对象指针
         * @note 转换器的生命周期将由绑定对象管理，请勿与其他对象共享
         */
        template <
            typename TTargetObject,
            typename TTargetProperty,
            typename TSourceObject,
            typename TSourceProperty>
        static auto Create(DynamicObject *target, TTargetProperty TTargetObject::*targetProperty,
                           DynamicObject *source, TSourceProperty TSourceObject::*sourceProperty,
                           BindingMode mode,
                           IValueConverter<typename TSourceProperty::TValue, typename TTargetProperty::TValue> *converter)
            -> typename std::enable_if<
                _IsProperty<TTargetProperty>::value &&
                    _IsProperty<TSourceProperty>::value &&
                    std::is_base_of<DynamicObject, TTargetObject>::value &&
                    std::is_base_of<DynamicObject, TSourceObject>::value &&
                    !BindingCastHelper<typename TSourceProperty::TValue, typename TTargetProperty::TValue>::value,
                Binding *>::type
        {
            using TTargetValue = typename TTargetProperty::TValue;
            using TSourceValue = typename TSourceProperty::TValue;

            auto binding = Create(
                target, Reflection::GetFieldId(targetProperty),
                source, Reflection::GetFieldId(sourceProperty), mode, converter);

            // update target action
            binding->_updateTargetFunc = [targetSetter = Reflection::GetPropertySetter(targetProperty),
                                          sourceGetter = Reflection::GetPropertyGetter(sourceProperty)](Binding *binding) -> bool //
            {
                IValueConverter<TSourceValue, TTargetValue> *converter =
                    reinterpret_cast<IValueConverter<TSourceValue, TTargetValue> *>(binding->_converter);

                if (targetSetter == nullptr ||
                    sourceGetter == nullptr ||
                    converter == nullptr ||
                    binding->_targetObject == nullptr ||
                    binding->_sourceObject == nullptr) {
                    return false;
                }

                targetSetter(
                    *binding->_targetObject,
                    converter->Convert(sourceGetter(*binding->_sourceObject)));
                return true;
            };

            // update source action
            binding->_updateSourceFunc = [targetGetter = Reflection::GetPropertyGetter(targetProperty),
                                          sourceSetter = Reflection::GetPropertySetter(sourceProperty)](Binding *binding) -> bool //
            {
                IValueConverter<TSourceValue, TTargetValue> *converter =
                    reinterpret_cast<IValueConverter<TSourceValue, TTargetValue> *>(binding->_converter);

                if (targetGetter == nullptr ||
                    sourceSetter == nullptr ||
                    converter == nullptr ||
                    binding->_targetObject == nullptr ||
                    binding->_sourceObject == nullptr) {
                    return false;
                }

                sourceSetter(
                    *binding->_sourceObject,
                    converter->ConvertBack(targetGetter(*binding->_targetObject)));
                return true;
            };

            binding->RegisterNotifications();
            binding->OnBindingChanged();
            return binding;
        }

        /**
         * @brief 创建延迟绑定对象
         * @param targetProperty 目标属性成员指针
         * @param source 源对象指针
         * @param sourceProperty 源属性成员指针
         * @param mode 绑定模式
         * @param converter 值转换器指针
         * @return 绑定对象指针
         * @note 转换器的生命周期将由绑定对象管理，请勿与其他对象共享
         */
        template <
            typename TTargetObject,
            typename TTargetProperty,
            typename TSourceObject,
            typename TSourceProperty>
        static auto Create(TTargetProperty TTargetObject::*targetProperty,
                           DynamicObject *source,
                           TSourceProperty TSourceObject::*sourceProperty,
                           BindingMode mode,
                           IValueConverter<typename TSourceProperty::TValue, typename TTargetProperty::TValue> *converter)
            -> typename std::enable_if<
                _IsProperty<TTargetProperty>::value &&
                    _IsProperty<TSourceProperty>::value &&
                    std::is_base_of<DynamicObject, TTargetObject>::value &&
                    std::is_base_of<DynamicObject, TSourceObject>::value &&
                    !BindingCastHelper<typename TSourceProperty::TValue, typename TTargetProperty::TValue>::value,
                Binding *>::type
        {
            return Create(nullptr, targetProperty, source, sourceProperty, mode, converter);
        }

        /**
         * @brief 创建延迟绑定对象
         * @param targetProperty 目标属性成员指针
         * @param sourceProperty 源属性成员指针
         * @param mode 绑定模式
         * @param converter 值转换器指针
         * @return 绑定对象指针
         * @note 转换器的生命周期将由绑定对象管理，请勿与其他对象共享
         */
        template <
            typename TTargetObject,
            typename TTargetProperty,
            typename TSourceObject,
            typename TSourceProperty>
        static auto Create(TTargetProperty TTargetObject::*targetProperty,
                           TSourceProperty TSourceObject::*sourceProperty,
                           BindingMode mode,
                           IValueConverter<typename TSourceProperty::TValue, typename TTargetProperty::TValue> *converter)
            -> typename std::enable_if<
                _IsProperty<TTargetProperty>::value &&
                    _IsProperty<TSourceProperty>::value &&
                    std::is_base_of<DynamicObject, TTargetObject>::value &&
                    std::is_base_of<DynamicObject, TSourceObject>::value &&
                    !BindingCastHelper<typename TSourceProperty::TValue, typename TTargetProperty::TValue>::value,
                Binding *>::type
        {
            return Create(nullptr, targetProperty, nullptr, sourceProperty, mode, converter);
        }
    };
}

// CanvasLayout.h


namespace sw
{
    /**
     * @brief 绝对位置布局方式的布局标记
     */
    struct CanvasLayoutTag {
        /**
         * @brief 左边
         */
        float left;

        /**
         * @brief 顶边
         */
        float top;

        /**
         * @brief 左边顶边均为0
         */
        CanvasLayoutTag();

        /**
         * @brief 指定左边和顶边
         */
        CanvasLayoutTag(float left, float top);

        /**
         * @brief 从LayoutTag创建
         */
        CanvasLayoutTag(uint64_t layoutTag);

        /**
         * @brief 隐式转换LayoutTag
         */
        operator uint64_t() const;
    };

    /**
     * @brief 绝对位置布局方式
     */
    class CanvasLayout : public LayoutHost
    {
    public:
        /**
         * @brief 测量元素所需尺寸，无需考虑边框和边距
         * @param availableSize 可用的尺寸
         * @return 返回元素需要占用的尺寸
         */
        virtual Size MeasureOverride(const Size &availableSize) override;

        /**
         * @brief 安排子元素的位置，可重写该函数以实现自定义布局
         * @param finalSize 可用于排列子元素的最终尺寸
         */
        virtual void ArrangeOverride(const Size &finalSize) override;
    };
}

// DockLayout.h


namespace sw
{
    /**
     * @brief 停靠布局标记
     */
    class DockLayoutTag : public IEqualityComparable<DockLayoutTag>
    {
    public:
        enum : uint64_t {
            Left,   ///< 左边
            Top,    ///< 顶边
            Right,  ///< 右边
            Bottom, ///< 底边
        };

    private:
        /**
         * @brief Tag值
         */
        uint64_t _value;

    public:
        /**
         * @brief 创建DockLayoutTag
         */
        DockLayoutTag(uint64_t value = Left);

        /**
         * @brief 隐式转换uint64_t
         */
        operator uint64_t() const;

        /**
         * @brief 判断值是否相等
         */
        bool Equals(const DockLayoutTag &other) const;

        /**
         * @brief 判断值是否相等
         */
        bool operator==(uint64_t value) const;

        /**
         * @brief 判断值是否不相等
         */
        bool operator!=(uint64_t value) const;
    };

    /**
     * @brief 停靠布局
     */
    class DockLayout : public LayoutHost
    {
    public:
        /**
         * @brief 最后一个子元素是否填充剩余空间
         */
        bool lastChildFill = true;

        /**
         * @brief 测量元素所需尺寸，无需考虑边框和边距
         * @param availableSize 可用的尺寸
         * @return 返回元素需要占用的尺寸
         */
        virtual Size MeasureOverride(const Size &availableSize) override;

        /**
         * @brief 安排子元素的位置，可重写该函数以实现自定义布局
         * @param finalSize 可用于排列子元素的最终尺寸
         */
        virtual void ArrangeOverride(const Size &finalSize) override;
    };
}

// FillLayout.h


namespace sw
{
    /**
     * @brief 一种将全部元素都铺满的布局，一般用于在只有一个子元素的时候将该元素铺满整个可用区域
     */
    class FillLayout : public LayoutHost
    {
    public:
        /**
         * @brief 测量元素所需尺寸，无需考虑边框和边距
         * @param availableSize 可用的尺寸
         * @return 返回元素需要占用的尺寸
         */
        virtual Size MeasureOverride(const Size &availableSize) override;

        /**
         * @brief 安排子元素的位置，可重写该函数以实现自定义布局
         * @param finalSize 可用于排列子元素的最终尺寸
         */
        virtual void ArrangeOverride(const Size &finalSize) override;
    };
}

// INotifyCollectionChanged.h


namespace sw
{
    // 前向声明
    class INotifyCollectionChanged;
    class NotifyCollectionChangedEventArgs;

    /**
     * @brief 集合变更事件处理函数类型
     */
    using NotifyCollectionChangedEventHandler =
        EventHandler<INotifyCollectionChanged, NotifyCollectionChangedEventArgs>;

    /**
     * @brief 描述集合变更的原因
     */
    enum class NotifyCollectionChangedAction {
        Add,     ///< 添加项
        Remove,  ///< 移除项
        Replace, ///< 替换项
        Move,    ///< 移动项
        Reset    ///< 重置集合
    };

    /**
     * @brief 集合变更事件参数类型
     */
    class NotifyCollectionChangedEventArgs : public EventArgs
    {
    public:
        /**
         * @brief 集合变更的原因
         */
        NotifyCollectionChangedAction action =
            NotifyCollectionChangedAction::Reset;

        /**
         * @brief 变更发生的集合对象
         */
        IList *list = nullptr;

        /**
         * @brief 变更发生的项索引
         */
        int index = -1;

        /**
         * @brief 记录移动项的原始索引
         */
        int oldIndex = -1;
    };

    /**
     * @brief 支持集合变更通知的接口
     */
    class INotifyCollectionChanged
    {
    public:
        /**
         * @brief 集合变更事件，当集合发生添加、移除、替换、移动或重置等变更时触发
         */
        Event<NotifyCollectionChangedEventHandler> CollectionChanged{
            Event<NotifyCollectionChangedEventHandler>::Init(this)
                .Delegate([](INotifyCollectionChanged *self) -> NotifyCollectionChangedEventHandler & {
                    return self->GetCollectionChangedEventDelegate();
                }) //
        };

        /**
         * @brief 虚析构函数
         */
        virtual ~INotifyCollectionChanged() = default;

    protected:
        /**
         * @brief 获取集合变更事件委托的引用
         * @note CollectionChanged事件使用该函数返回的委托来保存事件处理程序
         */
        virtual NotifyCollectionChangedEventHandler &GetCollectionChangedEventDelegate() = 0;
    };
}

// List.h


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
         * @brief 获取指定索引处的元素引用
         * @param index 元素索引
         * @return 元素引用
         * @throws std::out_of_range 索引超出范围
         */
        T &operator[](int index)
        {
            return GetAt(index);
        }

        /**
         * @brief 获取指定索引处的const元素引用
         * @param index 元素索引
         * @return const元素引用
         * @throws std::out_of_range 索引超出范围
         */
        const T &operator[](int index) const
        {
            return GetAt(index);
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
         */
        void Add(const T &value)
        {
            _data.push_back(value);
        }

        /**
         * @brief 在列表末尾追加元素（移动语义）
         * @param value 要追加的值
         */
        void Add(T &&value)
        {
            _data.push_back(std::move(value));
        }

        /**
         * @brief 移除指定索引处的元素
         * @param index 要移除的元素索引
         * @throws std::out_of_range 索引超出范围
         */
        void RemoveAt(int index)
        {
            if (index < 0 || index >= Count()) {
                throw std::out_of_range("Index out of range in List::RemoveAt.");
            }
            _data.erase(_data.begin() + static_cast<size_t>(index));
        }

        /**
         * @brief 在指定索引处插入元素
         * @param index 插入位置
         * @param value 要插入的值
         * @throws std::out_of_range 索引超出范围
         */
        void Insert(int index, const T &value)
        {
            if (index < 0 || index > Count()) {
                throw std::out_of_range("Index out of range in List::Insert.");
            }
            _data.insert(_data.begin() + static_cast<size_t>(index), value);
        }

        /**
         * @brief 在指定索引处插入元素（移动语义）
         * @param index 插入位置
         * @param value 要插入的值
         * @throws std::out_of_range 索引超出范围
         */
        void Insert(int index, T &&value)
        {
            if (index < 0 || index > Count()) {
                throw std::out_of_range("Index out of range in List::Insert.");
            }
            _data.insert(_data.begin() + static_cast<size_t>(index), std::move(value));
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
                RemoveAt(index);
                return true;
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
        std::vector<T> &GetInternalVector() noexcept
        {
            return _data;
        }

        /**
         * @brief 获取底层std::vector的const引用
         * @return std::vector的const引用
         */
        const std::vector<T> &GetInternalVector() const noexcept
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
        auto SetAtImpl(int index, const T &value)
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
        auto SetAtImpl(int index, const T &value)
            -> typename std::enable_if<!std::is_copy_assignable<U>::value>::type
        {
            throw std::logic_error("Type T must be copy assignable to use SetAt in List.");
        }

        /**
         * @brief 设置元素值的实现（移动语义，T可移动赋值时）
         * @param index 元素索引
         * @param value 要设置的值
         * @throws std::out_of_range 索引超出范围
         */
        template <typename U = T>
        auto SetAtImpl(int index, T &&value)
            -> typename std::enable_if<std::is_move_assignable<U>::value>::type
        {
            if (index < 0 || index >= Count()) {
                throw std::out_of_range("Index out of range in List::SetAt.");
            }
            _data[static_cast<size_t>(index)] = std::move(value);
        }

        /**
         * @brief 设置元素值的实现（移动语义，T不可移动赋值时，抛出异常）
         * @param index 元素索引
         * @param value 要设置的值
         * @throws std::logic_error T不可移动赋值
         */
        template <typename U = T>
        auto SetAtImpl(int index, T &&value)
            -> typename std::enable_if<!std::is_move_assignable<U>::value>::type
        {
            throw std::logic_error("Type T must be move assignable to use move SetAt in List.");
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
         * @throws std::logic_error T不可拷贝赋值时
         */
        virtual void SetAt(int index, const T &value) override
        {
            SetAtImpl(index, value);
        }

        /**
         * @brief 设置指定索引处的元素值（移动语义）
         * @param index 元素索引
         * @param value 要设置的值
         * @throws std::out_of_range 索引超出范围
         * @throws std::logic_error T不可移动赋值时
         */
        virtual void SetAt(int index, T &&value) override
        {
            SetAtImpl(index, std::move(value));
        }
    };
}

// StackLayout.h


namespace sw
{
    /**
     * @brief 横向堆叠布局
     */
    class StackLayoutH : public LayoutHost
    {
    public:
        /**
         * @brief 测量元素所需尺寸，无需考虑边框和边距
         * @param availableSize 可用的尺寸
         * @return 返回元素需要占用的尺寸
         */
        virtual Size MeasureOverride(const Size &availableSize) override;

        /**
         * @brief 安排子元素的位置，可重写该函数以实现自定义布局
         * @param finalSize 可用于排列子元素的最终尺寸
         */
        virtual void ArrangeOverride(const Size &finalSize) override;
    };

    /**
     * @brief 纵向堆叠布局
     */
    class StackLayoutV : public LayoutHost
    {
    public:
        /**
         * @brief 测量元素所需尺寸，无需考虑边框和边距
         * @param availableSize 可用的尺寸
         * @return 返回元素需要占用的尺寸
         */
        virtual Size MeasureOverride(const Size &availableSize) override;

        /**
         * @brief 安排子元素的位置，可重写该函数以实现自定义布局
         * @param finalSize 可用于排列子元素的最终尺寸
         */
        virtual void ArrangeOverride(const Size &finalSize) override;
    };

    /**
     * @brief 堆叠布局
     */
    class StackLayout : public LayoutHost
    {
    public:
        /**
         * @brief 排列方式
         */
        Orientation orientation = Orientation::Vertical;

        /**
         * @brief 测量元素所需尺寸，无需考虑边框和边距
         * @param availableSize 可用的尺寸
         * @return 返回元素需要占用的尺寸
         */
        virtual Size MeasureOverride(const Size &availableSize) override;

        /**
         * @brief 安排子元素的位置，可重写该函数以实现自定义布局
         * @param finalSize 可用于排列子元素的最终尺寸
         */
        virtual void ArrangeOverride(const Size &finalSize) override;
    };
}

// UniformGridLayout.h


namespace sw
{
    /**
     * @brief 均匀大小网格布局
     */
    class UniformGridLayout : public LayoutHost
    {
    public:
        /**
         * @brief 行数
         */
        int rows = 1;

        /**
         * @brief 列数
         */
        int columns = 1;

        /**
         * @brief 网格第一行中前导空白单元格的数量
         */
        int firstColumn = 0;

        /**
         * @brief 测量元素所需尺寸，无需考虑边框和边距
         * @param availableSize 可用的尺寸
         * @return 返回元素需要占用的尺寸
         */
        virtual Size MeasureOverride(const Size &availableSize) override;

        /**
         * @brief 安排子元素的位置，可重写该函数以实现自定义布局
         * @param finalSize 可用于排列子元素的最终尺寸
         */
        virtual void ArrangeOverride(const Size &finalSize) override;
    };
}

// WrapLayout.h


namespace sw
{
    /**
     * @brief 横向自动换行布局
     */
    class WrapLayoutH : public LayoutHost
    {
    public:
        /**
         * @brief 测量元素所需尺寸，无需考虑边框和边距
         * @param availableSize 可用的尺寸
         * @return 返回元素需要占用的尺寸
         */
        virtual Size MeasureOverride(const Size &availableSize) override;

        /**
         * @brief 安排子元素的位置，可重写该函数以实现自定义布局
         * @param finalSize 可用于排列子元素的最终尺寸
         */
        virtual void ArrangeOverride(const Size &finalSize) override;
    };

    /**
     * @brief 纵向自动换行布局
     */
    class WrapLayoutV : public LayoutHost
    {
    public:
        /**
         * @brief 测量元素所需尺寸，无需考虑边框和边距
         * @param availableSize 可用的尺寸
         * @return 返回元素需要占用的尺寸
         */
        virtual Size MeasureOverride(const Size &availableSize) override;

        /**
         * @brief 安排子元素的位置，可重写该函数以实现自定义布局
         * @param finalSize 可用于排列子元素的最终尺寸
         */
        virtual void ArrangeOverride(const Size &finalSize) override;
    };

    /**
     * @brief 自动换行布局
     */
    class WrapLayout : public LayoutHost
    {
    public:
        /**
         * @brief 排列方式
         */
        Orientation orientation = Orientation::Horizontal;

        /**
         * @brief 测量元素所需尺寸，无需考虑边框和边距
         * @param availableSize 可用的尺寸
         * @return 返回元素需要占用的尺寸
         */
        virtual Size MeasureOverride(const Size &availableSize) override;

        /**
         * @brief 安排子元素的位置，可重写该函数以实现自定义布局
         * @param finalSize 可用于排列子元素的最终尺寸
         */
        virtual void ArrangeOverride(const Size &finalSize) override;
    };
}

// ObservableCollection.h


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
         */
        void Add(const T &value)
        {
            int index = _items.Count();
            _items.Add(value);

            NotifyCollectionChangedEventArgs args{};
            args.action = NotifyCollectionChangedAction::Add;
            args.list   = this;
            args.index  = index;
            OnCollectionChanged(args);
        }

        /**
         * @brief 在集合末尾追加元素（移动语义），并触发添加通知
         * @param value 要追加的值
         */
        void Add(T &&value)
        {
            int index = _items.Count();
            _items.Add(std::move(value));

            NotifyCollectionChangedEventArgs args{};
            args.action = NotifyCollectionChangedAction::Add;
            args.list   = this;
            args.index  = index;
            OnCollectionChanged(args);
        }

        /**
         * @brief 移除指定索引处的元素，并触发移除通知
         * @param index 要移除的元素索引
         * @throws std::out_of_range 索引超出范围
         */
        void RemoveAt(int index)
        {
            _items.RemoveAt(index);

            NotifyCollectionChangedEventArgs args{};
            args.action = NotifyCollectionChangedAction::Remove;
            args.list   = this;
            args.index  = index;
            OnCollectionChanged(args);
        }

        /**
         * @brief 在指定索引处插入元素，并触发添加通知
         * @param index 插入位置
         * @param value 要插入的值
         * @throws std::out_of_range 索引超出范围
         */
        void Insert(int index, const T &value)
        {
            _items.Insert(index, value);

            NotifyCollectionChangedEventArgs args{};
            args.action = NotifyCollectionChangedAction::Add;
            args.list   = this;
            args.index  = index;
            OnCollectionChanged(args);
        }

        /**
         * @brief 在指定索引处插入元素（移动语义），并触发添加通知
         * @param index 插入位置
         * @param value 要插入的值
         * @throws std::out_of_range 索引超出范围
         */
        void Insert(int index, T &&value)
        {
            _items.Insert(index, std::move(value));

            NotifyCollectionChangedEventArgs args{};
            args.action = NotifyCollectionChangedAction::Add;
            args.list   = this;
            args.index  = index;
            OnCollectionChanged(args);
        }

        /**
         * @brief 将元素从一个索引移动到另一个索引，并触发移动通知
         * @param oldIndex 要移动的元素原索引
         * @param newIndex 移动后的元素索引
         * @throws std::out_of_range 索引超出范围
         */
        void Move(int oldIndex, int newIndex)
        {
            int count = _items.Count();
            if (oldIndex < 0 || oldIndex >= count || newIndex < 0 || newIndex >= count) {
                throw std::out_of_range("Index out of range in ObservableCollection::Move.");
            }

            if (oldIndex == newIndex) {
                return;
            }

            auto &items = _items.GetInternalVector();

            T value = std::move(items[static_cast<size_t>(oldIndex)]);
            items.erase(items.begin() + static_cast<size_t>(oldIndex));
            items.insert(items.begin() + static_cast<size_t>(newIndex), std::move(value));

            NotifyCollectionChangedEventArgs args{};
            args.action   = NotifyCollectionChangedAction::Move;
            args.list     = this;
            args.index    = newIndex;
            args.oldIndex = oldIndex;
            OnCollectionChanged(args);
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
        std::vector<T> &GetInternalVector() noexcept
        {
            return _items.GetInternalVector();
        }

        /**
         * @brief 获取底层std::vector的const引用
         * @return std::vector的const引用
         */
        const std::vector<T> &GetInternalVector() const noexcept
        {
            return _items.GetInternalVector();
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
         * @throws std::logic_error T不可拷贝赋值时
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

// SelfBinding.h


namespace sw
{
    /**
     * @brief 自绑定类，用于同一对象内部属性之间的绑定
     */
    class SelfBinding final : public BindingBase
    {
    private:
        /**
         * @brief 内部绑定对象
         */
        std::unique_ptr<Binding> _innerBinding;

    private:
        /**
         * @brief 构造函数
         * @param binding 内部绑定对象
         */
        SelfBinding(Binding *binding)
            : _innerBinding(binding)
        {
        }

    public:
        /**
         * @brief 更新目标属性的值
         * @return 如果更新成功则返回true，否则返回false
         */
        virtual bool UpdateTarget() override
        {
            return _innerBinding->UpdateTarget();
        }

        /**
         * @brief 更新源属性的值
         * @return 如果更新成功则返回true，否则返回false
         */
        virtual bool UpdateSource() override
        {
            return _innerBinding->UpdateSource();
        }

        /**
         * @brief 获取目标属性ID
         */
        virtual FieldId GetTargetPropertyId() const override
        {
            return _innerBinding->GetTargetPropertyId();
        }

        /**
         * @brief 获取源属性ID
         */
        virtual FieldId GetSourcePropertyId() const override
        {
            return _innerBinding->GetSourcePropertyId();
        }

        /**
         * @brief 获取绑定模式
         */
        BindingMode GetBindingMode() const
        {
            return _innerBinding->GetBindingMode();
        }

        /**
         * @brief 设置绑定模式
         */
        void SetBindingMode(BindingMode mode)
        {
            _innerBinding->SetBindingMode(mode);
        }

        /**
         * @brief 获取目标对象
         */
        DynamicObject *GetTargetObject() const
        {
            return _innerBinding->GetTargetObject();
        }

        /**
         * @brief 修改目标对象
         */
        void SetTargetObject(DynamicObject *target)
        {
            _innerBinding->SetBindingObjects(target, target);
        }

    public:
        /**
         * @brief 创建自绑定对象
         * @param targetProperty 目标属性成员指针
         * @param sourceProperty 源属性成员指针
         * @param mode 绑定模式
         * @param converter 值转换器指针
         * @return 绑定对象指针
         * @note 转换器的生命周期将由绑定对象管理，请勿与其他对象共享
         */
        template <
            typename TTargetObject,
            typename TTargetProperty,
            typename TSourceObject,
            typename TSourceProperty>
        static auto Create(TTargetProperty TTargetObject::*targetProperty,
                           TSourceProperty TSourceObject::*sourceProperty,
                           BindingMode mode,
                           IValueConverter<typename TSourceProperty::TValue, typename TTargetProperty::TValue> *converter = nullptr)
            -> typename std::enable_if<
                _IsProperty<TTargetProperty>::value &&
                    _IsProperty<TSourceProperty>::value &&
                    std::is_base_of<DynamicObject, TTargetObject>::value &&
                    std::is_base_of<DynamicObject, TSourceObject>::value &&
                    BindingCastHelper<typename TSourceProperty::TValue, typename TTargetProperty::TValue>::value,
                SelfBinding *>::type
        {
            return new SelfBinding(Binding::Create(targetProperty, sourceProperty, mode, converter));
        }

        /**
         * @brief 创建自绑定对象
         * @param targetProperty 目标属性成员指针
         * @param sourceProperty 源属性成员指针
         * @param mode 绑定模式
         * @param converter 值转换器指针
         * @return 绑定对象指针
         * @note 转换器的生命周期将由绑定对象管理，请勿与其他对象共享
         */
        template <
            typename TTargetObject,
            typename TTargetProperty,
            typename TSourceObject,
            typename TSourceProperty>
        static auto Create(TTargetProperty TTargetObject::*targetProperty,
                           TSourceProperty TSourceObject::*sourceProperty,
                           BindingMode mode,
                           IValueConverter<typename TSourceProperty::TValue, typename TTargetProperty::TValue> *converter)
            -> typename std::enable_if<
                _IsProperty<TTargetProperty>::value &&
                    _IsProperty<TSourceProperty>::value &&
                    std::is_base_of<DynamicObject, TTargetObject>::value &&
                    std::is_base_of<DynamicObject, TSourceObject>::value &&
                    !BindingCastHelper<typename TSourceProperty::TValue, typename TTargetProperty::TValue>::value,
                SelfBinding *>::type
        {
            return new SelfBinding(Binding::Create(targetProperty, sourceProperty, mode, converter));
        }
    };
}

// FileDialog.h


namespace sw
{
    /**
     * @brief https://learn.microsoft.com/en-us/windows/win32/api/commdlg/ns-commdlg-openfilenamew
     */
    enum class FileDialogFlags : DWORD {
        /// The File Name list box allows multiple selections. If you also set the OFN_EXPLORER flag,
        /// the dialog box uses the Explorer-style user interface; otherwise, it uses the old-style
        /// user interface.
        /// If the user selects more than one file, the lpstrFile buffer returns the path to the
        /// current directory followed by the file names of the selected files. The nFileOffset member
        /// is the offset, in bytes or characters, to the first file name, and the nFileExtension
        /// member is not used. For Explorer-style dialog boxes, the directory and file name strings
        /// are NULL separated, with an extra NULL character after the last file name. This format
        /// enables the Explorer-style dialog boxes to return long file names that include spaces.
        /// For old-style dialog boxes, the directory and file name strings are separated by spaces
        /// and the function uses short file names for file names with spaces. You can use the
        /// FindFirstFile function to convert between long and short file names.
        /// If you specify a custom template for an old-style dialog box, the definition of the File
        /// Name list box must contain the LBS_EXTENDEDSEL value.
        AllowMultiSelect = 0x00000200,

        /// If the user specifies a file that does not exist, this flag causes the dialog box to
        /// prompt the user for permission to create the file. If the user chooses to create the
        /// file, the dialog box closes and the function returns the specified name; otherwise,
        /// the dialog box remains open. If you use this flag with the OFN_ALLOWMULTISELECT flag,
        /// the dialog box allows the user to specify only one nonexistent file.
        CreatePrompt = 0x00002000,

        /// Prevents the system from adding a link to the selected file in the file system directory
        /// that contains the user's most recently used documents. To retrieve the location of this
        /// directory, call the SHGetSpecialFolderLocation function with the CSIDL_RECENT flag.
        DontAddTorecent = 0x02000000,

        /// Enables the hook function specified in the lpfnHook member.
        EnableHook = 0x00000020,

        /// Causes the dialog box to send CDN_INCLUDEITEM notification messages to your OFNHookProc hook
        /// procedure when the user opens a folder. The dialog box sends a notification for each item in
        /// the newly opened folder. These messages enable you to control which items the dialog box
        /// displays in the folder's item list.
        EnableIncludeNotify = 0x00400000,

        /// Enables the Explorer-style dialog box to be resized using either the mouse or the keyboard.
        /// By default, the Explorer-style Open and Save As dialog boxes allow the dialog box to be resized
        /// regardless of whether this flag is set. This flag is necessary only if you provide a hook
        /// procedure or custom template. The old-style dialog box does not permit resizing.
        EnableSizing = 0x00800000,

        /// The lpTemplateName member is a pointer to the name of a dialog template resource in the module
        /// identified by the hInstance member. If the OFN_EXPLORER flag is set, the system uses the specified
        /// template to create a dialog box that is a child of the default Explorer-style dialog box. If the
        /// OFN_EXPLORER flag is not set, the system uses the template to create an old-style dialog box that
        /// replaces the default dialog box.
        EnableTemplate = 0x00000040,

        /// The hInstance member identifies a data block that contains a preloaded dialog box template.
        /// The system ignores lpTemplateName if this flag is specified. If the OFN_EXPLORER flag is set,
        /// the system uses the specified template to create a dialog box that is a child of the default
        /// Explorer-style dialog box. If the OFN_EXPLORER flag is not set, the system uses the template to
        /// create an old-style dialog box that replaces the default dialog box.
        EnableTemplateHandle = 0x00000080,

        /// Indicates that any customizations made to the Open or Save As dialog box use the Explorer-style
        /// customization methods. For more information, see Explorer-Style Hook Procedures and Explorer-Style
        /// Custom Templates.
        /// By default, the Open and Save As dialog boxes use the Explorer-style user interface regardless of
        /// whether this flag is set. This flag is necessary only if you provide a hook procedure or custom template,
        /// or set the OFN_ALLOWMULTISELECT flag.
        /// If you want the old-style user interface, omit the OFN_EXPLORER flag and provide a replacement old-style
        /// template or hook procedure. If you want the old style but do not need a custom template or hook procedure,
        /// simply provide a hook procedure that always returns FALSE.
        Explorer = 0x00080000,

        /// The user typed a file name extension that differs from the extension specified by lpstrDefExt.
        /// The function does not use this flag if lpstrDefExt is NULL.
        ExtensionDifferent = 0x00000400,

        /// The user can type only names of existing files in the File Name entry field. If this flag is specified and
        /// the user enters an invalid name, the dialog box procedure displays a warning in a message box. If this flag
        /// is specified, the OFN_PATHMUSTEXIST flag is also used. This flag can be used in an Open dialog box. It cannot
        /// be used with a Save As dialog box.
        FileMustExist = 0x00001000,

        /// Forces the showing of system and hidden files, thus overriding the user setting to show or not show hidden
        /// files. However, a file that is marked both system and hidden is not shown.
        ForceShowHidden = 0x10000000,

        /// Hides the Read Only check box.
        HideReadOnly = 0x00000004,

        /// For old-style dialog boxes, this flag causes the dialog box to use long file names. If this flag is not
        /// specified, or if the OFN_ALLOWMULTISELECT flag is also set, old-style dialog boxes use short file names
        /// (8.3 format) for file names with spaces. Explorer-style dialog boxes ignore this flag and always display
        /// long file names.
        LongNames = 0x00200000,

        /// Restores the current directory to its original value if the user changed the directory while searching for files.
        /// This flag is ineffective for GetOpenFileName.
        NoChangeDir = 0x00000008,

        /// Directs the dialog box to return the path and file name of the selected shortcut (.LNK) file. If this value
        /// is not specified, the dialog box returns the path and file name of the file referenced by the shortcut.
        NoDereferenceLinks = 0x00100000,

        /// For old-style dialog boxes, this flag causes the dialog box to use short file names (8.3 format). Explorer-style
        /// dialog boxes ignore this flag and always display long file names.
        NoLongNames = 0x00040000,

        /// Hides and disables the Network button.
        NoNetworkButton = 0x00020000,

        /// The returned file does not have the Read Only check box selected and is not in a write-protected directory.
        NoReadOnlyReturn = 0x00008000,

        /// The file is not created before the dialog box is closed. This flag should be specified if the application saves
        /// the file on a create-nonmodify network share. When an application specifies this flag, the library does not
        /// check for write protection, a full disk, an open drive door, or network protection. Applications using this flag
        /// must perform file operations carefully, because a file cannot be reopened once it is closed.
        NoTestFileCreate = 0x00010000,

        /// The common dialog boxes allow invalid characters in the returned file name. Typically, the calling application
        /// uses a hook procedure that checks the file name by using the FILEOKSTRING message. If the text box in the edit
        /// control is empty or contains nothing but spaces, the lists of files and directories are updated. If the text box
        /// in the edit control contains anything else, nFileOffset and nFileExtension are set to values generated by parsing
        /// the text. No default extension is added to the text, nor is text copied to the buffer specified by lpstrFileTitle.
        /// If the value specified by nFileOffset is less than zero, the file name is invalid. Otherwise, the file name is valid,
        /// and nFileExtension and nFileOffset can be used as if the OFN_NOVALIDATE flag had not been specified.
        NoValidate = 0x00000100,

        /// Causes the Save As dialog box to generate a message box if the selected file already exists. The user must confirm
        /// whether to overwrite the file.
        OverwritePrompt = 0x00000002,

        /// The user can type only valid paths and file names. If this flag is used and the user types an invalid path and
        /// file name in the File Name entry field, the dialog box function displays a warning in a message box.
        PathMustExist = 0x00000800,

        /// Causes the Read Only check box to be selected initially when the dialog box is created. This flag indicates the
        /// state of the Read Only check box when the dialog box is closed.
        ReadOnly = 0x00000001,

        /// Specifies that if a call to the OpenFile function fails because of a network sharing violation, the error is ignored
        /// and the dialog box returns the selected file name. If this flag is not set, the dialog box notifies your hook procedure
        /// when a network sharing violation occurs for the file name specified by the user. If you set the OFN_EXPLORER flag,
        /// the dialog box sends the CDN_SHAREVIOLATION message to the hook procedure. If you do not set OFN_EXPLORER, the dialog
        /// box sends the SHAREVISTRING registered message to the hook procedure.
        ShareAware = 0x00004000,

        /// Causes the dialog box to display the Help button. The hwndOwner member must specify the window to receive the
        /// HELPMSGSTRING registered messages that the dialog box sends when the user clicks the Help button. An Explorer-style
        /// dialog box sends a CDN_HELP notification message to your hook procedure when the user clicks the Help button.
        ShowHelp = 0x00000010,
    };

    /**
     * @brief 标记FileDialogFlags枚举支持位运算
     */
    _SW_ENUM_ENABLE_BIT_OPERATIONS(FileDialogFlags);

    /**
     * @brief 文件筛选器信息
     */
    struct FileFilterItem {
        /**
         * @brief 文本
         */
        std::wstring name;

        /**
         * @brief 筛选器字符串，有多个类型时用分号分隔
         */
        std::wstring filter;

        /**
         * @brief 默认扩展名，当SaveFileDialog用户没有填写扩展名时会使用该值作为扩展名
         */
        std::wstring defaultExt;
    };

    /**
     * @brief “打开文件”对话框与“另存为”对话框的基类
     */
    class FileDialog : public IDialog
    {
    private:
        /**
         * @brief OPENFILENAMEW结构体
         */
        OPENFILENAMEW _ofn{};

        /**
         * @brief 储存文件名的缓冲区
         */
        std::vector<wchar_t> _buffer;

        /**
         * @brief 对话框标题
         */
        std::wstring _title;

        /**
         * @brief 初始目录
         */
        std::wstring _initialDir;

        /**
         * @brief 传给OPENFILENAMEW的筛选器字符串
         * @note 格式如："文本1\0筛选器字符串1\0文本2\0筛选器字符串2\0\0"，最后以两个空字符结尾
         */
        std::vector<wchar_t> _filterBuffer;

        /**
         * @brief 筛选器集合
         */
        ObservableCollection<FileFilterItem> _filters;

    public:
        /**
         * @brief 储存文件名的缓冲区大小，值不能小于MAX_PATH
         */
        const Property<int> BufferSize;

        /**
         * @brief 对话框标志
         */
        const Property<FileDialogFlags> Flags;

        /**
         * @brief 对话框标题，设为空字符串可显示默认标题
         */
        const Property<std::wstring> Title;

        /**
         * @brief 初始目录
         */
        const Property<std::wstring> InitialDir;

        /**
         * @brief 筛选器集合
         */
        const ReadOnlyProperty<ObservableCollection<FileFilterItem> *> Filters;

        /**
         * @brief 当前筛选器的索引，索引值从0开始
         */
        const Property<int> FilterIndex;

        /**
         * @brief 选中文件的路径
         */
        const ReadOnlyProperty<std::wstring> FileName;

        /**
         * @brief 是否允许多选
         */
        const Property<bool> MultiSelect;

        /**
         * @brief 所有选中的文件路径
         */
        const ReadOnlyProperty<List<std::wstring>> FileNames;

    public:
        /**
         * @brief 初始化FileDialog
         */
        FileDialog();

        /**
         * @brief FileDialog默认不支持该函数，调用该函数不会执行任何操作
         */
        virtual void Close() override;

        /**
         * @brief FileDialog默认不支持该函数，调用该函数不会执行任何操作
         */
        virtual void Show() override;

        /**
         * @brief 显示对话框，并指定所有者窗口
         * @return 若用户选择了文件则返回true，否则返回false
         */
        virtual int ShowDialog(Window *owner = nullptr) override = 0;

        /**
         * @brief 显示对话框，并指定所有者窗口
         * @return 若用户选择了文件则返回true，否则返回false
         */
        virtual int ShowDialog(Window &owner) = 0;

    protected:
        /**
         * @brief 获取OPENFILENAMEW指针
         */
        OPENFILENAMEW *GetOFN();

        /**
         * @brief 获取指向缓冲区的指针
         */
        wchar_t *GetBuffer();

        /**
         * @brief 清空缓冲区，显示对话框前必须调用此函数
         */
        void ClearBuffer();

        /**
         * @brief 处理文件路径，获取文件路径时会先调用这个函数对返回值进行处理
         * @param fileName 获取到的文件路径，可通过修改该值改变FileName和FileNames属性获取到的内容
         */
        virtual void ProcessFileName(std::wstring &fileName);

    private:
        /**
         * @brief 添加一个筛选器到缓冲区
         * @param filter 要添加的筛选器
         */
        void _AppendFilterToBuffer(const FileFilterItem &filter);

        /**
         * @brief 重置筛选器缓冲区
         */
        void _ResetFilterBuffer();

        /**
         * @brief Filters集合变更事件处理函数
         * @param sender 事件的发送者
         * @param args 包含集合变更信息的事件参数
         */
        void _FiltersCollectionChangedHandler(
            INotifyCollectionChanged &sender, NotifyCollectionChangedEventArgs &args);
    };

    /**
     * @brief “打开文件”对话框
     */
    class OpenFileDialog : public FileDialog
    {
    public:
        /**
         * @brief 初始化OpenFileDialog
         */
        OpenFileDialog();

        /**
         * @brief 显示对话框，并指定所有者窗口
         * @return 若用户选择了文件则返回true，否则返回false
         */
        virtual int ShowDialog(Window *owner = nullptr) override;

        /**
         * @brief 显示对话框，并指定所有者窗口
         * @return 若用户选择了文件则返回true，否则返回false
         */
        virtual int ShowDialog(Window &owner) override;
    };

    /**
     * @brief “另存为”对话框
     */
    class SaveFileDialog : public FileDialog
    {
    private:
        /**
         * @brief 初始文件名
         */
        std::wstring _initialFileName;

    public:
        /**
         * @brief 初始文件名
         */
        const Property<std::wstring> InitialFileName;

        /**
         * @brief 初始化SaveFileDialog
         */
        SaveFileDialog();

        /**
         * @brief 显示对话框，并指定所有者窗口
         * @return 若用户选择了文件则返回true，否则返回false
         */
        virtual int ShowDialog(Window *owner = nullptr) override;

        /**
         * @brief 显示对话框，并指定所有者窗口
         * @return 若用户选择了文件则返回true，否则返回false
         */
        virtual int ShowDialog(Window &owner) override;

    protected:
        /**
         * @brief 处理文件路径，获取文件路径时会先调用这个函数对返回值进行处理
         * @param fileName 获取到的文件路径，可通过修改该值改变FileName和FileNames属性获取到的内容
         */
        virtual void ProcessFileName(std::wstring &fileName) override;

    private:
        /**
         * @brief 设置初始文件名到缓冲区
         */
        void _SetInitialFileName();
    };
}

// FrameworkElement.h


namespace sw
{
    // 前向声明
    class DataBinding;
    class FrameworkElement;

    /**
     * @brief 数据上下文更改事件参数
     */
    struct DataContextChangedEventArgs : EventArgs {
        /**
         * @brief 旧的数据上下文值
         */
        DynamicObject *oldDataContext;
    };

    /**
     * @brief 数据上下文更改事件处理函数类型
     */
    using DataContextChangedEventHandler =
        EventHandler<FrameworkElement, DataContextChangedEventArgs>;

    /**
     * @brief 框架元素类，提供数据上下文和绑定功能
     */
    class FrameworkElement : public ObservableObject,
                             public ITag<Variant>
    {
    private:
        /**
         * @brief 属性的绑定信息
         */
        std::unordered_map<FieldId, std::unique_ptr<BindingBase>> _bindings{};

        /**
         * @brief 用户自定义数据标签
         */
        Variant _tag = nullptr;

        /**
         * @brief 数据上下文
         */
        Variant _dataContext = nullptr;

        /**
         * @brief 数据上下文改变事件委托
         */
        DataContextChangedEventHandler _dataContextChanged;

    public:
        /**
         * @brief 数据上下文改变时触发该事件
         */
        const Event<DataContextChangedEventHandler> DataContextChanged;

        /**
         * @brief 自定义数据标签，可用于存储任意用户数据
         */
        const Property<Variant> Tag;

        /**
         * @brief 数据上下文
         * @note 若需以引用语义持有外部对象，请使用Variant::MakeRef构造，
         *       而非传入DynamicObject*裸指针 —— 后者会被Variant按值装箱为
         *       BoxedObject<DynamicObject*>，导致绑定无法解析到原对象。
         */
        const Property<Variant> DataContext;

        /**
         * @brief 当前元素的有效数据上下文
         * @note 若当前元素的DataContext不为nullptr则返回该值，否则递归获取父元素的DataContext
         */
        const ReadOnlyProperty<DynamicObject *> CurrentDataContext;

    protected:
        /**
         * @brief 初始化FrameworkElement
         */
        FrameworkElement();

        // 删除拷贝构造函数
        FrameworkElement(const FrameworkElement &) = delete;

        // 删除移动构造函数
        FrameworkElement(FrameworkElement &&) = delete;

        // 删除拷贝赋值运算符
        FrameworkElement &operator=(const FrameworkElement &) = delete;

        // 删除移动赋值运算符
        FrameworkElement &operator=(FrameworkElement &&) = delete;

    public:
        /**
         * @brief 添加绑定对象
         * @return 若函数成功则返回true，否则返回false
         * @note 绑定对象的生命周期将由当前元素管理，请勿与其他对象共享
         * @note 请确保绑定对象的目标属性为当前元素的属性，该函数内部不会对此进行检查
         * @note 同一个属性只能设置一个绑定，若该属性已存在绑定则会被新的绑定覆盖
         */
        bool AddBinding(BindingBase *binding);

        /**
         * @brief 添加绑定对象
         * @return 若函数成功则返回true，否则返回false
         * @note 绑定对象的生命周期将由当前元素管理，请勿与其他对象共享
         * @note 该函数会将绑定的目标对象设置为当前元素，若未指定源对象则会将DataContext作为源对象
         * @note 同一个属性只能设置一个绑定，若该属性已存在绑定则会被新的绑定覆盖
         */
        bool AddBinding(Binding *binding);

        /**
         * @brief 添加自绑定对象
         * @return 若函数成功则返回true，否则返回false
         * @note 绑定对象的生命周期将由当前元素管理，请勿与其他对象共享
         * @note 同一个属性只能设置一个绑定，若该属性已存在绑定则会被新的绑定覆盖
         */
        bool AddBinding(SelfBinding *binding);

        /**
         * @brief 添加绑定到DataContext的绑定对象
         * @return 若函数成功则返回true，否则返回false
         * @note 绑定对象的生命周期将由当前元素管理，请勿与其他对象共享
         * @note 同一个属性只能设置一个绑定，若该属性已存在绑定则会被新的绑定覆盖
         */
        bool AddBinding(DataBinding *binding);

        /**
         * @brief 移除指定属性的绑定对象
         * @return 若函数成功则返回true，否则返回false
         */
        bool RemoveBinding(FieldId propertyId);

        /**
         * @brief 移除指定属性的绑定对象
         * @return 若函数成功则返回true，否则返回false
         */
        template <typename T, typename TProperty>
        bool RemoveBinding(TProperty T::*prop)
        { return RemoveBinding(Reflection::GetFieldId(prop)); }

    public:
        /**
         * @brief 获取Tag
         */
        virtual Variant GetTag() const override final;

        /**
         * @brief 设置Tag
         */
        virtual void SetTag(const Variant &tag) override final;

    protected:
        /**
         * @brief 当Tag更改时调用此函数
         */
        virtual void OnTagChanged();

        /**
         * @brief 当CurrentDataContext更改时调用此函数
         * @param oldDataContext 旧的数据上下文值
         */
        virtual void OnCurrentDataContextChanged(DynamicObject *oldDataContext);

    public:
        /**
         * @brief 获取逻辑树中的父元素
         * @return 父元素指针，如果没有父元素则返回nullptr
         */
        virtual FrameworkElement *GetParent() const = 0;

        /**
         * @brief 获取逻辑树中的子元素数量
         * @return 子元素数量
         */
        virtual int GetChildCount() const = 0;

        /**
         * @brief 获取逻辑树中指定索引处的子元素
         * @param index 子元素索引
         * @throw std::out_of_range 如果索引超出范围
         */
        virtual FrameworkElement &GetChildAt(int index) const = 0;
    };
}

// GridLayout.h


namespace sw
{
    /**
     * @brief 网格布局方式的布局标记
     */
    struct GridLayoutTag {
        /// @brief 所在行
        uint16_t row = 0;

        /// @brief 所在列
        uint16_t column = 0;

        /// @brief 所跨行数
        uint16_t rowSpan = 1;

        /// @brief 所跨列数
        uint16_t columnSpan = 1;

        /// @brief GridLayoutTag默认值
        GridLayoutTag() = default;

        /// @brief 初始化GridLayoutTag
        GridLayoutTag(
            uint16_t row, uint16_t column, uint16_t rowSpan = 1, uint16_t columnSpan = 1);

        /// @brief 从LayoutTag创建
        GridLayoutTag(uint64_t layoutTag);

        /// @brief 隐式转换LayoutTag
        operator uint64_t() const noexcept;
    };

    /**
     * @brief GridRow和GridColumn的类型
     */
    enum class GridRCType {
        FixSize,    ///< 固定大小
        AutoSize,   ///< 自动大小
        FillRemain, ///< 填充剩余空间
    };

    /**
     * @brief 网格中的行信息
     */
    struct GridRow {
        /// @brief 类型
        GridRCType type = GridRCType::FillRemain;

        /// @brief 高度
        double height = 1;

        /// @brief 创建一个FillRemain的GridRow
        GridRow() = default;

        /// @brief 固定大小的行
        GridRow(double height);

        /// @brief 初始化GridRow
        GridRow(GridRCType type, double height);
    };

    /**
     * @brief 固定高度的行
     */
    struct FixSizeGridRow : public GridRow {
        /// @brief 初始化FixSizeGridRow
        FixSizeGridRow(double height);
    };

    /**
     * @brief 自动高度的行
     */
    struct AutoSizeGridRow : public GridRow {
        /// @brief 初始化AutoSizeGridRow
        AutoSizeGridRow();
    };

    /**
     * @brief 填充剩余高度的行
     */
    struct FillRemainGridRow : public GridRow {
        /// @brief 初始化FillRemainGridRow
        FillRemainGridRow(double proportion = 1);
    };

    /**
     * @brief 网格中的列信息
     */
    struct GridColumn {
        /// @brief 类型
        GridRCType type = GridRCType::FillRemain;

        /// @brief 宽度
        double width = 1;

        /// @brief 创建一个FillRemain的GridColumn
        GridColumn() = default;

        /// @brief 固定大小的列
        GridColumn(double width);

        /// @brief 初始化GridColumn
        GridColumn(GridRCType type, double width);
    };

    /**
     * @brief 固定宽度的列
     */
    struct FixSizeGridColumn : public GridColumn {
        /// @brief 初始化FixSizeGridColumn
        FixSizeGridColumn(double width);
    };

    /**
     * @brief 自动宽度的列
     */
    struct AutoSizeGridColumn : public GridColumn {
        /// @brief 初始化AutoSizeGridColumn
        AutoSizeGridColumn();
    };

    /**
     * @brief 填充剩余宽度的列
     */
    struct FillRemainGridColumn : public GridColumn {
        /// @brief 初始化FillRemainGridColumn
        FillRemainGridColumn(double proportion = 1);
    };

    /**
     * @brief 网格布局方式
     */
    class GridLayout : public LayoutHost
    {
    private:
        /**
         * @brief 子元素的信息
         */
        struct _ChildInfo {
            ILayout *instance;         ///< 子元素对象
            GridLayoutTag layoutTag;   ///< 布局标记
            GridRCType rowMeasureType; ///< 元素measure行时的类型
            GridRCType colMeasureType; ///< 元素measure列时的类型
        };

        /**
         * @brief 行信息
         */
        struct _RowInfo {
            GridRow row;           ///< 行
            double size       = 0; ///< 所需空间大小
            double proportion = 0; ///< 类型为FillRemain时该字段保存该行的高度占比，范围为0~1
        };

        /**
         * @brief 列信息
         */
        struct _ColInfo {
            GridColumn col;        ///< 列
            double size       = 0; ///< 所需空间大小
            double proportion = 0; ///< 类型为FillRemain时该字段保存该列的宽度占比，范围为0~1
        };

        /**
         * @brief 一些内部数据
         */
        struct {
            std::vector<_RowInfo> rowsInfo;       ///< 行信息
            std::vector<_ColInfo> colsInfo;       ///< 列信息
            std::vector<_ChildInfo> childrenInfo; ///< 子元素信息
            std::vector<Rect> cells;              ///< 保存格信息
        } _internalData;

    public:
        /**
         * @brief 行定义
         */
        ObservableCollection<GridRow> rows;

        /**
         * @brief 列定义
         */
        ObservableCollection<GridColumn> columns;

        /**
         * @brief 测量元素所需尺寸，无需考虑边框和边距
         * @param availableSize 可用的尺寸
         * @return 返回元素需要占用的尺寸
         */
        virtual Size MeasureOverride(const Size &availableSize) override;

        /**
         * @brief 安排子元素的位置，可重写该函数以实现自定义布局
         * @param finalSize 可用于排列子元素的最终尺寸
         */
        virtual void ArrangeOverride(const Size &finalSize) override;

    private:
        /**
         * @brief 更新内部数据
         */
        void _UpdateInternalData();

        /**
         * @brief 获取指定行列处的网格信息
         */
        Rect &_GetCell(int row, int col);
    };
}

// DataBinding.h


namespace sw
{
    /**
     * @brief 数据绑定，用于UI元素与DataContext之间的属性绑定
     */
    class DataBinding final : public BindingBase
    {
    private:
        /**
         * @brief 目标元素
         */
        FrameworkElement *_targetElement;

        /**
         * @brief 内部绑定对象
         */
        std::unique_ptr<Binding> _innerBinding;

    private:
        /**
         * @brief 构造函数
         * @param targetElement 目标元素
         * @param binding 内部绑定对象
         */
        DataBinding(FrameworkElement *targetElement, Binding *binding)
            : _targetElement(targetElement), _innerBinding(binding)
        {
            UpdateDataContextBinding();
            RegisterNotifications();
        }

    public:
        /**
         * @brief 析构函数
         */
        virtual ~DataBinding()
        {
            UnregisterNotifications();
        }

        /**
         * @brief 更新目标属性的值
         * @return 如果更新成功则返回true，否则返回false
         */
        virtual bool UpdateTarget() override
        {
            return _innerBinding->UpdateTarget();
        }

        /**
         * @brief 更新源属性的值
         * @return 如果更新成功则返回true，否则返回false
         */
        virtual bool UpdateSource() override
        {
            return _innerBinding->UpdateSource();
        }

        /**
         * @brief 获取目标属性ID
         */
        virtual FieldId GetTargetPropertyId() const override
        {
            return _innerBinding->GetTargetPropertyId();
        }

        /**
         * @brief 获取源属性ID
         */
        virtual FieldId GetSourcePropertyId() const override
        {
            return _innerBinding->GetSourcePropertyId();
        }

        /**
         * @brief 获取绑定模式
         */
        BindingMode GetBindingMode() const
        {
            return _innerBinding->GetBindingMode();
        }

        /**
         * @brief 设置绑定模式
         */
        void SetBindingMode(BindingMode mode)
        {
            _innerBinding->SetBindingMode(mode);
        }

        /**
         * @brief 获取目标元素
         * @return 目标元素指针
         */
        FrameworkElement *GetTargetObject() const
        {
            return _targetElement;
        }

        /**
         * @brief 设置目标元素
         * @param element 目标元素指针
         */
        void SetTargetObject(FrameworkElement *element)
        {
            if (_targetElement != element) {
                UnregisterNotifications();
                _targetElement = element;
                UpdateDataContextBinding();
                RegisterNotifications();
            }
        }

    private:
        /**
         * @brief 注册事件通知
         */
        void RegisterNotifications()
        {
            if (_targetElement == nullptr) {
                return;
            }

            _targetElement->ObjectDead +=
                ObjectDeadEventHandler(*this, &DataBinding::OnTargetElementDead);

            _targetElement->DataContextChanged +=
                DataContextChangedEventHandler(*this, &DataBinding::OnTargetElementDataContextChanged);
        }

        /**
         * @brief 注销事件通知
         */
        void UnregisterNotifications()
        {
            if (_targetElement == nullptr) {
                return;
            }

            _targetElement->ObjectDead -=
                ObjectDeadEventHandler(*this, &DataBinding::OnTargetElementDead);

            _targetElement->DataContextChanged -=
                DataContextChangedEventHandler(*this, &DataBinding::OnTargetElementDataContextChanged);
        }

        /**
         * @brief 更新数据上下文绑定
         */
        void UpdateDataContextBinding()
        {
            if (_targetElement == nullptr) {
                _innerBinding->SetBindingObjects(nullptr, nullptr);
            } else {
                _innerBinding->SetBindingObjects(_targetElement, _targetElement->CurrentDataContext);
            }
        }

        /**
         * @brief 目标元素销毁事件处理函数
         */
        void OnTargetElementDead(INotifyObjectDead &sender, EventArgs &e)
        {
            SetTargetObject(nullptr);
        }

        /**
         * @brief 目标元素数据上下文更改事件处理函数
         */
        void OnTargetElementDataContextChanged(FrameworkElement &sender, DataContextChangedEventArgs &e)
        {
            UpdateDataContextBinding();
        }

    public:
        /**
         * @brief 创建数据绑定对象
         * @param targetElement 目标元素
         * @param binding 内部绑定对象
         * @return 绑定对象指针
         * @note 绑定对象不能为nullptr，其生命周期将由DataBinding管理，请勿与其他对象共享
         */
        static DataBinding *Create(FrameworkElement *targetElement, Binding *binding)
        {
            assert(binding != nullptr);
            return new DataBinding(targetElement, binding);
        }

        /**
         * @brief 创建数据绑定对象
         * @param targetProperty 目标属性成员指针
         * @param sourceProperty 源属性成员指针
         * @param mode 绑定模式
         * @param converter 值转换器指针
         * @return 绑定对象指针
         * @note 转换器的生命周期将由绑定对象管理，请勿与其他对象共享
         */
        template <
            typename TTargetObject,
            typename TTargetProperty,
            typename TSourceObject,
            typename TSourceProperty>
        static auto Create(TTargetProperty TTargetObject::*targetProperty,
                           TSourceProperty TSourceObject::*sourceProperty,
                           BindingMode mode,
                           IValueConverter<typename TSourceProperty::TValue, typename TTargetProperty::TValue> *converter = nullptr)
            -> typename std::enable_if<
                _IsProperty<TTargetProperty>::value &&
                    _IsProperty<TSourceProperty>::value &&
                    std::is_base_of<DynamicObject, TTargetObject>::value &&
                    std::is_base_of<DynamicObject, TSourceObject>::value &&
                    BindingCastHelper<typename TSourceProperty::TValue, typename TTargetProperty::TValue>::value,
                DataBinding *>::type
        {
            return new DataBinding(nullptr, Binding::Create(targetProperty, sourceProperty, mode, converter));
        }

        /**
         * @brief 创建数据绑定对象
         * @param targetProperty 目标属性成员指针
         * @param sourceProperty 源属性成员指针
         * @param mode 绑定模式
         * @param converter 值转换器指针
         * @return 绑定对象指针
         * @note 转换器的生命周期将由绑定对象管理，请勿与其他对象共享
         */
        template <
            typename TTargetObject,
            typename TTargetProperty,
            typename TSourceObject,
            typename TSourceProperty>
        static auto Create(TTargetProperty TTargetObject::*targetProperty,
                           TSourceProperty TSourceObject::*sourceProperty,
                           BindingMode mode,
                           IValueConverter<typename TSourceProperty::TValue, typename TTargetProperty::TValue> *converter)
            -> typename std::enable_if<
                _IsProperty<TTargetProperty>::value &&
                    _IsProperty<TSourceProperty>::value &&
                    std::is_base_of<DynamicObject, TTargetObject>::value &&
                    std::is_base_of<DynamicObject, TSourceObject>::value &&
                    !BindingCastHelper<typename TSourceProperty::TValue, typename TTargetProperty::TValue>::value,
                DataBinding *>::type
        {
            return new DataBinding(nullptr, Binding::Create(targetProperty, sourceProperty, mode, converter));
        }
    };
}

// MenuItem.h


namespace sw
{
    /**
     * @brief 菜单项描述结构体
     */
    struct MenuItemDesc //
    {
        /// @brief 菜单项标记，可用于区分不同菜单项
        uint64_t tag = 0;

        /// @brief 菜单项文本，若为"-"且没有子项则被视为分隔符
        std::wstring text{};

        /// @brief 菜单项是否可用，默认为true
        bool enabled = true;

        /// @brief 菜单项是否选中，默认为false
        bool checked = false;

        /// @brief 菜单项位图句柄，默认为NULL
        HBITMAP hBitmap = NULL;

        /// @brief 子菜单项列表
        List<MenuItemDesc> subItems{};

        /// @brief 默认构造函数
        MenuItemDesc() = default;

        /// @brief 构造指定文本的菜单项描述
        /// @param text 菜单项文本
        MenuItemDesc(const wchar_t *text);

        /// @brief 构造指定文本的菜单项描述
        /// @param text 菜单项文本
        MenuItemDesc(const std::wstring &text);

        /// @brief 构造指定文本和子项的菜单项描述
        /// @param text 菜单项文本
        /// @param subItems 子菜单项列表
        MenuItemDesc(const std::wstring &text,
                     std::initializer_list<MenuItemDesc> subItems);

        /// @brief 构造指定标记和文本的菜单项描述
        /// @param tag 菜单项标记
        /// @param text 菜单项文本
        MenuItemDesc(uint64_t tag, const std::wstring &text);

        /// @brief 构造指定标记、文本和子项的菜单项描述
        /// @param tag 菜单项标记
        /// @param text 菜单项文本
        /// @param subItems 子菜单项列表
        MenuItemDesc(uint64_t tag, const std::wstring &text,
                     std::initializer_list<MenuItemDesc> subItems);
    };

    /**
     * @brief 菜单项类
     */
    class MenuItem : public FrameworkElement
    {
    private:
        /**
         * @brief 基类别名，方便调用基类函数
         */
        using TBase = FrameworkElement;

        /**
         * @brief 菜单项描述信息
         * @note 该字段仅用于保存当前节点的状态，_desc.subItems始终为空。
         */
        MenuItemDesc _desc{};

        /**
         * @brief 菜单项句柄
         */
        HMENU _hMenu = NULL;

        /**
         * @brief 菜单项ID
         */
        int _id = -1;

        /**
         * @brief 是否为根菜单项
         */
        bool _isRoot = false;

        /**
         * @brief 父菜单项指针
         */
        MenuItem *_parent = nullptr;

        /**
         * @brief 子菜单项列表
         */
        std::vector<std::unique_ptr<MenuItem>> _subItems{};

    public:
        /**
         * @brief 菜单项Id
         */
        const ReadOnlyProperty<int> Id;

        /**
         * @brief 菜单项句柄
         * @note 若当前菜单项不是根菜单项，菜单项句柄可能会随着内容的更改而改变，
         *       因此不建议缓存该属性值。
         */
        const ReadOnlyProperty<HMENU> Handle;

        /**
         * @brief 菜单项文本
         * @note 若菜单项文本为"-"且没有子项则被视为分隔符，此时IsSeparator属性将返回true。
         */
        const Property<std::wstring> Text;

        /**
         * @brief 菜单项是否可用
         */
        const Property<bool> Enabled;

        /**
         * @brief 菜单项是否选中
         */
        const Property<bool> IsChecked;

        /**
         * @brief 菜单项位图
         */
        const Property<HBITMAP> Bitmap;

        /**
         * @brief 菜单项是否为分隔符
         */
        const ReadOnlyProperty<bool> IsSeparator;

        /**
         * @brief 菜单项Tag
         */
        const Property<uint64_t> Tag;

    private:
        /**
         * @brief 构造函数，使用菜单项描述信息初始化菜单项
         * @param desc 菜单项描述信息
         */
        MenuItem(const MenuItemDesc &desc);

    public:
        /**
         * @brief 析构函数
         */
        virtual ~MenuItem();

        /**
         * @brief 创建一个新的菜单项实例
         * @param desc 菜单项描述信息
         */
        static MenuItem *Create(const MenuItemDesc &desc);

        /**
         * @brief 创建一个新的根菜单项实例
         * @return 新创建的根菜单项实例
         */
        static MenuItem *CreateRoot(bool isPopup);

    protected:
        /**
         * @brief 当Tag更改时调用此函数
         */
        virtual void OnTagChanged() override;

    public:
        /**
         * @brief 获取逻辑树中的父元素
         * @return 父元素指针，如果没有父元素则返回nullptr
         */
        virtual MenuItem *GetParent() const override final;

        /**
         * @brief 获取逻辑树中的子元素数量
         * @return 子元素数量
         */
        virtual int GetChildCount() const override final;

        /**
         * @brief 获取逻辑树中指定索引处的子元素
         * @param index 子元素索引
         * @throw std::out_of_range 如果索引超出范围
         */
        virtual MenuItem &GetChildAt(int index) const override final;

        /**
         * @brief 向当前菜单项添加一个子菜单项
         * @param desc 子菜单项的描述信息
         * @return 新增的子菜单项实例
         */
        MenuItem *AddChild(const MenuItemDesc &desc);

        /**
         * @brief 向当前菜单项的指定索引处插入一个子菜单项
         * @param index 插入位置的子菜单项索引
         * @param desc 子菜单项的描述信息
         * @return 新增的子菜单项实例
         * @throw std::out_of_range 如果索引超出范围
         */
        MenuItem *InsertChild(int index, const MenuItemDesc &desc);

        /**
         * @brief 移除指定索引处的子菜单项
         * @param index 子菜单项索引
         * @return 若函数成功则返回true，否则返回false
         */
        bool RemoveChildAt(int index);

        /**
         * @brief 移除指定子菜单项
         * @param child 子菜单项指针
         * @return 若函数成功则返回true，否则返回false
         */
        bool RemoveChild(MenuItem *child);

        /**
         * @brief 移除所有子菜单项
         */
        void ClearChildren();

        /**
         * @brief 获取指定子菜单项在当前菜单项中的索引
         * @param child 子菜单项指针
         * @return 子菜单项索引，如果未找到则返回-1
         */
        int IndexOf(MenuItem *child) const;

        /**
         * @brief 重置子菜单项列表
         * @param descs 新的子菜单项描述信息列表
         */
        void ResetChildren(std::initializer_list<MenuItemDesc> descs);

        /**
         * @brief 查找对应ID的子菜单项
         * @param id 子菜单项ID
         * @return 指向子菜单项的指针，如果未找到则返回nullptr
         */
        MenuItem *FindChildById(int id);

        /**
         * @brief 查找对应tag的子菜单项
         * @param tag 子菜单项tag
         * @return 指向子菜单项的指针，如果未找到则返回nullptr
         */
        MenuItem *FindChildByTag(uint64_t tag);

        /**
         * @brief 获取菜单项描述信息的副本
         * @return 菜单项描述信息副本
         */
        MenuItemDesc CopyDescTree() const;

    private:
        /**
         * @brief 重建菜单项
         */
        void _ResetMenuItem();

        /**
         * @brief 更新菜单项状态
         */
        void _UpdateState();

        /**
         * @brief 插入子菜单到指定父菜单项的指定位置
         * @param parent 父菜单项指针
         * @param child 子菜单项指针
         * @param index 子菜单项索引，默认为-1表示添加到末尾
         */
        static void _SetParent(MenuItem *parent, MenuItem *child, int index = -1);

        /**
         * @brief 生成一个新的菜单项ID
         * @return 新的菜单项ID
         */
        static int _GenerateMenuItemID();

        /**
         * @brief 根据菜单项描述信息填充MENUITEMINFOW结构体
         * @param desc 菜单项描述信息
         * @param pMii 待填充的MENUITEMINFOW结构体指针
         */
        static void _ApplyMenuDesc(const MenuItemDesc &desc, MENUITEMINFOW *pMii);

        /**
         * @brief 根据菜单项描述信息和菜单句柄应用菜单项属性
         * @param hParentMenu 菜单项所在菜单的句柄
         * @param index 菜单项在父菜单中的索引
         * @param desc 菜单项描述信息
         */
        static void _UpdateMenuItem(HMENU hParentMenu, int index, const MenuItemDesc &desc);
    };
}

// WndBase.h


namespace sw
{
    class UIElement; // UIElement.h

    /**
     * @brief 表示一个Windows窗口，是所有窗口和控件的基类
     */
    class WndBase : public FrameworkElement,
                    public IToString<WndBase>
    {
        // 部分控件可能会改变HWND，设为友元类向Control类暴露_hwnd字段
        friend class Control;

        // HwndWrapper不使用InitWindow或InitControl初始化句柄，向其暴露底层细节以便实现相关功能
        friend class HwndWrapper;

        /**
         * @brief 当前线程中等待CBT钩子绑定HWND的WndBase实例
         *
         * Init* / ResetHandle 在调用 CreateWindowExW 之前把待绑定的实例
         * 写入此变量，CBT 钩子在 HCBT_CREATEWND 时取出并将 HWND 与实例
         * 关联，之后立即清空。
         */
        static thread_local WndBase *_pendingInit;

        /**
         * @brief 当前线程中待自卸的CBT钩子句柄
         *
         * 与 _pendingInit 配对：调用方装钩子后写入，CBT 回调完成绑定时一并
         * 取出并立即调用 UnhookWindowsHookEx 把自己摘掉，避免嵌套创建时
         * 调用栈上累积多个钩子。
         */
        static thread_local HHOOK _pendingHook;

    private:
        /**
         * @brief 用于判断给定指针是否为指向WndBase的指针
         */
        const uint32_t _check;

        /**
         * @brief 窗口句柄
         */
        HWND _hwnd = NULL;

        /**
         * @brief 字体句柄
         */
        HFONT _hfont = NULL;

        /**
         * @brief 字体信息
         */
        sw::Font _font;

        /**
         * @brief 窗口的位置和尺寸
         */
        sw::Rect _rect{};

        /**
         * @brief 窗口标题或文本
         */
        std::wstring _text{};

        /**
         * @brief 窗口是否拥有焦点
         */
        bool _focused = false;

        /**
         * @brief 窗口是否已销毁
         */
        bool _isDestroyed = false;

        /**
         * @brief 当前对象是否是控件
         */
        bool _isControl = false;

        /**
         * @brief 窗口句柄原本的WndProc
         */
        WNDPROC _originalWndProc = NULL;

    public:
        /**
         * @brief 窗口句柄
         */
        const ReadOnlyProperty<HWND> Handle;

        /**
         * @brief 字体
         */
        const Property<sw::Font> Font;

        /**
         * @brief 字体名称
         */
        const Property<std::wstring> FontName;

        /**
         * @brief 字体大小
         */
        const Property<double> FontSize;

        /**
         * @brief 字体粗细
         */
        const Property<sw::FontWeight> FontWeight;

        /**
         * @brief 位置和尺寸
         */
        const Property<sw::Rect> Rect;

        /**
         * @brief 左边
         */
        const Property<double> Left;

        /**
         * @brief 顶边
         */
        const Property<double> Top;

        /**
         * @brief 宽度
         */
        const Property<double> Width;

        /**
         * @brief 高度
         */
        const Property<double> Height;

        /**
         * @brief 用户区尺寸
         */
        const ReadOnlyProperty<sw::Rect> ClientRect;

        /**
         * @brief 用户区宽度
         */
        const ReadOnlyProperty<double> ClientWidth;

        /**
         * @brief 用户区高度
         */
        const ReadOnlyProperty<double> ClientHeight;

        /**
         * @brief 窗口或控件是否可用
         */
        const Property<bool> Enabled;

        /**
         * @brief 窗口或控件是否可见
         */
        const Property<bool> Visible;

        /**
         * @brief 窗口标题或控件文本
         */
        const Property<std::wstring> Text;

        /**
         * @brief 窗口是否拥有焦点
         */
        const Property<bool> Focused;

        /**
         * @brief 是否已销毁，当该值为true时不应该继续使用当前对象
         */
        const ReadOnlyProperty<bool> IsDestroyed;

        /**
         * @brief 是否接受拖放文件
         */
        const Property<bool> AcceptFiles;

        /**
         * @brief 当前对象是否是控件
         */
        const ReadOnlyProperty<bool> IsControl;

        /**
         * @brief 窗口类名
         */
        const ReadOnlyProperty<std::wstring> ClassName;

        /**
         * @brief 窗口是否为一组控件中的第一个控件
         * @note 当窗口拥有WS_GROUP样式时该属性值为true，否则为false
         */
        const Property<bool> IsGroupStart;

        /**
         * @brief 鼠标是否被当前窗口捕获
         */
        const ReadOnlyProperty<bool> IsMouseCaptured;

    protected:
        /**
         * @brief 初始化WndBase
         */
        WndBase();

    public:
        /**
         * @brief 析构函数，这里用纯虚函数使该类成为抽象类
         */
        virtual ~WndBase() = 0;

        /**
         * @brief 尝试将对象转换成UIElement
         * @return 若函数成功则返回UIElement指针，否则返回nullptr
         */
        virtual UIElement *ToUIElement();

        /**
         * @brief 获取当前对象的描述字符串
         */
        virtual std::wstring ToString() const;

        /**
         * @brief 获取逻辑树中的父元素
         * @return 始终返回nullptr
         * @note WndBase不管理父子关系；若需访问Win32 HWND父窗口请使用GetParentWnd
         */
        virtual WndBase *GetParent() const override;

        /**
         * @brief 获取逻辑树中的子元素数量
         * @return 始终返回0
         * @note WndBase不管理父子关系
         */
        virtual int GetChildCount() const override;

        /**
         * @brief 获取逻辑树中指定索引处的子元素
         * @param index 子元素索引
         * @throw std::out_of_range 始终抛出
         * @note WndBase不管理父子关系
         */
        virtual WndBase &GetChildAt(int index) const override;

    protected:
        /**
         * @brief 初始化为窗口，该函数会调用CreateWindowExW
         * @return 若函数成功则返回true，否则返回false
         */
        bool InitWindow(LPCWSTR lpWindowName, DWORD dwStyle, DWORD dwExStyle);

        /**
         * @brief 初始化为控件，该函数会调用CreateWindowExW
         * @return 若函数成功则返回true，否则返回false
         */
        bool InitControl(LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, DWORD dwExStyle, LPVOID lpParam = NULL);

        /**
         * @brief 调用默认的WndProc，对于窗口则调用DefWindowProcW，控件则调用_controlOldWndProc
         */
        LRESULT DefaultWndProc(const ProcMsg &msg);

        /**
         * @brief 对WndProc的封装
         */
        virtual LRESULT WndProc(ProcMsg &refMsg);

        /**
         * @brief 获取内部记录窗口文本的字符串引用
         * @note Text属性的Get方法会调用该函数，部分控件如编辑框需要重写该函数以返回正确的文本
         */
        virtual std::wstring &GetInternalText();

        /**
         * @brief 修改窗口文本，该函数默认实现为调用SetWindowTextW
         * @note Text属性的Set方法会调用该函数，部分控件如下拉框需要重写该函数以正确设置文本
         */
        virtual void SetInternalText(const std::wstring &value);

        /**
         * @brief 接收到WM_CREATE时调用该函数
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnCreate();

        /**
         * @brief 接收到WM_CLOSE时调用该函数
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnClose();

        /**
         * @brief 接收到WM_DESTROY时调用该函数
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnDestroy();

        /**
         * @brief 接收到WM_PAINT时调用该函数
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnPaint();

        /**
         * @brief 在OnPaint函数完成之后调用该函数
         */
        virtual void OnEndPaint();

        /**
         * @brief 接收到WM_NCPAINT时调用该函数
         * @param hRgn 窗口更新区域的句柄，可能为NULL
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnNcPaint(HRGN hRgn);

        /**
         * @brief 在OnNcPaint函数完成之后调用该函数
         */
        virtual void OnEndNcPaint();

        /**
         * @brief 接收到WM_MOVE时调用该函数
         * @param newClientPosition 移动后用户区左上角的位置
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMove(const Point &newClientPosition);

        /**
         * @brief 接收到WM_SIZE时调用该函数
         * @param newClientSize 改变后的用户区尺寸
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnSize(const Size &newClientSize);

        /**
         * @brief Text属性更改时调用此函数
         */
        virtual void OnTextChanged();

        /**
         * @brief 接收到WM_SETFOCUS时调用该函数
         * @param hPrevFocus 丢失焦点的hwnd，可能为NULL
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnSetFocus(HWND hPrevFocus);

        /**
         * @brief 接收到WM_KILLFOCUS时调用该函数
         * @param hNextFocus 接收到焦点的hwnd，可能为NULL
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnKillFocus(HWND hNextFocus);

        /**
         * @brief 接收到WM_MOUSEMOVE时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState 指示某些按键是否按下
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseMove(const Point &mousePosition, MouseKey keyState);

        /**
         * @brief 接收到WM_MOUSELEAVE时调用该函数
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseLeave();

        /**
         * @brief 接收到WM_MOUSEWHEEL时调用该函数
         * @param wheelDelta 滚轮滚动的距离，为120的倍数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState 指示某些按键是否按下
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseWheel(int wheelDelta, const Point &mousePosition, MouseKey keyState);

        /**
         * @brief 接收到WM_LBUTTONDOWN时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState 指示某些按键是否按下
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseLeftButtonDown(const Point &mousePosition, MouseKey keyState);

        /**
         * @brief 接收到WM_LBUTTONUP时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState 指示某些按键是否按下
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseLeftButtonUp(const Point &mousePosition, MouseKey keyState);

        /**
         * @brief 接收到WM_LBUTTONDBLCLK时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState 指示某些按键是否按下
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseLeftButtonDoubleClick(const Point &mousePosition, MouseKey keyState);

        /**
         * @brief 接收到WM_RBUTTONDOWN时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState 指示某些按键是否按下
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseRightButtonDown(const Point &mousePosition, MouseKey keyState);

        /**
         * @brief 接收到WM_RBUTTONUP时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState 指示某些按键是否按下
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseRightButtonUp(const Point &mousePosition, MouseKey keyState);

        /**
         * @brief 接收到WM_RBUTTONDBLCLK时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState 指示某些按键是否按下
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseRightButtonDoubleClick(const Point &mousePosition, MouseKey keyState);

        /**
         * @brief 接收到WM_MBUTTONDOWN时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState 指示某些按键是否按下
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseMiddleButtonDown(const Point &mousePosition, MouseKey keyState);

        /**
         * @brief 接收到WM_MBUTTONUP时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState 指示某些按键是否按下
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseMiddleButtonUp(const Point &mousePosition, MouseKey keyState);

        /**
         * @brief 接收到WM_MBUTTONDBLCLK时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState 指示某些按键是否按下
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseMiddleButtonDoubleClick(const Point &mousePosition, MouseKey keyState);

        /**
         * @brief 接收到WM_CHAR时调用该函数
         * @param ch 按键的字符代码
         * @param flags 附加信息
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnChar(wchar_t ch, const KeyFlags &flags);

        /**
         * @brief 接收到WM_DEADCHAR时调用该函数
         * @param ch 按键的字符代码
         * @param flags 附加信息
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnDeadChar(wchar_t ch, const KeyFlags &flags);

        /**
         * @brief 接收到WM_KEYDOWN时调用该函数
         * @param key 虚拟按键
         * @param flags 附加信息
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnKeyDown(VirtualKey key, const KeyFlags &flags);

        /**
         * @brief 接收到WM_KEYUP时调用该函数
         * @param key 虚拟按键
         * @param flags 附加信息
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnKeyUp(VirtualKey key, const KeyFlags &flags);

        /**
         * @brief 接收到WM_SYSCHAR时调用该函数
         * @param ch 按键的字符代码
         * @param flags 附加信息
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnSysChar(wchar_t ch, const KeyFlags &flags);

        /**
         * @brief 接收到WM_SYSDEADCHAR时调用该函数
         * @param ch 按键的字符代码
         * @param flags 附加信息
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnSysDeadChar(wchar_t ch, const KeyFlags &flags);

        /**
         * @brief 接收到WM_SYSKEYDOWN时调用该函数
         * @param key 虚拟按键
         * @param flags 附加信息
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnSysKeyDown(VirtualKey key, const KeyFlags &flags);

        /**
         * @brief 接收到WM_SYSKEYUP时调用该函数
         * @param key 虚拟按键
         * @param flags 附加信息
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnSysKeyUp(VirtualKey key, const KeyFlags &flags);

        /**
         * @brief Visible属性改变时调用此函数
         */
        virtual void VisibleChanged(bool newVisible);

        /**
         * @brief 设置父窗口
         * @return 设置是否成功
         */
        virtual bool SetParent(WndBase *parent);

        /**
         * @brief 父窗口改变时调用此函数
         * @param newParent 新的父窗口
         */
        virtual void ParentChanged(WndBase *newParent);

        /**
         * @brief 当父窗口接收到控件的WM_COMMAND时调用该函数
         * @param code 通知代码
         */
        virtual void OnCommand(int code);

        /**
         * @brief 当WM_COMMAND接收到控件命令时调用该函数
         * @param pControl 控件对象指针
         * @param code 通知代码
         * @param id 控件id
         */
        virtual void OnControlCommand(WndBase *pControl, int code, int id);

        /**
         * @brief 当WM_COMMAND接收到菜单命令时调用该函数
         * @param id 菜单id
         */
        virtual void OnMenuCommand(int id);

        /**
         * @brief 当WM_COMMAND接收到快捷键命令时调用该函数
         * @param id 快捷键id
         */
        virtual void OnAcceleratorCommand(int id);

        /**
         * @brief 窗口句柄初始化完成
         * @param hwnd 窗口句柄
         */
        virtual void HandleInitialized(HWND hwnd);

        /**
         * @brief 字体改变时调用该函数
         * @param hfont 字体句柄
         */
        virtual void FontChanged(HFONT hfont);

        /**
         * @brief 接收到WM_SETCURSOR消息时调用该函数
         * @param hwnd 鼠标所在窗口的句柄
         * @param hitTest hit-test的结果，详见WM_NCHITTEST消息的返回值
         * @param message 触发该事件的鼠标消息，如WM_MOUSEMOVE
         * @param result 消息的返回值，默认为false
         * @return 若返回true则将result作为消息的返回值，否则使用DefaultWndProc的返回值
         */
        virtual bool OnSetCursor(HWND hwnd, HitTestResult hitTest, int message, bool &result);

        /**
         * @brief 接收到WM_CONTEXTMENU后调用目标控件的该函数
         * @param isKeyboardMsg 消息是否由按下快捷键（Shift+F10、VK_APPS）产生
         * @param mousePosition 鼠标在屏幕中的位置
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnContextMenu(bool isKeyboardMsg, const Point &mousePosition);

        /**
         * @brief 接收到WM_MENUSELECT后调用该函数
         * @param hMenu 当前被点击的菜单句柄
         * @param id 若选中的菜单项打开了子菜单则该值为菜单项在其父菜单中的索引，否则为菜单项的id
         * @param flags 菜单项的标志（MF_*）
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         * @note 若flags为0xFFFF且hMenu为NULL则表示菜单被关闭
         */
        virtual bool OnMenuSelect(HMENU hMenu, int id, int flags);

        /**
         * @brief 接收到WM_NOTIFY后调用该函数
         * @param pNMHDR 包含有关通知消息的信息
         * @param result 函数返回值为true时将该值作为消息的返回值，默认值为0
         * @return 若已处理该消息则返回true，否则调用发出通知控件的OnNotified函数，依据其返回值判断是否调用DefaultWndProc
         */
        virtual bool OnNotify(NMHDR *pNMHDR, LRESULT &result);

        /**
         * @brief 父窗口接收到WM_NOTIFY后且父窗口OnNotify函数返回false时调用发出通知控件的该函数
         * @param pNMHDR 包含有关通知消息的信息
         * @param result 函数返回值为true时将该值作为消息的返回值
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnNotified(NMHDR *pNMHDR, LRESULT &result);

        /**
         * @brief 接收到WM_VSCROLL时调用目标控件的该函数
         * @param event 事件类型，即消息wParam的低字
         * @param pos 当前滚动条的位置，仅当event为SB_THUMBPOSITION或SB_THUMBTRACK时有效
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnVerticalScroll(int event, int pos);

        /**
         * @brief 接收到WM_HSCROLL时调用目标控件的该函数
         * @param event 事件类型，即消息wParam的低字
         * @param pos 当前滚动条的位置，仅当event为SB_THUMBPOSITION或SB_THUMBTRACK时有效
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnHorizontalScroll(int event, int pos);

        /**
         * @brief 接收到WM_ENABLE时调用该函数
         * @param newValue Enabled的新值
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnEnabledChanged(bool newValue);

        /**
         * @brief 接收到WM_CTLCOLORxxx时调用该函数
         * @param pControl 消息相关的控件
         * @param hdc 控件的显示上下文句柄
         * @param hRetBrush 要返回的画笔
         * @return 若返回true则将hRetBrush作为消息的返回值，否则使用DefaultWndProc的返回值
         */
        virtual bool OnCtlColor(WndBase *pControl, HDC hdc, HBRUSH &hRetBrush);

        /**
         * @brief 父窗口接收到WM_CTLCOLORxxx时调用对应控件的该函数
         * @param hdc 控件的显示上下文句柄
         * @param hRetBrush 要返回的画笔
         * @return 若返回true则将hRetBrush作为消息的返回值，否则使用DefaultWndProc的返回值
         */
        virtual bool OnColor(HDC hdc, HBRUSH &hRetBrush);

        /**
         * @brief 接收到WM_NCHITTEST后调用该函数
         * @param testPoint 要测试的点在屏幕中的位置
         * @param result 测试的结果，默认为调用DefaultWndProc的结果
         */
        virtual void OnNcHitTest(const Point &testPoint, HitTestResult &result);

        /**
         * @brief 接收到WM_ERASEBKGND时调用该函数
         * @param hdc 设备上下文句柄
         * @param result 若已处理该消息则设为非零值，默认值为0
         * @return 若返回true则将result作为消息的返回值，否则使用DefaultWndProc的返回值
         */
        virtual bool OnEraseBackground(HDC hdc, LRESULT &result);

        /**
         * @brief 接收到WM_DRAWITEM时调用该函数
         * @param id 控件的标识符，若消息是通过菜单发送的则此参数为零
         * @param pDrawItem 包含有关要绘制的项和所需绘图类型的信息的结构体指针
         * @return 若已处理该消息则返回true，否则调用发出通知控件的OnDrawItemSelf函数，依据其返回值判断是否调用DefaultWndProc
         */
        virtual bool OnDrawItem(int id, DRAWITEMSTRUCT *pDrawItem);

        /**
         * @brief 父窗口接收到WM_DRAWITEM后且父窗口OnDrawItem函数返回false时调用发出通知控件的该函数
         * @param pDrawItem 包含有关要绘制的项和所需绘图类型的信息的结构体指针
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnDrawItemSelf(DRAWITEMSTRUCT *pDrawItem);

        /**
         * @brief 接收到WM_MEASUREITEM时调用该函数
         * @param id 控件的标识符，若消息是通过菜单发送的则此参数为零
         * @param pMeasure 包含有关要绘制的项的信息的结构体指针
         * @return 若已处理该消息则返回true，否则调用发出通知控件的OnMeasureItemSelf函数，依据其返回值判断是否调用DefaultWndProc
         */
        virtual bool OnMeasureItem(int id, MEASUREITEMSTRUCT *pMeasure);

        /**
         * @brief 父窗口接收到WM_MEASUREITEM后且父窗口OnMeasureItem函数返回false时调用发出通知控件的该函数
         * @param pMeasure 包含有关要绘制的项的信息的结构体指针
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMeasureItemSelf(MEASUREITEMSTRUCT *pMeasure);

        /**
         * @brief 接收到WM_DROPFILES时调用该函数
         * @param hDrop 描述拖入文件的句柄
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnDropFiles(HDROP hDrop);

    public:
        /**
         * @brief 获取当前窗口对应的Win32父窗口（HWND父）的WndBase包装
         * @return 若Win32父窗口存在且已注册为WndBase则返回该指针，否则返回nullptr
         * @note 该函数访问的是Win32 HWND树，而非逻辑树；与GetParent()语义不同
         */
        WndBase *GetParentWnd() const noexcept;

        /**
         * @brief 同步窗口位置和尺寸到内部记录的Rect
         */
        void UpdateInternalRect();

        /**
         * @brief 同步窗口文本到内部记录的字符串
         */
        void UpdateInternalText();

        /**
         * @brief 该函数调用ShowWindow
         */
        void Show(int nCmdShow);

        /**
         * @brief 发送关闭消息
         */
        void Close();

        /**
         * @brief 该函数调用UpdateWindow
         */
        void Update();

        /**
         * @brief 更新字体
         */
        void UpdateFont();

        /**
         * @brief 获取字体句柄
         */
        HFONT GetFontHandle() const noexcept;

        /**
         * @brief 重画
         * @param erase 是否擦除旧的背景
         * @param updateWindow 是否调用UpdateWindow
         */
        void Redraw(bool erase = false, bool updateWindow = false);

        /**
         * @brief 判断当前对象在界面中是否可视，与Visible属性不同的是该函数返回值会受父窗口的影响
         */
        bool IsVisible() const noexcept;

        /**
         * @brief 获取窗口样式
         */
        DWORD GetStyle() const noexcept;

        /**
         * @brief 设置窗口样式
         */
        void SetStyle(DWORD style) noexcept;

        /**
         * @brief 判断窗口是否设有指定样式
         * @param mask 样式的位掩码，可以是多个样式
         */
        bool GetStyle(DWORD mask) const noexcept;

        /**
         * @brief 打开或关闭指定的样式
         * @param mask 样式的位掩码，可以是多个样式
         * @param value 是否启用指定的样式
         */
        void SetStyle(DWORD mask, bool value) noexcept;

        /**
         * @brief 获取扩展窗口样式
         */
        DWORD GetExtendedStyle() const noexcept;

        /**
         * @brief 设置扩展窗口样式
         */
        void SetExtendedStyle(DWORD style) noexcept;

        /**
         * @brief 判断窗口是否设有指定扩展样式
         * @param mask 扩展样式的位掩码，可以是多个扩展样式
         */
        bool GetExtendedStyle(DWORD mask) const noexcept;

        /**
         * @brief 打开或关闭指定的扩展样式
         * @param mask 扩展样式的位掩码，可以是多个扩展样式
         * @param value 是否启用指定的扩展样式
         */
        void SetExtendedStyle(DWORD mask, bool value) noexcept;

        /**
         * @brief 获取用户区点在屏幕上点的位置
         * @param point 用户区坐标
         * @return 该点在屏幕上的坐标
         */
        Point PointToScreen(const Point &point) const noexcept;

        /**
         * @brief 获取屏幕上点在当前用户区点的位置
         * @param screenPoint 屏幕上点的坐标
         * @return 该点在用户区的坐标
         */
        Point PointFromScreen(const Point &screenPoint) const noexcept;

        /**
         * @brief 发送消息（ASCII）
         */
        LRESULT SendMessageA(UINT uMsg, WPARAM wParam, LPARAM lParam) const;

        /**
         * @brief 发送消息（UNICODE）
         */
        LRESULT SendMessageW(UINT uMsg, WPARAM wParam, LPARAM lParam) const;

        /**
         * @brief 发送消息（ASCII）并立即返回
         */
        BOOL PostMessageA(UINT uMsg, WPARAM wParam, LPARAM lParam) const noexcept;

        /**
         * @brief 发送消息（UNICODE）并立即返回
         */
        BOOL PostMessageW(UINT uMsg, WPARAM wParam, LPARAM lParam) const noexcept;

        /**
         * @brief 测试指定点在窗口的哪一部分
         * @param testPoint 要测试的点在屏幕中的位置
         */
        HitTestResult NcHitTest(const Point &testPoint);

        /**
         * @brief 在窗口线程上执行指定委托
         * @param action 要执行的委托
         */
        void Invoke(const Action<> &action);

        /**
         * @brief 在窗口线程上执行指定委托，并立即返回
         * @param action 要执行的委托
         * @return 若成功将委托放入窗口线程的消息队列则返回true，否则返回false
         */
        bool InvokeAsync(const Action<> &action);

        /**
         * @brief 获取当前窗口所属线程的线程id
         */
        DWORD GetThreadId() const noexcept;

        /**
         * @brief 判断当前线程是否为窗口所属线程
         */
        bool CheckAccess() const noexcept;

        /**
         * @brief 判断当前对象所属线程是否与另一个WndBase对象所属线程相同
         */
        bool CheckAccess(const WndBase &other) const noexcept;

    public:
        /**
         * @brief 通过窗口句柄获取WndBase
         * @param hwnd 窗口句柄
         * @return 若函数成功则返回对象的指针，否则返回nullptr
         */
        static WndBase *GetWndBase(HWND hwnd) noexcept;

        /**
         * @brief 检查指针是否指向有效的WndBase对象
         */
        static bool IsPtrValid(const WndBase *ptr) noexcept;

    private:
        /**
         * @brief 窗口过程函数，调用对象的WndProc
         */
        static LRESULT CALLBACK _WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

        /**
         * @brief CBT钩子过程，在WM_NCCREATE之前完成HWND绑定与（控件场景下的）WndProc子类化
         */
        static LRESULT CALLBACK _CbtProc(int code, WPARAM wParam, LPARAM lParam);

        /**
         * @brief 获取控件创建时所在的容器
         */
        static WndBase *_GetControlInitContainer();

        /**
         * @brief 获取一个新的控件id
         */
        static int _NextControlId();

        /**
         * @brief 关联窗口句柄与WndBase对象
         * @param hwnd 窗口句柄
         * @param wnd 与句柄关联的对象
         */
        static void _SetWndBase(HWND hwnd, WndBase &wnd);
    };
}

// Menu.h


namespace sw
{
    class MenuBase; // 向前声明

    /**
     * @brief 菜单被点击事件参数
     */
    class MenuItemClickedEventArgs : public EventArgs
    {
    public:
        /// @brief 菜单项所在的菜单
        MenuBase *menu;
    };

    /**
     * @brief 菜单项被点击事件处理函数类型
     */
    using MenuItemClickedEventHandler =
        EventHandler<MenuItem, MenuItemClickedEventArgs>;

    /**
     * @brief 菜单类型的基类
     */
    class MenuBase
    {
    private:
        /**
         * @brief 根菜单项
         */
        std::unique_ptr<MenuItem> _root;

        /**
         * @brief 菜单项被点击事件委托
         */
        MenuItemClickedEventHandler _itemClicked;

    public:
        /**
         * @brief 菜单项被点击事件
         */
        const Event<MenuItemClickedEventHandler> ItemClicked;

        /**
         * @brief 根菜单项
         */
        const ReadOnlyProperty<MenuItem *> Root;

        /**
         * @brief 菜单句柄
         */
        const ReadOnlyProperty<HMENU> Handle;

    protected:
        /**
         * @brief 创建菜单实例
         */
        MenuBase(MenuItem *root);

    public:
        /**
         * @brief 析构函数
         */
        virtual ~MenuBase() = default;

        // 删除拷贝构造函数
        MenuBase(const MenuBase &) = delete;

        // 删除移动构造函数
        MenuBase(MenuBase &&) = delete;

        // 删除拷贝赋值运算符
        MenuBase &operator=(const MenuBase &) = delete;

        // 删除移动赋值运算符
        MenuBase &operator=(MenuBase &&) = delete;

    public:
        /**
         * @brief 触发菜单项点击事件
         * @param menuItemId 菜单项ID
         * @return 若事件被成功触发则返回true，否则返回false
         */
        bool RaiseClickedEvent(int menuItemId);

        /**
         * @brief 查找对应ID的菜单项
         * @param id 菜单项ID
         * @return 指向菜单项的指针，如果未找到则返回nullptr
         */
        MenuItem *FindMenuItemById(int id);

        /**
         * @brief 查找对应tag的菜单项
         * @param tag 菜单项tag
         * @return 指向菜单项的指针，如果未找到则返回nullptr
         */
        MenuItem *FindMenuItemByTag(uint64_t tag);

        /**
         * @brief 查找对应文本的菜单项
         * @param text 菜单项文本
         * @return 指向菜单项的指针，如果未找到则返回nullptr
         */
        MenuItem *FindMenuItemByText(const std::wstring &text);
    };

    /**
     * @brief 窗口菜单
     */
    class Menu : public MenuBase
    {
    public:
        /**
         * @brief 初始化窗口菜单
         */
        Menu();

        /**
         * @brief 初始化窗口菜单并设置菜单项
         * @param items 菜单项列表
         */
        Menu(std::initializer_list<MenuItemDesc> items);
    };

    /**
     * @brief 上下文菜单
     */
    class ContextMenu : public MenuBase
    {
    public:
        /**
         * @brief 初始化上下文菜单
         */
        ContextMenu();

        /**
         * @brief 初始化上下文菜单并设置菜单项
         * @param items 菜单项列表
         */
        ContextMenu(std::initializer_list<MenuItemDesc> items);

        /**
         * @brief 在指定位置显示上下文菜单
         * @param hwnd 关联的窗口句柄
         * @param point 显示位置
         * @param horz 水平对齐方式，默认为左对齐
         * @param vert 垂直对齐方式，默认为顶部对齐
         * @return 若函数成功则返回true，否则返回false
         */
        bool Show(
            HWND hwnd, const Point &point,
            sw::HorizontalAlignment horz = sw::HorizontalAlignment::Left,
            sw::VerticalAlignment vert   = sw::VerticalAlignment::Top);
    };
}

// MsgBox.h


namespace sw
{
    /**
     * @brief 消息框按钮类型
     */
    enum class MsgBoxButton {
        AbortRetryIgnore    = MB_ABORTRETRYIGNORE,  ///< “中止”、“重试”、“忽略” 三按钮
        CancelRetryContinue = MB_CANCELTRYCONTINUE, ///< “取消”、“重试”、“继续” 三按钮
        Ok                  = MB_OK,                ///< “确定” 单按钮（默认）
        OkCancel            = MB_OKCANCEL,          ///< “确定”、“取消” 双按钮
        RetryCancel         = MB_RETRYCANCEL,       ///< “重试”、“取消” 双按钮
        YesNo               = MB_YESNO,             ///< “是”、“否” 双按钮
        YesNoCancel         = MB_YESNOCANCEL,       ///< “是”、“否”、“取消” 三按钮
    };

    /**
     * @brief 消息框图标类型
     */
    enum class MsgBoxIcon {
        None        = 0,                  ///< 不显示图标
        Warning     = MB_ICONWARNING,     ///< 警告图标（黄色三角形）
        Information = MB_ICONINFORMATION, ///< 信息图标（蓝色字母 i）
        Question    = MB_ICONQUESTION,    ///< 问号图标（微软已不建议使用，推荐改用 Warning）
        Error       = MB_ICONERROR,       ///< 错误图标（红色叉号）
    };

    /**
     * @brief 消息框结果
     */
    enum class MsgBoxResult {
        Abort    = IDABORT,    ///< 用户点击了 “中止” 按钮
        Cancel   = IDCANCEL,   ///< 用户点击了 “取消” 按钮，或按 Esc / 关闭按钮
        Continue = IDCONTINUE, ///< 用户点击了 “继续” 按钮
        Ignore   = IDIGNORE,   ///< 用户点击了 “忽略” 按钮
        No       = IDNO,       ///< 用户点击了 “否” 按钮
        Ok       = IDOK,       ///< 用户点击了 “确定” 按钮
        Retry    = IDRETRY,    ///< 用户点击了 “重试” 按钮
        TryAgain = IDTRYAGAIN, ///< 用户点击了 “重试” 按钮（仅 CancelRetryContinue 组合下产生）
        Yes      = IDYES,      ///< 用户点击了 “是” 按钮
    };

    /**
     * @brief 处理消息框消息的辅助类
     */
    class MsgBoxResultHelper
    {
    private:
        /**
         * @brief 消息框的结果
         */
        MsgBoxResult _result;

    public:
        /**
         * @brief 消息框的结果
         */
        ReadOnlyProperty<MsgBoxResult> Result;

    public:
        /**
         * @brief 构造MsgBoxResultHelper
         */
        MsgBoxResultHelper(MsgBoxResult result);

        /**
         * @brief 隐式转换MsgBoxResult
         */
        operator sw::MsgBoxResult() const noexcept;

        /**
         * @brief 指定按下“确定”按钮时的处理函数
         */
        MsgBoxResultHelper &OnOk(const Action<> &callback);

        /**
         * @brief 指定按下“是”按钮时的处理函数
         */
        MsgBoxResultHelper &OnYes(const Action<> &callback);

        /**
         * @brief 指定按下“否”按钮时的处理函数
         */
        MsgBoxResultHelper &OnNo(const Action<> &callback);

        /**
         * @brief 指定按下“取消”按钮时的处理函数
         */
        MsgBoxResultHelper &OnCancel(const Action<> &callback);

        /**
         * @brief 指定按下“中止”按钮时的处理函数
         */
        MsgBoxResultHelper &OnAbort(const Action<> &callback);

        /**
         * @brief 指定按下“继续”按钮时的处理函数
         */
        MsgBoxResultHelper &OnContinue(const Action<> &callback);

        /**
         * @brief 指定按下“忽略”按钮时的处理函数
         */
        MsgBoxResultHelper &OnIgnore(const Action<> &callback);

        /**
         * @brief 指定按下“重试”按钮时的处理函数
         */
        MsgBoxResultHelper &OnRetry(const Action<> &callback);

        /**
         * @brief 指定按下“重试”按钮时的处理函数
         * @note 只有在按钮类型为CancelRetryContinue并按下“重试”时才触发
         */
        MsgBoxResultHelper &OnTryAgain(const Action<> &callback);

    public:
        /**
         * @brief 指定消息框结果的处理函数
         */
        template <MsgBoxResult RES>
        MsgBoxResultHelper &On(const Action<> &callback)
        {
            if (this->_result == RES)
                if (callback) callback();
            return *this;
        }
    };

    /**
     * @brief 消息框类
     */
    class MsgBox
    {
    public:
        /**
         * @brief 静态类，不允许实例化
         */
        MsgBox() = delete;

    public:
        /**
         * @brief 显示消息框
         */
        static MsgBoxResultHelper Show(const WndBase *owner, const std::wstring &text = L"", const std::wstring &caption = L"", MsgBoxButton button = MsgBoxButton::Ok, MsgBoxIcon icon = MsgBoxIcon::None);

        /**
         * @brief 显示消息框
         */
        static MsgBoxResultHelper Show(const WndBase &owner, const std::wstring &text = L"", const std::wstring &caption = L"", MsgBoxButton button = MsgBoxButton::Ok, MsgBoxIcon icon = MsgBoxIcon::None);

        /**
         * @brief 显示消息框，将当前活动窗体作为Owner
         */
        static MsgBoxResultHelper Show(const std::wstring &text = L"", const std::wstring &caption = L"", MsgBoxButton button = MsgBoxButton::Ok, MsgBoxIcon icon = MsgBoxIcon::None);

        /**
         * @brief 显示一个含“消息”图标的消息框
         */
        static MsgBoxResultHelper ShowInfo(const WndBase *owner, const std::wstring &text = L"", const std::wstring &caption = L"", MsgBoxButton button = MsgBoxButton::Ok);

        /**
         * @brief 显示一个含“消息”图标的消息框
         */
        static MsgBoxResultHelper ShowInfo(const WndBase &owner, const std::wstring &text = L"", const std::wstring &caption = L"", MsgBoxButton button = MsgBoxButton::Ok);

        /**
         * @brief 显示一个含“消息”图标的消息框，将当前活动窗体作为Owner
         */
        static MsgBoxResultHelper ShowInfo(const std::wstring &text = L"", const std::wstring &caption = L"", MsgBoxButton button = MsgBoxButton::Ok);

        /**
         * @brief 显示一个含“错误”图标的消息框
         */
        static MsgBoxResultHelper ShowError(const WndBase *owner, const std::wstring &text = L"", const std::wstring &caption = L"", MsgBoxButton button = MsgBoxButton::Ok);

        /**
         * @brief 显示一个含“错误”图标的消息框
         */
        static MsgBoxResultHelper ShowError(const WndBase &owner, const std::wstring &text = L"", const std::wstring &caption = L"", MsgBoxButton button = MsgBoxButton::Ok);

        /**
         * @brief 显示一个含“错误”图标的消息框，将当前活动窗体作为Owner
         */
        static MsgBoxResultHelper ShowError(const std::wstring &text = L"", const std::wstring &caption = L"", MsgBoxButton button = MsgBoxButton::Ok);

        /**
         * @brief 显示一个含“警告”图标的消息框
         */
        static MsgBoxResultHelper ShowWarning(const WndBase *owner, const std::wstring &text = L"", const std::wstring &caption = L"", MsgBoxButton button = MsgBoxButton::Ok);

        /**
         * @brief 显示一个含“警告”图标的消息框
         */
        static MsgBoxResultHelper ShowWarning(const WndBase &owner, const std::wstring &text = L"", const std::wstring &caption = L"", MsgBoxButton button = MsgBoxButton::Ok);

        /**
         * @brief 显示一个含“警告”图标的消息框，将当前活动窗体作为Owner
         */
        static MsgBoxResultHelper ShowWarning(const std::wstring &text = L"", const std::wstring &caption = L"", MsgBoxButton button = MsgBoxButton::Ok);

        /**
         * @brief 显示一个含“问题”图标的消息框
         */
        static MsgBoxResultHelper ShowQuestion(const WndBase *owner, const std::wstring &text = L"", const std::wstring &caption = L"", MsgBoxButton button = MsgBoxButton::YesNo);

        /**
         * @brief 显示一个含“问题”图标的消息框
         */
        static MsgBoxResultHelper ShowQuestion(const WndBase &owner, const std::wstring &text = L"", const std::wstring &caption = L"", MsgBoxButton button = MsgBoxButton::YesNo);

        /**
         * @brief 显示一个含“问题”图标的消息框，将当前活动窗体作为Owner
         */
        static MsgBoxResultHelper ShowQuestion(const std::wstring &text = L"", const std::wstring &caption = L"", MsgBoxButton button = MsgBoxButton::YesNo);
    };
}

// Timer.h


namespace sw
{
    class Timer; // 向前声明

    /**
     * @brief 计时器触发事件类型
     */
    using TimerTickHandler = EventHandler<Timer>;

    /**
     * @brief 计时器
     */
    class Timer : public WndBase
    {
    private:
        /**
         * @brief 是否已启动
         */
        bool _started = false;

        /**
         * @brief 触发间隔
         */
        uint32_t _interval = 1000;

        /**
         * @brief 计时器触发事件的委托
         */
        TimerTickHandler _tick;

    public:
        /**
         * @brief 计时器触发事件
         */
        const Event<TimerTickHandler> Tick;

        /**
         * @brief 相对于上一次触发的Tick事件引发下一次Tick事件之间的时间（以毫秒为单位）
         */
        const Property<uint32_t> Interval;

    public:
        /**
         * @brief 初始化计时器
         */
        Timer();

        /**
         * @brief 开始计时器
         */
        void Start();

        /**
         * @brief 停止计时器
         */
        void Stop();

    protected:
        /**
         * @brief 计时器已启动并且达到间隔时间时调用该函数
         */
        virtual void OnTick();

    private:
        /**
         * @brief 通过窗口句柄获取Timer指针
         * @param hwnd 窗口句柄
         * @return 若函数成功则返回对象的指针，否则返回nullptr
         */
        static Timer *_GetTimerPtr(HWND hwnd);

        /**
         * @brief 关联窗口句柄与Timer对象
         * @param hwnd 窗口句柄
         * @param timer 与句柄关联的对象
         */
        static void _SetTimerPtr(HWND hwnd, Timer &timer);

        /**
         * @brief TimerProc回调函数
         */
        static void CALLBACK _TimerProc(HWND hwnd, UINT msg, UINT_PTR idTimer, DWORD time);
    };
}

// ToolTip.h


namespace sw
{
    /**
     * @brief 提示信息图标类型
     */
    enum class ToolTipIcon {
        None         = TTI_NONE,                ///< 无图标
        Info         = TTI_INFO,                ///< 信息图标
        Warning      = TTI_WARNING,             ///< 警告图标
        Error        = TTI_ERROR,               ///< 错误图标
        LargeInfo    = 4 /*TTI_INFO_LARGE*/,    ///< 大错误图标
        LargeWarning = 5 /*TTI_WARNING_LARGE*/, ///< 大错误图标
        LargeError   = 6 /*TTI_ERROR_LARGE*/,   ///< 大错误图标
    };

    /**
     * @brief 信息提示，用于用户将指针移过关联控件时显示信息
     */
    class ToolTip : public WndBase
    {
    private:
        /**
         * @brief 图标
         */
        ToolTipIcon _icon{ToolTipIcon::None};

        /**
         * @brief 标题
         */
        std::wstring _title{};

    public:
        /**
         * @brief 触发提示信息的时间，以毫秒为单位，设置负数可恢复默认值
         */
        const Property<int> InitialDelay;

        /**
         * @brief 提示框中显示的图标，标题不为空时图标才会显示
         */
        const Property<ToolTipIcon> ToolTipIcon;

        /**
         * @brief 提示框中显示的标题
         */
        const Property<std::wstring> ToolTipTitle;

        /**
         * @brief 提示框的最大宽度，若未设置则为-1，设置负值可取消限制
         */
        const Property<double> MaxTipWidth;

    public:
        /**
         * @brief 初始化ToolTip
         */
        ToolTip();

        /**
         * @brief 初始化ToolTip，指定窗口样式
         */
        explicit ToolTip(DWORD style);

    public:
        /**
         * @brief 给指定句柄设置提示信息
         * @param hwnd 要设置提示信息的句柄
         * @param tooltip 提示信息
         */
        bool SetToolTip(HWND hwnd, const std::wstring &tooltip);

        /**
         * @brief 给指定窗口或控件设置提示信息
         * @param wnd 要设置提示信息的窗口或控件
         * @param tooltip 提示信息
         */
        bool SetToolTip(const WndBase &wnd, const std::wstring &tooltip);

        /**
         * @brief 移除所有关联的提示信息
         */
        void RemoveAll();

    private:
        /**
         * @brief 更新图标和标题
         */
        void _UpdateIconAndTitle();
    };

    /**
     * @brief 气泡样式的信息提示
     */
    class BallonToolTip : public ToolTip
    {
    public:
        /**
         * @brief 初始化BallonToolTip
         */
        BallonToolTip();
    };
}

// UIElement.h


namespace sw
{
    // 前向声明
    class UIElement;
    class ContextMenu;
    class DataBinding;

    /**
     * @brief 通知布局更新的条件
     */
    enum class LayoutUpdateCondition : uint32_t {
        /// 尺寸改变时更新布局
        SizeChanged = 1 << 0,

        /// 位置改变时更新布局
        PositionChanged = 1 << 1,

        /// 添加子元素时更新布局
        ChildAdded = 1 << 2,

        /// 移除子元素时更新布局
        ChildRemoved = 1 << 3,

        /// 文本改变时更新布局
        TextChanged = 1 << 4,

        /// 字体改变时更新布局
        FontChanged = 1 << 5,

        /// 框架内部使用，表示布局已失效
        /// @note 该标记指示了Measure函数的结果已失效，需要重新调用Measure函数来更新尺寸
        MeasureInvalidated = 1 << 29,

        /// 框架内部使用，表示不需要更新布局
        /// @note 一旦设置了该标记，InvalidateMeasure函数将不会更新状态和触发布局更新
        /// @note 该标记用于抑制布局更新，可能会频繁被设置/取消，一般不建议用户直接使用
        Supressed = 1 << 30,
    };

    /**
     * @brief 标记LayoutUpdateCondition支持位操作
     */
    _SW_ENUM_ENABLE_BIT_OPERATIONS(LayoutUpdateCondition);

    /**
     * @brief 表示界面中的元素
     */
    class UIElement : public WndBase,
                      public ILayout
    {
    private:
        /**
         * @brief 布局更新条件
         */
        sw::LayoutUpdateCondition _layoutUpdateCondition =
            sw::LayoutUpdateCondition::SizeChanged |
            sw::LayoutUpdateCondition::ChildAdded |
            sw::LayoutUpdateCondition::ChildRemoved;

        /**
         * @brief 是否在不可见时不参与布局
         */
        bool _collapseWhenHide = true;

        /**
         * @brief 当前元素所需要占用的尺寸
         */
        Size _desireSize{};

        /**
         * @brief 当对齐方式为拉伸时用该字段存储原始大小
         */
        Size _origionalSize{};

        /**
         * @brief 最小尺寸，若值为负数或0则表示不限制最小尺寸
         */
        Size _minSize{-1, -1};

        /**
         * @brief 最大尺寸，若值为负数或0则表示不限制最大尺寸
         */
        Size _maxSize{-1, -1};

        /**
         * @brief 边距
         */
        Thickness _margin{};

        /**
         * @brief 水平对齐方式
         */
        HorizontalAlignment _horizontalAlignment = HorizontalAlignment::Center;

        /**
         * @brief 垂直对齐方式
         */
        VerticalAlignment _verticalAlignment = VerticalAlignment::Center;

        /**
         * @brief 指向父元素的指针，在调用SetParent后会更新该值
         */
        UIElement *_parent = nullptr;

        /**
         * @brief 所有子元素
         */
        std::vector<UIElement *> _children{};

        /**
         * @brief 参与布局的子元素，即所有非collapsed状态的子元素
         */
        std::vector<UIElement *> _layoutVisibleChildren{};

        /**
         * @brief 记录路由事件的map
         */
        std::unordered_map<RoutedEventType, RoutedEventHandler> _eventMap{};

        /**
         * @brief 布局标记
         */
        uint64_t _layoutTag = 0;

        /**
         * @brief 上下文菜单
         */
        sw::ContextMenu *_contextMenu = nullptr;

        /**
         * @brief Arrange时子元素的水平偏移量
         */
        double _arrangeOffsetX = 0;

        /**
         * @brief Arrange时子元素的垂直偏移量
         */
        double _arrangeOffsetY = 0;

        /**
         * @brief 所有子元素在当前元素中最右边的位置
         */
        double _childRightmost = 0;

        /**
         * @brief 所有子元素在当前元素中最底边的位置
         */
        double _childBottommost = 0;

        /**
         * @brief 元素是否悬浮，若元素悬浮则该元素不会随滚动条滚动而改变位置
         */
        bool _float = false;

        /**
         * @brief 表示用户是否可以通过按下Tab键将焦点移动到当前元素
         */
        bool _tabStop = false;

        /**
         * @brief 背景颜色
         */
        Color _backColor{KnownColors::White};

        /**
         * @brief 文本颜色
         */
        Color _textColor{KnownColors::Black};

        /**
         * @brief 是否使用透明背景
         */
        bool _transparent = false;

        /**
         * @brief 是否继承父元素的文本颜色
         */
        bool _inheritTextColor = false;

        /**
         * @brief 是否使用默认的鼠标样式
         */
        bool _useDefaultCursor = true;

        /**
         * @brief 鼠标句柄
         */
        HCURSOR _hCursor = NULL;

        /**
         * @brief 上一次Measure函数调用时的可用大小
         */
        Size _lastMeasureAvailableSize{};

        /**
         * @brief 用于存储批量调整子元素位置时调用DeferWindowPos的句柄
         */
        HDWP _hdwpChildren = NULL;

        /**
         * @brief OnColor函数中使用的背景画刷句柄
         */
        HBRUSH _hCtlColorBrush = NULL;

        /**
         * @brief 记录上一次调用OnColor时的文本颜色
         */
        COLORREF _lastTextColor = 0;

        /**
         * @brief 记录上一次调用OnColor时的背景颜色
         */
        COLORREF _lastBackColor = 0;

        /**
         * @brief 当前元素是否响应鼠标事件
         */
        bool _isHitTestVisible = true;

        /**
         * @brief 当前元素是否是通过按下Tab键获得的焦点
         */
        bool _focusedViaTab = false;

        /**
         * @brief 属性的绑定信息
         */
        std::unordered_map<FieldId, std::unique_ptr<BindingBase>> _bindings{};

    public:
        /**
         * @brief 边距
         */
        const Property<Thickness> Margin;

        /**
         * @brief 水平对齐方式
         */
        const Property<HorizontalAlignment> HorizontalAlignment;

        /**
         * @brief 垂直对齐方式
         */
        const Property<VerticalAlignment> VerticalAlignment;

        /**
         * @brief 子元素数量
         */
        const ReadOnlyProperty<int> ChildCount;

        /**
         * @brief 是否在不可见时不参与布局
         */
        const Property<bool> CollapseWhenHide;

        /**
         * @brief 布局标记，对于不同的布局有不同含义
         */
        const Property<uint64_t> LayoutTag;

        /**
         * @brief 右键按下时弹出的菜单
         */
        const Property<sw::ContextMenu *> ContextMenu;

        /**
         * @brief 元素是否悬浮，若元素悬浮则该元素不会随滚动条滚动而改变位置
         */
        const Property<bool> Float;

        /**
         * @brief 表示用户是否可以通过按下Tab键将焦点移动到当前元素
         */
        const Property<bool> TabStop;

        /**
         * @brief 背景颜色，修改该属性会同时将Transparent属性设为false，对于部分控件该属性可能不生效
         */
        const Property<Color> BackColor;

        /**
         * @brief 文本颜色，修改该属性会同时将InheritTextColor属性设为false，对于部分控件该属性可能不生效
         */
        const Property<Color> TextColor;

        /**
         * @brief 是否使用透明背景
         * @note 此属性并非真正意义上的透明，将该属性设为true可继承父元素的背景颜色
         */
        const Property<bool> Transparent;

        /**
         * @brief 是否继承父元素的文本颜色
         */
        const Property<bool> InheritTextColor;

        /**
         * @brief 触发布局更新的条件
         * @note 修改该属性不会立即触发布局更新
         */
        const Property<sw::LayoutUpdateCondition> LayoutUpdateCondition;

        /**
         * @brief 当前元素的布局状态是否有效
         */
        const ReadOnlyProperty<bool> IsMeasureValid;

        /**
         * @brief 最小宽度，当值为负数或0时表示不限制
         */
        const Property<double> MinWidth;

        /**
         * @brief 最小高度，当值为负数或0时表示不限制
         */
        const Property<double> MinHeight;

        /**
         * @brief 最大宽度，当值为负数或0时表示不限制
         */
        const Property<double> MaxWidth;

        /**
         * @brief 最大高度，当值为负数或0时表示不限制
         */
        const Property<double> MaxHeight;

        /**
         * @brief 元素的逻辑位置和尺寸，即去除布局偏移以及拉伸影响的位置和尺寸
         * @note 当布局未完成时该属性的值可能不准确
         */
        const ReadOnlyProperty<sw::Rect> LogicalRect;

        /**
         * @brief 当前元素是否响应鼠标事件
         */
        const Property<bool> IsHitTestVisible;

        /**
         * @brief 当前元素是否是通过按下Tab键获得的焦点
         */
        const ReadOnlyProperty<bool> IsFocusedViaTab;

    public:
        /**
         * @brief 初始化UIElement
         */
        UIElement();

        /**
         * @brief 析构函数，这里用纯虚函数使该类成为抽象类
         */
        virtual ~UIElement() = 0;

        /**
         * @brief 注册路由事件处理函数，当事件已注册时会覆盖已注册的函数
         * @param eventType 路由事件类型
         * @param handler 处理函数，当值为nullptr时可取消注册
         * @deprecated 使用AddHandler函数代替以避免覆盖已注册的事件处理函数
         */
        [[deprecated("Use AddHandler instead to avoid overwriting existing event handlers.")]]
        void RegisterRoutedEvent(RoutedEventType eventType, const RoutedEventHandler &handler);

        /**
         * @brief 添加路由事件处理函数
         * @param eventType 路由事件类型
         * @param handler 处理函数
         */
        void AddHandler(RoutedEventType eventType, const RoutedEventHandler &handler);

        /**
         * @brief 移除已添加的路由事件处理函数
         * @param eventType 路由事件类型
         * @param handler 处理函数
         * @return 是否成功移除
         */
        bool RemoveHandler(RoutedEventType eventType, const RoutedEventHandler &handler);

        /**
         * @brief 取消对应类型路由事件的注册，该函数会移除对应事件所有的处理函数
         * @param eventType 路由事件类型
         */
        void UnregisterRoutedEvent(RoutedEventType eventType);

        /**
         * @brief 判断路由事件是否已存在事件处理函数
         * @param eventType 路由事件类型
         */
        bool IsRoutedEventRegistered(RoutedEventType eventType);

        /**
         * @brief 添加子元素
         * @return 若函数成功则返回true，否则返回false
         * @note 添加的子元素必须与当前元素在同一线程创建
         */
        bool AddChild(UIElement *element);

        /**
         * @brief 添加子元素
         * @return 若函数成功则返回true，否则返回false
         * @note 添加的子元素必须与当前元素在同一线程创建
         */
        bool AddChild(UIElement &element);

        /**
         * @brief 添加子元素并设置布局标记
         * @return 若函数成功则返回true，否则返回false
         * @note 添加的子元素必须与当前元素在同一线程创建
         */
        bool AddChild(UIElement *element, uint64_t layoutTag);

        /**
         * @brief 添加子元素并设置布局标记
         * @return 若函数成功则返回true，否则返回false
         * @note 添加的子元素必须与当前元素在同一线程创建
         */
        bool AddChild(UIElement &element, uint64_t layoutTag);

        /**
         * @brief 移除指定索引处的子元素
         * @param index 要移除的索引
         * @return 移除是否成功
         */
        bool RemoveChildAt(int index);

        /**
         * @brief 移除子元素
         * @return 移除是否成功
         */
        bool RemoveChild(UIElement *element);

        /**
         * @brief 移除子元素
         * @return 移除是否成功
         */
        bool RemoveChild(UIElement &element);

        /**
         * @brief 移除所有子元素
         */
        void ClearChildren();

        /**
         * @brief 获取指定元素的索引
         * @param element 要查找的元素
         * @return 若找到指定元素则返回对应的索引，否则返回-1
         */
        int IndexOf(UIElement *element);

        /**
         * @brief 获取指定元素的索引
         * @param element 要查找的元素
         * @return 若找到指定元素则返回对应的索引，否则返回-1
         */
        int IndexOf(UIElement &element);

        /**
         * @brief 移动到界面顶部
         */
        void MoveToTop();

        /**
         * @brief 移动到界面底部
         */
        void MoveToBottom();

        /**
         * @brief 判断当前元素是否为根节点
         */
        bool IsRootElement() const;

        /**
         * @brief 获取当前元素所在界面树的根节点
         */
        UIElement *GetRootElement();

        /**
         * @brief 获取当前元素在界面树上的下一个节点，若已是最后一个节点则返回根节点
         */
        UIElement *GetNextElement();

        /**
         * @brief 获取当前元素在界面树上的上一个节点，若元素为第一个节点则返回最后一个节点
         */
        UIElement *GetPreviousElement();

        /**
         * @brief 获取下一个TabStop属性为true的元素
         */
        UIElement *GetNextTabStopElement();

        /**
         * @brief 获取上一个TabStop属性为true的元素
         */
        UIElement *GetPreviousTabStopElement();

        /**
         * @brief 获取当前要显示的背景颜色
         * @return 当Transparent为true时获取到祖先节点中首个Transparent为false的背景颜色，否则返回当前元素的背景颜色
         */
        Color GetRealBackColor() const;

        /**
         * @brief 获取当前要显示的文本颜色
         * @return 当InheritTextColor为true时获取到祖先节点中首个InheritTextColor为false的文本颜色，否则返回当前元素的文本颜色
         */
        Color GetRealTextColor() const;

        /**
         * @brief 设置鼠标样式
         * @param hCursor 鼠标句柄
         */
        void SetCursor(HCURSOR hCursor);

        /**
         * @brief 设置鼠标样式
         * @param cursor 鼠标样式
         */
        void SetCursor(StandardCursor cursor);

        /**
         * @brief 将鼠标样式设置为默认样式
         */
        void ResetCursor();

        /**
         * @brief 设置对齐方式
         * @param horz 水平对齐方式
         * @param vert 垂直对齐方式
         */
        void SetAlignment(sw::HorizontalAlignment horz, sw::VerticalAlignment vert);

        /**
         * @brief 调整当前元素的尺寸
         * @note 通过该函数可以调整横向或纵向对齐方式为拉伸时的DesireSize
         */
        void Resize(const Size &size);

        /**
         * @brief 判断指定的布局更新条件是否已设置
         */
        bool IsLayoutUpdateConditionSet(sw::LayoutUpdateCondition condition);

        /**
         * @brief 使元素的布局状态失效，并立即触发布局更新
         */
        void InvalidateMeasure();

        /**
         * @brief 尝试将当前元素移动到可视区域内
         * @return 若函数成功则返回true，否则返回false
         * @note 对于悬浮元素（Float属性为true）始终返回false
         */
        bool BringIntoView();

        /**
         * @brief 尝试将对象转换成UIElement
         * @return 若函数成功则返回UIElement指针，否则返回nullptr
         */
        virtual UIElement *ToUIElement() override final;

        /**
         * @brief 获取逻辑树中的父元素
         * @return 父元素指针，如果没有父元素则返回nullptr
         */
        virtual UIElement *GetParent() const override final;

        /**
         * @brief 获取逻辑树中的子元素数量
         * @return 子元素数量
         */
        virtual int GetChildCount() const override final;

        /**
         * @brief 获取逻辑树中指定索引处的子元素
         * @param index 子元素索引
         * @throw std::out_of_range 如果索引超出范围
         */
        virtual UIElement &GetChildAt(int index) const override final;

        /**
         * @brief 获取布局标记
         */
        virtual uint64_t GetLayoutTag() const override;

        /**
         * @brief 获取当前元素所需尺寸
         */
        virtual Size GetDesireSize() const override;

        /**
         * @brief 获取参与布局的子元素数量
         * @note 参与布局的子元素：即非collapsed状态的元素
         */
        virtual int GetChildLayoutCount() const override final;

        /**
         * @brief 获取对应索引处的子元素，只索引参与布局的子元素
         * @throw std::out_of_range 如果索引超出范围
         * @note 参与布局的子元素：即非collapsed状态的元素
         */
        virtual UIElement &GetChildLayoutAt(int index) const override final;

        /**
         * @brief 测量元素所需尺寸
         * @param availableSize 可用的尺寸
         */
        virtual void Measure(const Size &availableSize) override;

        /**
         * @brief 安排元素位置
         * @param finalSize 最终元素所安排的位置
         */
        virtual void Arrange(const sw::Rect &finalPosition) override;

    protected:
        /**
         * @brief 触发路由事件
         * @param eventType 事件类型
         */
        void RaiseRoutedEvent(RoutedEventType eventType);

        /**
         * @brief 触发路由事件
         * @param eventArgs 要触发事件的事件参数
         */
        void RaiseRoutedEvent(RoutedEventArgs &eventArgs);

        /**
         * @brief 获取布局时子元素的水平偏移量
         * @note 内部使用，Layer类中通过修改该值来实现子元素的布局偏移
         */
        double &GetInternalArrangeOffsetX();

        /**
         * @brief 获取布局时子元素的垂直偏移量
         * @note 内部使用，Layer类中通过修改该值来实现子元素的布局偏移
         */
        double &GetInternalArrangeOffsetY();

        /**
         * @brief 获取所有子元素在当前元素中最右边的位置（只考虑参与布局的子窗口且忽略悬浮的元素）
         * @param update 是否更字段
         * @return _childRightmost字段
         */
        double GetChildRightmost(bool update);

        /**
         * @brief 获取所有子元素在当前元素中最底边的位置（只考虑参与布局的子窗口且忽略悬浮的元素）
         * @param update 是否更字段
         * @return _childBottommost字段
         */
        double GetChildBottommost(bool update);

        /**
         * @brief 更新子元素的Z轴位置
         */
        void UpdateChildrenZOrder(bool invalidateMeasure = true);

        /**
         * @brief 更新兄弟元素的Z轴位置
         */
        void UpdateSiblingsZOrder(bool invalidateMeasure = true);

        /**
         * @brief 限定指定尺寸在最小和最大尺寸之间
         * @param size 要限定的尺寸，不包含边距
         */
        void ClampDesireSize(sw::Size &size) const;

        /**
         * @brief 限定指定矩形的尺寸在最小和最大尺寸之间
         * @param rect 要限定的矩形，不包含边距
         */
        void ClampDesireSize(sw::Rect &rect) const;

        /**
         * @brief 查询所有子元素，直到queryFunc返回false或所有子元素均被查询
         * @param queryFunc 查询函数，参数为子元素指针，返回值为bool，返回false时停止查询
         * @return 若queryFunc在某次调用中返回false则返回false，否则返回true
         */
        bool QueryAllChildren(const Predicate<UIElement *> &queryFunc);

        /**
         * @brief 查询自身和所有子元素，直到queryFunc返回false或所有子元素均被查询
         * @param queryFunc 查询函数，参数为元素指针，返回值为bool，返回false时停止查询
         * @return 若queryFunc在某次调用中返回false则返回false，否则返回true
         */
        bool QueryAllElements(const Predicate<UIElement *> &queryFunc);

        /**
         * @brief 测量元素所需尺寸，无需考虑边框和边距
         * @param availableSize 可用的尺寸
         * @return 返回元素需要占用的尺寸，若返回Size{NAN,NAN}则使用默认实现
         * @note 返回值除了Size{NAN,NAN}表示默认尺寸外不应包含NAN、INF、负数等非法值
         */
        virtual Size MeasureOverride(const Size &availableSize);

        /**
         * @brief 安排子元素的位置，可重写该函数以实现自定义布局
         * @param finalSize 可用于排列子元素的最终尺寸
         */
        virtual void ArrangeOverride(const Size &finalSize);

        /**
         * @brief 设置背景颜色
         * @param color 要设置的颜色
         * @param redraw 是否重绘
         */
        virtual void OnSetBackColor(Color color, bool redraw);

        /**
         * @brief 设置文本颜色
         * @param color 要设置的颜色
         * @param redraw 是否重绘
         */
        virtual void OnSetTextColor(Color color, bool redraw);

        /**
         * @brief 尝试将指定的矩形区域移动到可视区域内
         * @param screenRect 要移动到可视区域的矩形在屏幕中的位置
         * @return 若已处理该请求则返回true，否则返回false以继续向上冒泡
         */
        virtual bool RequestBringIntoView(const sw::Rect &screenRect);

        /**
         * @brief 添加子元素后调用该函数
         * @param element 添加的子元素
         */
        virtual void OnAddedChild(UIElement &element);

        /**
         * @brief 移除子元素后调用该函数
         * @param element 移除的子元素
         * @note 该函数也会在程序退出阶段（父元素HWND已销毁或正在销毁时）被调用，
         *       重写中若对自身HWND执行SendMessage或调用其它依赖有效句柄的控件API，
         *       必须先用!IsDestroyed守卫，否则可能触发控件内部异常路径甚至卡住进程
         */
        virtual void OnRemovedChild(UIElement &element);

        /**
         * @brief 通过tab键将焦点从当前元素移出时调用该函数
         * @param forward 指示焦点移动的方向，true表示向后移动，false表示向前移动
         */
        virtual void OnTabMove(bool forward);

        /**
         * @brief 通过tab键将焦点移动到当前元素时调用该函数
         */
        virtual void OnTabStop();

        /**
         * @brief 当MinWidth、MinHeight、MaxWidth或MaxHeight属性更改时调用此函数
         */
        virtual void OnMinMaxSizeChanged();

        /**
         * @brief 路由事件经过当前元素时调用该函数
         * @param eventArgs 事件参数
         * @param handler 事件处理函数，值为空时表示当前元素没有注册该事件处理函数
         */
        virtual void OnRoutedEvent(RoutedEventArgs &eventArgs, const RoutedEventHandler &handler);

        /**
         * @brief 设置父窗口
         * @return 设置是否成功
         */
        virtual bool SetParent(WndBase *parent) override;

        /**
         * @brief 父窗口改变时调用此函数
         * @param newParent 新的父窗口
         */
        virtual void ParentChanged(WndBase *newParent) override;

        /**
         * @brief 接收到WM_CLOSE时调用该函数
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnClose() override;

        /**
         * @brief 接收到WM_MOVE时调用该函数
         * @param newClientPosition 移动后用户区左上角的位置
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMove(const Point &newClientPosition) override;

        /**
         * @brief 接收到WM_SIZE时调用该函数
         * @param newClientSize 改变后的用户区尺寸
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnSize(const Size &newClientSize) override;

        /**
         * @brief Text属性更改时调用此函数
         */
        virtual void OnTextChanged() override;

        /**
         * @brief 字体改变时调用该函数
         * @param hfont 字体句柄
         */
        virtual void FontChanged(HFONT hfont) override;

        /**
         * @brief Visible属性改变时调用此函数
         */
        virtual void VisibleChanged(bool newVisible) override;

        /**
         * @brief 接收到WM_SETFOCUS时调用该函数
         * @param hPrevFocus 丢失焦点的hwnd，可能为NULL
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnSetFocus(HWND hPrevFocus) override;

        /**
         * @brief 接收到WM_KILLFOCUS时调用该函数
         * @param hNextFocus 接收到焦点的hwnd，可能为NULL
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnKillFocus(HWND hNextFocus) override;

        /**
         * @brief 接收到WM_CHAR时调用该函数
         * @param ch 按键的字符代码
         * @param flags 附加信息
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnChar(wchar_t ch, const KeyFlags &flags) override;

        /**
         * @brief 接收到WM_KEYDOWN时调用该函数
         * @param key 虚拟按键
         * @param flags 附加信息
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnKeyDown(VirtualKey key, const KeyFlags &flags) override;

        /**
         * @brief 接收到WM_KEYUP时调用该函数
         * @param key 虚拟按键
         * @param flags 附加信息
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnKeyUp(VirtualKey key, const KeyFlags &flags) override;

        /**
         * @brief 接收到WM_MOUSEMOVE时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState 指示某些按键是否按下
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseMove(const Point &mousePosition, MouseKey keyState) override;

        /**
         * @brief 接收到WM_MOUSELEAVE时调用该函数
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseLeave() override;

        /**
         * @brief 接收到WM_MOUSEWHEEL时调用该函数
         * @param wheelDelta 滚轮滚动的距离，为120的倍数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState 指示某些按键是否按下
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseWheel(int wheelDelta, const Point &mousePosition, MouseKey keyState) override;

        /**
         * @brief 接收到WM_LBUTTONDOWN时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState 指示某些按键是否按下
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseLeftButtonDown(const Point &mousePosition, MouseKey keyState) override;

        /**
         * @brief 接收到WM_LBUTTONUP时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState 指示某些按键是否按下
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseLeftButtonUp(const Point &mousePosition, MouseKey keyState) override;

        /**
         * @brief 接收到WM_RBUTTONDOWN时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState 指示某些按键是否按下
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseRightButtonDown(const Point &mousePosition, MouseKey keyState) override;

        /**
         * @brief 接收到WM_RBUTTONUP时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState 指示某些按键是否按下
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseRightButtonUp(const Point &mousePosition, MouseKey keyState) override;

        /**
         * @brief 接收到WM_MBUTTONDOWN时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState 指示某些按键是否按下
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseMiddleButtonDown(const Point &mousePosition, MouseKey keyState) override;

        /**
         * @brief 接收到WM_MBUTTONUP时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState 指示某些按键是否按下
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseMiddleButtonUp(const Point &mousePosition, MouseKey keyState) override;

        /**
         * @brief 接收到WM_CONTEXTMENU后调用目标控件的该函数
         * @param isKeyboardMsg 消息是否由按下快捷键（Shift+F10、VK_APPS）产生
         * @param mousePosition 鼠标在屏幕中的位置
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnContextMenu(bool isKeyboardMsg, const Point &mousePosition) override;

        /**
         * @brief 当WM_COMMAND接收到菜单命令时调用该函数
         * @param id 菜单id
         */
        virtual void OnMenuCommand(int id) override;

        /**
         * @brief 父窗口接收到WM_CTLCOLORxxx时调用对应控件的该函数
         * @param hdc 控件的显示上下文句柄
         * @param hRetBrush 要返回的画笔
         * @return 若返回true则将hRetBrush作为消息的返回值，否则使用DefaultWndProc的返回值
         */
        virtual bool OnColor(HDC hdc, HBRUSH &hRetBrush) override;

        /**
         * @brief 接收到WM_SETCURSOR消息时调用该函数
         * @param hwnd 鼠标所在窗口的句柄
         * @param hitTest hit-test的结果，详见WM_NCHITTEST消息的返回值
         * @param message 触发该事件的鼠标消息，如WM_MOUSEMOVE
         * @param result 消息的返回值，默认为false
         * @return 若返回true则将result作为消息的返回值，否则使用DefaultWndProc的返回值
         */
        virtual bool OnSetCursor(HWND hwnd, HitTestResult hitTest, int message, bool &result) override;

        /**
         * @brief 接收到WM_DROPFILES时调用该函数
         * @param hDrop 描述拖入文件的句柄
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnDropFiles(HDROP hDrop) override;

        /**
         * @brief 接收到WM_NCHITTEST后调用该函数
         * @param testPoint 要测试的点在屏幕中的位置
         * @param result 测试的结果，默认为调用DefaultWndProc的结果
         */
        virtual void OnNcHitTest(const Point &testPoint, HitTestResult &result) override;

    private:
        /**
         * @brief 设置水平对齐方式
         * @param value 要设置的值
         * @return 值是否发生改变
         */
        bool _SetHorzAlignment(sw::HorizontalAlignment value);

        /**
         * @brief 设置垂直对齐方式
         * @param value 要设置的值
         * @return 值是否发生改变
         */
        bool _SetVertAlignment(sw::VerticalAlignment value);

        /**
         * @brief 添加MeasureInvalidated标记
         */
        void _SetMeasureInvalidated();

        /**
         * @brief 更新_layoutVisibleChildren的内容
         */
        void _UpdateLayoutVisibleChildren();

        /**
         * @brief 添加元素到_layoutVisibleChildren中
         * @return 若元素在布局中不可见则返回false，否则添加元素并返回true
         */
        bool _AddToLayoutVisibleChildren(UIElement *element);

        /**
         * @brief 从_layoutVisibleChildren中移除指定元素
         */
        void _RemoveFromLayoutVisibleChildren(UIElement *element);

        /**
         * @brief 循环获取界面树上的下一个节点
         */
        static UIElement *_GetNextElement(UIElement *element);

        /**
         * @brief 获取界面树上最深的最后一个节点
         */
        static UIElement *_GetDeepestLastElement(UIElement *element);

        /**
         * @brief 循环获取界面树上的上一个节点
         */
        static UIElement *_GetPreviousElement(UIElement *element);

    private:
        /**
         * @brief 判断AddChild是否有对应类型的重载
         */
        template <typename T, typename = void>
        struct _CanAddChild : std::false_type {
        };

        // 已移动到类外，防止clang报错不完整的类型
        // /**
        //  * @brief _CanAddChild模板偏特化，当AddChild有对应类型的重载时该偏特化生效
        //  */
        // template <typename T>
        // struct _CanAddChild<
        //     T, decltype(void(std::declval<UIElement>().AddChild(std::declval<T>())))> : std::true_type {
        // };

        /**
         * @brief 判断AddChildren的参数类型是否均可添加
         */
        template <typename First, typename... Rest>
        struct _CanAddChildren
            : std::integral_constant<bool, _CanAddChild<First>::value && _CanAddChildren<Rest...>::value> {
        };

        /**
         * @brief _CanAddChildren模板偏特化，递归终止条件
         */
        template <typename T>
        struct _CanAddChildren<T> : _CanAddChild<T> {
        };

        /**
         * @brief AddChildren的内部实现
         * @return 返回成功添加的子元素数量
         */
        template <typename T>
        int _AddChildrenImpl(T &&child)
        {
            return this->AddChild(std::forward<T>(child)) ? 1 : 0;
        }

        /**
         * @brief AddChildren的内部实现
         * @return 返回成功添加的子元素数量
         */
        template <typename First, typename... Rest>
        int _AddChildrenImpl(First &&first, Rest &&...rest)
        {
            int count = 0;
            if (this->AddChild(std::forward<First>(first)))
                count = 1 + this->_AddChildrenImpl(std::forward<Rest>(rest)...);
            return count;
        }

    public:
        /**
         * @brief 添加多个子元素
         * @return 返回成功添加的子元素数量
         * @note 当有一个子元素添加失败时后续的子元素将不会继续添加
         * @note 添加的子元素必须与当前元素在同一线程创建
         */
        template <typename First, typename... Rest>
        auto AddChildren(First &&first, Rest &&...rest)
            -> typename std::enable_if<_CanAddChildren<First, Rest...>::value, int>::type
        {
            this->_layoutUpdateCondition |= sw::LayoutUpdateCondition::Supressed;
            int count = this->_AddChildrenImpl(std::forward<First>(first), std::forward<Rest>(rest)...);
            this->_layoutUpdateCondition &= ~sw::LayoutUpdateCondition::Supressed;

            if (this->IsLayoutUpdateConditionSet(sw::LayoutUpdateCondition::ChildAdded) && count > 0)
                this->InvalidateMeasure();
            return count;
        }

        /**
         * @brief 注册成员函数作为路由事件处理函数，当事件已注册时会覆盖已注册的函数
         * @tparam T 成员函数所在的类
         * @param eventType 路由事件类型
         * @param obj 注册的成员函数所在的对象
         * @param handler 处理函数，当值为nullptr时可取消注册
         * @deprecated 使用AddHandler函数代替以避免覆盖已注册的事件处理函数
         */
        template <typename T>
        [[deprecated("Use AddHandler instead to avoid overwriting existing event handlers.")]]
        void RegisterRoutedEvent(RoutedEventType eventType, T &obj, void (T::*handler)(UIElement &, RoutedEventArgs &))
        {
            if (handler == nullptr) {
                this->UnregisterRoutedEvent(eventType);
            } else {
                this->RegisterRoutedEvent(eventType, RoutedEventHandler(obj, handler));
            }
        }

        /**
         * @brief 根据事件参数类型注册路由事件，当事件已注册时会覆盖已注册的函数
         * @tparam TEventArgs 路由事件的参数类型，必须继承自TypedRoutedEventArgs<...>
         * @param handler 事件的处理函数，当值为nullptr时可取消注册
         * @deprecated 使用AddHandler函数代替以避免覆盖已注册的事件处理函数
         */
        template <typename TEventArgs>
        [[deprecated("Use AddHandler instead to avoid overwriting existing event handlers.")]]
        auto RegisterRoutedEvent(const Action<UIElement &, TEventArgs &> &handler)
            -> typename std::enable_if<std::is_base_of<RoutedEventArgs, TEventArgs>::value && sw::_IsTypedRoutedEventArgs<TEventArgs>::value>::type
        {
            if (!handler) {
                this->UnregisterRoutedEvent(TEventArgs::EventType);
            } else {
                this->RegisterRoutedEvent(TEventArgs::EventType, RoutedEventHandlerWrapper<TEventArgs>(handler));
            }
        }

        /**
         * @brief 根据事件参数类型注册成员函数作为路由事件，当事件已注册时会覆盖已注册的函数
         * @tparam TEventArgs 路由事件的参数类型，必须继承自TypedRoutedEventArgs<...>
         * @tparam THandleObj 成员函数所在的类
         * @param obj 注册的成员函数所在的对象
         * @param handler 事件的处理函数，当值为nullptr时可取消注册
         * @deprecated 使用AddHandler函数代替以避免覆盖已注册的事件处理函数
         */
        template <typename TEventArgs, typename THandleObj>
        [[deprecated("Use AddHandler instead to avoid overwriting existing event handlers.")]]
        auto RegisterRoutedEvent(THandleObj &obj, void (THandleObj::*handler)(UIElement &, TEventArgs &))
            -> typename std::enable_if<std::is_base_of<RoutedEventArgs, TEventArgs>::value && sw::_IsTypedRoutedEventArgs<TEventArgs>::value>::type
        {
            if (handler == nullptr) {
                this->UnregisterRoutedEvent(TEventArgs::EventType);
            } else {
                this->RegisterRoutedEvent(TEventArgs::EventType, RoutedEventHandlerWrapper<TEventArgs>(Action<UIElement &, TEventArgs &>(obj, handler)));
            }
        }

        /**
         * @brief 添加成员函数作为路由事件处理函数
         * @tparam T 成员函数所在的类
         * @param eventType 路由事件类型
         * @param obj 注册的成员函数所在的对象
         * @param handler 处理函数
         */
        template <typename T>
        void AddHandler(RoutedEventType eventType, T &obj, void (T::*handler)(UIElement &, RoutedEventArgs &))
        {
            if (handler) this->_eventMap[eventType].Add(obj, handler);
        }

        /**
         * @brief 根据事件参数类型添加路由事件处理函数
         * @tparam TEventArgs 路由事件的参数类型，必须继承自TypedRoutedEventArgs<...>
         * @param handler 事件的处理函数
         */
        template <typename TEventArgs>
        auto AddHandler(const Action<UIElement &, TEventArgs &> &handler)
            -> typename std::enable_if<std::is_base_of<RoutedEventArgs, TEventArgs>::value && sw::_IsTypedRoutedEventArgs<TEventArgs>::value>::type
        {
            if (handler) this->AddHandler(TEventArgs::EventType, RoutedEventHandlerWrapper<TEventArgs>(handler));
        }

        /**
         * @brief 根据事件参数类型添加成员函数作为路由事件处理函数
         * @tparam TEventArgs 路由事件的参数类型，必须继承自TypedRoutedEventArgs<...>
         * @tparam THandleObj 成员函数所在的类
         * @param obj 注册的成员函数所在的对象
         * @param handler 事件的处理函数
         */
        template <typename TEventArgs, typename THandleObj>
        auto AddHandler(THandleObj &obj, void (THandleObj::*handler)(UIElement &, TEventArgs &))
            -> typename std::enable_if<std::is_base_of<RoutedEventArgs, TEventArgs>::value && sw::_IsTypedRoutedEventArgs<TEventArgs>::value>::type
        {
            if (handler) this->AddHandler(TEventArgs::EventType, RoutedEventHandlerWrapper<TEventArgs>(Action<UIElement &, TEventArgs &>(obj, handler)));
        }

        /**
         * @brief 添加路由事件处理函数
         * @tparam TEventArgs 路由事件的参数类型，必须继承自RoutedEventArgs
         * @param eventType 路由事件类型
         * @param handler 处理函数
         */
        template <typename TEventArgs>
        auto AddHandler(RoutedEventType eventType, const Action<UIElement &, TEventArgs &> &handler)
            -> typename std::enable_if<
                std::is_base_of<RoutedEventArgs, TEventArgs>::value &&
                !std::is_same<TEventArgs, RoutedEventArgs>::value &&
                !sw::_IsTypedRoutedEventArgs<TEventArgs>::value>::type
        {
            if (handler) this->AddHandler(eventType, RoutedEventHandlerWrapper<TEventArgs>(handler));
        }

        /**
         * @brief 添加成员函数作为路由事件处理函数
         * @tparam TEventArgs 路由事件的参数类型，必须继承自RoutedEventArgs
         * @tparam THandleObj 成员函数所在的类
         * @param eventType 路由事件类型
         * @param obj 注册的成员函数所在的对象
         * @param handler 处理函数
         */
        template <typename TEventArgs, typename THandleObj>
        auto AddHandler(RoutedEventType eventType, THandleObj &obj, void (THandleObj::*handler)(UIElement &, TEventArgs &))
            -> typename std::enable_if<
                std::is_base_of<RoutedEventArgs, TEventArgs>::value &&
                !std::is_same<TEventArgs, RoutedEventArgs>::value &&
                !sw::_IsTypedRoutedEventArgs<TEventArgs>::value>::type
        {
            if (handler) this->AddHandler(eventType, RoutedEventHandlerWrapper<TEventArgs>(Action<UIElement &, TEventArgs &>(obj, handler)));
        }

        /**
         * @brief 移除已添加的类型为成员函数的路由事件处理函数
         * @tparam T 成员函数所在的类
         * @param eventType 路由事件类型
         * @param obj 注册的成员函数所在的对象
         * @param handler 处理函数
         * @return 是否成功移除
         */
        template <typename T>
        bool RemoveHandler(RoutedEventType eventType, T &obj, void (T::*handler)(UIElement &, RoutedEventArgs &))
        {
            return handler == nullptr ? false : this->_eventMap[eventType].Remove(obj, handler);
        }

        /**
         * @brief 移除已添加的路由事件处理函数
         * @tparam TEventArgs 路由事件的参数类型，必须继承自TypedRoutedEventArgs<...>
         * @param handler 事件的处理函数
         * @return 是否成功移除
         */
        template <typename TEventArgs>
        auto RemoveHandler(const Action<UIElement &, TEventArgs &> &handler)
            -> typename std::enable_if<std::is_base_of<RoutedEventArgs, TEventArgs>::value && sw::_IsTypedRoutedEventArgs<TEventArgs>::value, bool>::type
        {
            if (handler == nullptr) {
                return false;
            } else {
                return this->RemoveHandler(TEventArgs::EventType, RoutedEventHandlerWrapper<TEventArgs>(handler));
            }
        }

        /**
         * @brief 移除已添加的类型为成员函数的路由事件处理函数
         * @tparam TEventArgs 路由事件的参数类型，必须继承自TypedRoutedEventArgs<...>
         * @tparam THandleObj 成员函数所在的类
         * @param obj 注册的成员函数所在的对象
         * @param handler 事件的处理函数
         * @return 是否成功移除
         */
        template <typename TEventArgs, typename THandleObj>
        auto RemoveHandler(THandleObj &obj, void (THandleObj::*handler)(UIElement &, TEventArgs &))
            -> typename std::enable_if<std::is_base_of<RoutedEventArgs, TEventArgs>::value && sw::_IsTypedRoutedEventArgs<TEventArgs>::value, bool>::type
        {
            if (handler == nullptr) {
                return false;
            } else {
                return this->RemoveHandler(TEventArgs::EventType, RoutedEventHandlerWrapper<TEventArgs>(Action<UIElement &, TEventArgs &>(obj, handler)));
            }
        }

        /**
         * @brief 移除已添加的路由事件处理函数
         * @tparam TEventArgs 路由事件的参数类型，必须继承自RoutedEventArgs
         * @param eventType 路由事件类型
         * @param handler 处理函数
         * @return 是否成功移除
         */
        template <typename TEventArgs>
        auto RemoveHandler(RoutedEventType eventType, const Action<UIElement &, TEventArgs &> &handler)
            -> typename std::enable_if<
                std::is_base_of<RoutedEventArgs, TEventArgs>::value &&
                    !std::is_same<TEventArgs, RoutedEventArgs>::value &&
                    !sw::_IsTypedRoutedEventArgs<TEventArgs>::value,
                bool>::type
        {
            if (handler == nullptr) {
                return false;
            } else {
                return this->RemoveHandler(eventType, RoutedEventHandlerWrapper<TEventArgs>(handler));
            }
        }

        /**
         * @brief 移除已添加的类型为成员函数的路由事件处理函数
         * @tparam TEventArgs 路由事件的参数类型，必须继承自RoutedEventArgs
         * @tparam THandleObj 成员函数所在的类
         * @param eventType 路由事件类型
         * @param obj 注册的成员函数所在的对象
         * @param handler 处理函数
         * @return 是否成功移除
         */
        template <typename TEventArgs, typename THandleObj>
        auto RemoveHandler(RoutedEventType eventType, THandleObj &obj, void (THandleObj::*handler)(UIElement &, TEventArgs &))
            -> typename std::enable_if<
                std::is_base_of<RoutedEventArgs, TEventArgs>::value &&
                    !std::is_same<TEventArgs, RoutedEventArgs>::value &&
                    !sw::_IsTypedRoutedEventArgs<TEventArgs>::value,
                bool>::type
        {
            if (handler == nullptr) {
                return false;
            } else {
                return this->RemoveHandler(eventType, RoutedEventHandlerWrapper<TEventArgs>(Action<UIElement &, TEventArgs &>(obj, handler)));
            }
        }
    };

    /**
     * @brief _CanAddChild模板偏特化，当AddChild有对应类型的重载时该偏特化生效
     */
    template <typename T>
    struct UIElement::_CanAddChild<
        T, decltype(void(std::declval<UIElement>().AddChild(std::declval<T>())))> : std::true_type {
    };
}

// Control.h


namespace sw
{
    /**
     * @brief 控件
     */
    class Control : public UIElement
    {
    public:
        /**
         * @brief 控件的标识符
         */
        const ReadOnlyProperty<int> ControlId;

        /**
         * @brief 控件是否在窗口的控件层次结构中
         * @note 当控件已创建并且被添加到任意父窗口（可以是其他框架窗口）时该值为true
         */
        const ReadOnlyProperty<bool> IsInHierarchy;

    public:
        /**
         * @brief 初始化控件
         */
        Control();

        /**
         * @brief 析构函数，这里用纯虚函数使该类成为抽象类
         */
        virtual ~Control() = 0;

    protected:
        /**
         * @brief 销毁控件句柄并重新初始化，该操作会创建新的句柄并设置样式、文本、字体等
         * @param lpParam 创建控件句柄时传给CreateWindowExW的参数
         * @return 若函数成功则返回true，否则返回false
         * @note 该函数只能在创建控件的线程调用
         */
        bool ResetHandle(LPVOID lpParam = NULL);

        /**
         * @brief 销毁控件句柄并重新初始化，并修改样式，该操作会创建新的句柄并设置样式、文本、字体等
         * @param style 新的样式
         * @param exStyle 新的扩展样式
         * @param lpParam 创建控件句柄时传给CreateWindowExW的参数
         * @return 若函数成功则返回true，否则返回false
         * @note 该函数只能在创建控件的线程调用
         */
        bool ResetHandle(DWORD style, DWORD exStyle, LPVOID lpParam = NULL);

    protected:
        /**
         * @brief 父窗口接收到WM_NOTIFY后且父窗口OnNotify函数返回false时调用发出通知控件的该函数
         * @param pNMHDR 包含有关通知消息的信息
         * @param result 函数返回值为true时将该值作为消息的返回值
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnNotified(NMHDR *pNMHDR, LRESULT &result) override;

        /**
         * @brief 接收到NM_CUSTOMDRAW后调用该函数
         * @param pNMCD 包含有关自定义绘制的信息
         * @param result 函数返回值为true时将该值作为消息的返回值
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnCustomDraw(NMCUSTOMDRAW *pNMCD, LRESULT &result);

        /**
         * @brief 擦除背景前调用该函数
         * @param pNMCD 包含有关自定义绘制的信息
         * @param result 函数返回值为true时将该值作为NM_CUSTOMDRAW消息的返回值
         * @return 若已完成绘制则返回true，否则返回false以使用默认绘制
         */
        virtual bool OnPreErase(NMCUSTOMDRAW *pNMCD, LRESULT &result);

        /**
         * @brief 擦除背景后调用该函数
         * @param pNMCD 包含有关自定义绘制的信息
         * @param result 函数返回值为true时将该值作为NM_CUSTOMDRAW消息的返回值
         * @return 若已完成绘制则返回true，否则返回false以使用默认绘制
         */
        virtual bool OnPostErase(NMCUSTOMDRAW *pNMCD, LRESULT &result);

        /**
         * @brief 绘制控件前调用该函数
         * @param pNMCD 包含有关自定义绘制的信息
         * @param result 函数返回值为true时将该值作为NM_CUSTOMDRAW消息的返回值
         * @return 若已完成绘制则返回true，否则返回false以使用默认绘制
         */
        virtual bool OnPrePaint(NMCUSTOMDRAW *pNMCD, LRESULT &result);

        /**
         * @brief 绘制控件后调用该函数
         * @param pNMCD 包含有关自定义绘制的信息
         * @param result 函数返回值为true时将该值作为NM_CUSTOMDRAW消息的返回值
         * @return 若已完成绘制则返回true，否则返回false以使用默认绘制
         */
        virtual bool OnPostPaint(NMCUSTOMDRAW *pNMCD, LRESULT &result);

        /**
         * @brief 绘制子项前调用该函数
         * @param pNMCD 包含有关自定义绘制的信息
         * @param subItem 若消息包含CDDS_SUBITEM标志则该值为true，否则为false
         * @param result 函数返回值为true时将该值作为NM_CUSTOMDRAW消息的返回值
         * @return 若已完成绘制则返回true，否则返回false以使用默认绘制
         */
        virtual bool OnItemPrePaint(NMCUSTOMDRAW *pNMCD, bool subItem, LRESULT &result);

        /**
         * @brief 绘制子项后调用该函数
         * @param pNMCD 包含有关自定义绘制的信息
         * @param subItem 若消息包含CDDS_SUBITEM标志则该值为true，否则为false
         * @param result 函数返回值为true时将该值作为NM_CUSTOMDRAW消息的返回值
         * @return 若已完成绘制则返回true，否则返回false以使用默认绘制
         */
        virtual bool OnItemPostPaint(NMCUSTOMDRAW *pNMCD, bool subItem, LRESULT &result);

        /**
         * @brief 控件句柄发生改变时调用该函数
         * @param hwnd 新的控件句柄
         */
        virtual void OnHandleChanged(HWND hwnd);
    };
}

// HwndWrapper.h


namespace sw
{
    /**
     * @brief 将Win32 window包装为SimpleWindow对象
     * @note 与HwndHost类似，不同的是HwndWrapper不会创建容器窗口并且会接管句柄的窗口过程函数
     */
    class HwndWrapper : public UIElement
    {
    protected:
        /**
         * @brief 子类需要调用该函数以初始化HwndWrapper，该函数会调用BuildWindowCore
         */
        void InitHwndWrapper();

        /**
         * @brief 初始化HwndWrapper时会调用该函数，需在该函数中创建并返回要被托管的窗口句柄
         * @param isControl 若被托管的窗口句柄是一个控件需要将该值置为true
         * @param controlId 当isControl被设为true时，该值提供一个可用的控件id，确保不会与其他控件的id相重复
         * @return 被托管窗口句柄
         */
        virtual HWND BuildWindowCore(bool &isControl, int controlId) = 0;
    };
}

// Layer.h


namespace sw
{
    template <typename TBase, typename = void>
    class Layer; // 向前声明

    /**
     * @brief 表示可以设置布局方式的元素类型，如窗口、面板等
     */
    template <typename TBase>
    class Layer<TBase,
                typename std::enable_if<std::is_base_of<UIElement, TBase>::value>::type>
        : public TBase
    {
    private:
        /**
         * @brief 滚动条滚动一行的距离
         */
        static constexpr int _LayerScrollBarLineInterval = 20;

    private:
        /**
         * @brief 是否按照布局方式与子元素自动调整尺寸
         */
        bool _autoSize = true;

        /**
         * @brief 指向所自定义的布局方式对象的指针
         */
        LayoutHost *_customLayout = nullptr;

        /**
         * @brief 记录水平滚动条是否已被禁止
         */
        bool _horizontalScrollDisabled = true;

        /**
         * @brief 记录垂直滚动条是否已被禁止
         */
        bool _verticalScrollDisabled = true;

        /**
         * @brief 是否在鼠标滚动时滚动内容
         */
        bool _mouseWheelScrollEnabled = true;

    public:
        /**
         * @brief 自定义的布局方式，赋值后将自动与所指向的布局关联，每个布局只能关联一个对象，设为nullptr可恢复默认布局
         */
        const Property<LayoutHost *> Layout{
            Property<LayoutHost *>::Init(this)
                .Getter([](Layer *self) -> LayoutHost * {
                    return self->_customLayout;
                })
                .Setter([](Layer *self, LayoutHost *value) {
                    if (value != nullptr)
                        value->Associate(self);
                    self->_customLayout = value;
                    self->InvalidateMeasure();
                })};

        /**
         * @brief 是否按照布局方式与子元素自动调整尺寸，该属性仅在当前元素已设置布局方式并且非顶级元素时有效
         */
        const Property<bool> AutoSize{
            Property<bool>::Init(this)
                .Getter([](Layer *self) -> bool {
                    return self->_autoSize;
                })
                .Setter([](Layer *self, bool value) {
                    if (self->_autoSize != value) {
                        self->_autoSize = value;
                        self->InvalidateMeasure();
                    }
                })};

        /**
         * @brief 是否显示横向滚动条
         */
        const Property<bool> HorizontalScrollBar{
            Property<bool>::Init(this)
                .Getter([](Layer *self) -> bool {
                    return self->GetStyle(WS_HSCROLL);
                })
                .Setter([](Layer *self, bool value) {
                    if (self->HorizontalScrollBar == value) {
                        return;
                    }
                    if (value) {
                        ShowScrollBar(self->Handle, SB_HORZ, value);
                        self->HorizontalScrollPos = self->HorizontalScrollPos;
                    } else {
                        self->HorizontalScrollPos = 0;
                        ShowScrollBar(self->Handle, SB_HORZ, value);
                    }
                })};

        /**
         * @brief 是否显示纵向滚动条
         */
        const Property<bool> VerticalScrollBar{
            Property<bool>::Init(this)
                .Getter([](Layer *self) -> bool {
                    return self->GetStyle(WS_VSCROLL);
                })
                .Setter([](Layer *self, bool value) {
                    if (self->VerticalScrollBar == value) {
                        return;
                    }
                    if (value) {
                        ShowScrollBar(self->Handle, SB_VERT, value);
                        self->VerticalScrollPos = self->VerticalScrollPos;
                    } else {
                        self->VerticalScrollPos = 0;
                        ShowScrollBar(self->Handle, SB_VERT, value);
                    }
                })};

        /**
         * @brief 横向滚动条位置
         */
        const Property<double> HorizontalScrollPos{
            Property<double>::Init(this)
                .Getter([](Layer *self) -> double {
                    SCROLLINFO info{};
                    info.cbSize = sizeof(info);
                    info.fMask  = SIF_POS;
                    GetScrollInfo(self->Handle, SB_HORZ, &info);
                    return Dip::PxToDipX(info.nPos);
                })
                .Setter([](Layer *self, double value) {
                    SCROLLINFO info{};
                    info.cbSize = sizeof(info);
                    info.fMask  = SIF_POS;
                    info.nPos   = Dip::DipToPxX(value);
                    SetScrollInfo(self->Handle, SB_HORZ, &info, true);

                    LayoutHost *layout = self->_GetLayout();

                    if (layout != nullptr && !self->_horizontalScrollDisabled && self->HorizontalScrollBar) {
                        self->GetInternalArrangeOffsetX() = -self->HorizontalScrollPos;
                        self->_MeasureAndArrangeWithoutResize(*layout, self->ClientRect->GetSize());
                    }
                })};

        /**
         * @brief 纵向滚动条位置
         */
        const Property<double> VerticalScrollPos{
            Property<double>::Init(this)
                .Getter([](Layer *self) -> double {
                    SCROLLINFO info{};
                    info.cbSize = sizeof(info);
                    info.fMask  = SIF_POS;
                    GetScrollInfo(self->Handle, SB_VERT, &info);
                    return Dip::PxToDipY(info.nPos);
                })
                .Setter(
                    [](Layer *self, double value) {
                        SCROLLINFO info{};
                        info.cbSize = sizeof(info);
                        info.fMask  = SIF_POS;
                        info.nPos   = Dip::DipToPxY(value);
                        SetScrollInfo(self->Handle, SB_VERT, &info, true);

                        LayoutHost *layout = self->_GetLayout();

                        if (layout != nullptr && !self->_verticalScrollDisabled && self->VerticalScrollBar) {
                            self->GetInternalArrangeOffsetY() = -self->VerticalScrollPos;
                            self->_MeasureAndArrangeWithoutResize(*layout, self->ClientRect->GetSize());
                        }
                    })};

        /**
         * @brief 横向滚动条可设置的最大位置
         */
        const ReadOnlyProperty<double> HorizontalScrollLimit{
            Property<double>::Init(this)
                .Getter([](Layer *self) -> double {
                    if (self->_horizontalScrollDisabled) {
                        return 0;
                    }
                    SCROLLINFO info{};
                    info.cbSize = sizeof(info);
                    info.fMask  = SIF_RANGE | SIF_PAGE;
                    GetScrollInfo(self->Handle, SB_HORZ, &info);
                    return Dip::PxToDipX(info.nMax - info.nPage + 1);
                })};

        /**
         * @brief 纵向滚动条可设置的最大位置
         */
        const ReadOnlyProperty<double> VerticalScrollLimit{
            Property<double>::Init(this)
                .Getter([](Layer *self) -> double {
                    if (self->_verticalScrollDisabled) {
                        return 0;
                    }
                    SCROLLINFO info{};
                    info.cbSize = sizeof(info);
                    info.fMask  = SIF_RANGE | SIF_PAGE;
                    GetScrollInfo(self->Handle, SB_VERT, &info);
                    return Dip::PxToDipY(info.nMax - info.nPage + 1);
                })};

        /**
         * @brief 是否在鼠标滚动时滚动内容
         */
        const Property<bool> MouseWheelScrollEnabled{
            Property<bool>::Init(this)
                .Getter([](Layer *self) -> bool {
                    return self->_mouseWheelScrollEnabled;
                })
                .Setter([](Layer *self, bool value) {
                    self->_mouseWheelScrollEnabled = value;
                })};

    protected:
        /**
         * @brief 更新布局
         */
        void UpdateLayout()
        {
            LayoutHost *layout = _GetLayout();

            if (layout == nullptr) {
                _MeasureAndArrangeWithoutLayout();
            } else {
                _MeasureAndArrangeWithoutResize(*layout, this->ClientRect->GetSize());
            }

            UpdateScrollRange();
            // Redraw();
        }

        /**
         * @brief 获取默认布局对象
         */
        virtual LayoutHost *GetDefaultLayout()
        {
            return nullptr;
        }

        /**
         * @brief 触发滚动条相关事件时调用该函数
         * @param scrollbar 滚动条类型
         * @param event 滚动条事件
         * @param pos 当event为ThumbPosition或ThubmTrack时表示当前滚动条位置，其他情况固定为0
         */
        virtual void OnScroll(ScrollOrientation scrollbar, ScrollEvent event, double pos)
        {
            ScrollingEventArgs args(scrollbar, event, pos);
            this->RaiseRoutedEvent(args);

            if (args.cancel) {
                return;
            }

            if (scrollbar == ScrollOrientation::Horizontal) {
                // 水平滚动条
                switch (event) {
                    case ScrollEvent::ThubmTrack: {
                        HorizontalScrollPos = pos;
                        break;
                    }
                    case ScrollEvent::Left: {
                        ScrollToLeft();
                        break;
                    }
                    case ScrollEvent::Right: {
                        ScrollToRight();
                        break;
                    }
                    case ScrollEvent::PageLeft: {
                        ScrollHorizontal(-GetHorizontalScrollPageSize());
                        break;
                    }
                    case ScrollEvent::PageRight: {
                        ScrollHorizontal(GetHorizontalScrollPageSize());
                        break;
                    }
                    case ScrollEvent::LineLeft: {
                        ScrollHorizontal(-_LayerScrollBarLineInterval);
                        break;
                    }
                    case ScrollEvent::LineRight: {
                        ScrollHorizontal(_LayerScrollBarLineInterval);
                        break;
                    }
                    default: {
                        break;
                    }
                }
            } else {
                // 垂直滚动条
                switch (event) {
                    case ScrollEvent::ThubmTrack: {
                        VerticalScrollPos = pos;
                        break;
                    }
                    case ScrollEvent::Bottom: {
                        ScrollToBottom();
                        break;
                    }
                    case ScrollEvent::Top: {
                        ScrollToTop();
                        break;
                    }
                    case ScrollEvent::PageUp: {
                        ScrollVertical(-GetVerticalScrollPageSize());
                        break;
                    }
                    case ScrollEvent::PageDown: {
                        ScrollVertical(GetVerticalScrollPageSize());
                        break;
                    }
                    case ScrollEvent::LineUp: {
                        ScrollVertical(-_LayerScrollBarLineInterval);
                        break;
                    }
                    case ScrollEvent::LineDown: {
                        ScrollVertical(_LayerScrollBarLineInterval);
                        break;
                    }
                    default: {
                        break;
                    }
                }
            }
        }

        /**
         * @brief 接收到WM_VSCROLL时调用目标控件的该函数
         * @param event 事件类型，即消息wParam的低字
         * @param pos 当前滚动条的位置，仅当event为SB_THUMBPOSITION或SB_THUMBTRACK时有效
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnVerticalScroll(int event, int pos) override
        {
            OnScroll(ScrollOrientation::Vertical, (ScrollEvent)event,
                     (event == SB_THUMBTRACK || event == SB_THUMBPOSITION) ? Dip::PxToDipY(pos) : (0.0));
            return true;
        }

        /**
         * @brief 接收到WM_HSCROLL时调用目标控件的该函数
         * @param event 事件类型，即消息wParam的低字
         * @param pos 当前滚动条的位置，仅当event为SB_THUMBPOSITION或SB_THUMBTRACK时有效
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnHorizontalScroll(int event, int pos) override
        {
            OnScroll(ScrollOrientation::Horizontal, (ScrollEvent)event,
                     (event == SB_THUMBTRACK || event == SB_THUMBPOSITION) ? Dip::PxToDipX(pos) : (0.0));
            return true;
        }

        /**
         * @brief 安排元素位置
         * @param finalSize 最终元素所安排的位置
         */
        virtual void Arrange(const sw::Rect &finalPosition) override
        {
            TBase::Arrange(finalPosition);
            UpdateScrollRange();
        }

        /**
         * @brief 测量元素所需尺寸，无需考虑边框和边距
         * @param availableSize 可用的尺寸
         * @return 返回元素需要占用的尺寸
         */
        virtual Size MeasureOverride(const Size &availableSize) override
        {
            LayoutHost *layout = _GetLayout();

            // 未设置布局时无法使用自动尺寸
            // 若未使用自动尺寸，则按照普通元素measure
            if (layout == nullptr || !_autoSize) {
                return TBase::MeasureOverride(availableSize);
            }

            return layout->MeasureOverride(availableSize);
        }

        /**
         * @brief 安排子元素的位置，可重写该函数以实现自定义布局
         * @param finalSize 可用于排列子元素的最终尺寸
         */
        virtual void ArrangeOverride(const Size &finalSize) override
        {
            LayoutHost *layout = _GetLayout();

            if (layout == nullptr) {
                // 未设置布局方式，此时需要对子元素进行Measure和Arrange
                _MeasureAndArrangeWithoutLayout();
            } else if (!_autoSize) {
                // 已设置布局方式，但是AutoSize被取消，此时子元素也未Measure
                _MeasureAndArrangeWithoutResize(*layout, finalSize);
            } else {
                // 已设置布局方式且AutoSize为true，此时子元素已Measure，调用Arrange即可
                layout->ArrangeOverride(finalSize);
            }
        }

        /**
         * @brief 尝试将指定的矩形区域移动到可视区域内
         * @param screenRect 要移动到可视区域的矩形在屏幕中的位置
         * @return 若已处理该请求则返回true，否则返回false以继续向上冒泡
         */
        virtual bool RequestBringIntoView(const sw::Rect &screenRect) override
        {
            bool handled = false;

            sw::Point pos = this->PointFromScreen(screenRect.GetPos());
            sw::Rect rect = {pos.x, pos.y, screenRect.width, screenRect.height};

            sw::Rect clientRect = this->ClientRect;

            if (VerticalScrollBar) {
                double curPos = VerticalScrollPos;
                if (rect.top < curPos) {
                    VerticalScrollPos = rect.top;
                } else if (rect.top + rect.height > curPos + clientRect.height) {
                    if (rect.height >= clientRect.height) {
                        VerticalScrollPos = rect.top;
                    } else {
                        VerticalScrollPos = rect.top + rect.height - clientRect.height;
                    }
                }
                handled = true;
            }

            if (HorizontalScrollBar) {
                double curPos = HorizontalScrollPos;
                if (rect.left < curPos) {
                    HorizontalScrollPos = rect.left;
                } else if (rect.left + rect.width > curPos + clientRect.width) {
                    if (rect.width >= clientRect.width) {
                        HorizontalScrollPos = rect.left;
                    } else {
                        HorizontalScrollPos = rect.left + rect.width - clientRect.width;
                    }
                }
                handled = true;
            }

            return handled;
        }

        /**
         * @brief 路由事件经过当前元素时调用该函数
         * @param eventArgs 事件参数
         * @param handler 事件处理函数，值为空时表示当前元素没有注册该事件处理函数
         */
        virtual void OnRoutedEvent(RoutedEventArgs &eventArgs, const RoutedEventHandler &handler) override
        {
            TBase::OnRoutedEvent(eventArgs, handler);

            if (!eventArgs.handled &&
                eventArgs.eventType == UIElement_MouseWheel && _mouseWheelScrollEnabled) //
            {
                auto &wheelArgs = static_cast<MouseWheelEventArgs &>(eventArgs);
                bool shiftDown  = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
                double offset   = -std::copysign(_LayerScrollBarLineInterval, wheelArgs.wheelDelta);
                if (shiftDown) {
                    if (HorizontalScrollBar) {
                        ScrollHorizontal(offset);
                        eventArgs.handled = true;
                    }
                } else {
                    if (VerticalScrollBar) {
                        ScrollVertical(offset);
                        eventArgs.handled = true;
                    }
                }
            }
        }

    public:
        /**
         * @brief 获取横向滚动条的范围
         * @param refMin 滚动范围最小值
         * @param refMax 滚动范围最大值
         */
        void GetHorizontalScrollRange(double &refMin, double &refMax)
        {
            INT nMin = 0, nMax = 0;
            GetScrollRange(this->Handle, SB_HORZ, &nMin, &nMax);

            refMin = Dip::PxToDipX(nMin);
            refMax = Dip::PxToDipX(nMax);
        }

        /**
         * @brief 获取纵向滚动条的范围
         * @param refMin 滚动范围最小值
         * @param refMax 滚动范围最大值
         */
        void GetVerticalScrollRange(double &refMin, double &refMax)
        {
            INT nMin = 0, nMax = 0;
            GetScrollRange(this->Handle, SB_VERT, &nMin, &nMax);

            refMin = Dip::PxToDipY(nMin);
            refMax = Dip::PxToDipY(nMax);
        }

        /**
         * @brief 设置横向滚动条的范围
         * @param min 滚动范围最小值
         * @param max 滚动范围最大值
         */
        void SetHorizontalScrollRange(double min, double max)
        {
            SCROLLINFO info{};
            info.cbSize = sizeof(info);
            info.fMask  = SIF_RANGE | SIF_PAGE;
            info.nMin   = Dip::DipToPxX(min);
            info.nMax   = Dip::DipToPxX(max);
            info.nPage  = Dip::DipToPxX(this->ClientWidth);

            SetScrollInfo(this->Handle, SB_HORZ, &info, true);
        }

        /**
         * @brief 设置纵向滚动条的范围
         * @param min 滚动范围最小值
         * @param max 滚动范围最大值
         */
        void SetVerticalScrollRange(double min, double max)
        {
            SCROLLINFO info{};
            info.cbSize = sizeof(info);
            info.fMask  = SIF_RANGE | SIF_PAGE;
            info.nMin   = Dip::DipToPxY(min);
            info.nMax   = Dip::DipToPxY(max);
            info.nPage  = Dip::DipToPxY(this->ClientHeight);

            SetScrollInfo(this->Handle, SB_VERT, &info, true);
        }

        /**
         * @brief 获取水平滚动条滚动页面大小
         */
        double GetHorizontalScrollPageSize()
        {
            SCROLLINFO info{};
            info.cbSize = sizeof(info);
            info.fMask  = SIF_PAGE;
            GetScrollInfo(this->Handle, SB_HORZ, &info);
            return Dip::PxToDipX(info.nPage);
        }

        /**
         * @brief 获取垂直滚动条滚动页面大小
         */
        double GetVerticalScrollPageSize()
        {
            SCROLLINFO info{};
            info.cbSize = sizeof(info);
            info.fMask  = SIF_PAGE;
            GetScrollInfo(this->Handle, SB_VERT, &info);
            return Dip::PxToDipY(info.nPage);
        }

        /**
         * @brief 设置水平滚动条滚动页面大小
         */
        void SetHorizontalScrollPageSize(double pageSize)
        {
            SCROLLINFO info{};
            info.cbSize = sizeof(info);
            info.fMask  = SIF_PAGE;
            info.nPage  = Dip::DipToPxX(pageSize);
            SetScrollInfo(this->Handle, SB_HORZ, &info, true);
        }

        /**
         * @brief 设置垂直滚动条滚动页面大小
         */
        void SetVerticalScrollPageSize(double pageSize)
        {
            SCROLLINFO info{};
            info.cbSize = sizeof(info);
            info.fMask  = SIF_PAGE;
            info.nPage  = Dip::DipToPxY(pageSize);
            SetScrollInfo(this->Handle, SB_VERT, &info, true);
        }

        /**
         * @brief 根据子元素更新滚动条范围，未设定布局方式时该函数无效
         */
        void UpdateScrollRange()
        {
            if (_GetLayout() == nullptr) {
                // 当未设置布局方式时滚动条和控件位置需要手动设置
                // 将以下俩字段设为false确保xxxScrollLimit属性在未设置布局方式时仍可用
                _horizontalScrollDisabled = false;
                _verticalScrollDisabled   = false;
                return;
            }

            if (HorizontalScrollBar) {
                double childRightmost = this->GetChildRightmost(true);

                if (int(childRightmost - this->ClientWidth) > 0) {
                    _horizontalScrollDisabled = false;
                    EnableScrollBar(this->Handle, SB_HORZ, ESB_ENABLE_BOTH);
                    SetHorizontalScrollRange(0, childRightmost);

                    // 当尺寸改变时确保子元素位置与滚动条同步
                    double pos = HorizontalScrollPos;
                    if (-this->GetInternalArrangeOffsetX() > pos) {
                        HorizontalScrollPos = pos;
                    }

                } else {
                    HorizontalScrollPos = 0;
                    EnableScrollBar(this->Handle, SB_HORZ, ESB_DISABLE_BOTH);
                    _horizontalScrollDisabled = true;
                }
            }

            if (VerticalScrollBar) {
                double childBottommost = this->GetChildBottommost(true);

                if (int(childBottommost - this->ClientHeight) > 0) {
                    _verticalScrollDisabled = false;
                    EnableScrollBar(this->Handle, SB_VERT, ESB_ENABLE_BOTH);
                    SetVerticalScrollRange(0, childBottommost);

                    // 当尺寸改变时确保子元素位置与滚动条同步
                    double pos = VerticalScrollPos;
                    if (-this->GetInternalArrangeOffsetY() > pos) {
                        VerticalScrollPos = pos;
                    }

                } else {
                    VerticalScrollPos = 0;
                    EnableScrollBar(this->Handle, SB_VERT, ESB_DISABLE_BOTH);
                    _verticalScrollDisabled = true;
                }
            }
        }

        /**
         * @brief 将垂直滚动条移动到顶部
         */
        void ScrollToTop()
        {
            VerticalScrollPos = 0;
        }

        /**
         * @brief 将垂直滚动条移动到底部
         */
        void ScrollToBottom()
        {
            VerticalScrollPos = VerticalScrollLimit;
        }

        /**
         * @brief 将水平滚动条移动到最左
         */
        void ScrollToLeft()
        {
            HorizontalScrollPos = 0;
        }

        /**
         * @brief 将水平滚动条移动到最右
         */
        void ScrollToRight()
        {
            HorizontalScrollPos = HorizontalScrollLimit;
        }

        /**
         * @brief 水平滚动
         * @param offset 滚动的偏移量
         */
        void ScrollHorizontal(double offset)
        {
            HorizontalScrollPos += offset;
        }

        /**
         * @brief 垂直滚动
         * @param offset 滚动的偏移量
         */
        void ScrollVertical(double offset)
        {
            VerticalScrollPos += offset;
        }

    private:
        /**
         * @brief 获取布局对象，若Layout属性被赋值则返回设置的对象，否则返回默认布局对象
         */
        LayoutHost *_GetLayout()
        {

            auto layout = (_customLayout != nullptr) ? _customLayout : GetDefaultLayout();
            return (layout != nullptr && layout->IsAssociated(this)) ? layout : nullptr;
        }

        /**
         * @brief 在没有设定布局方式时，使用该函数对子元素Measure和Arrange
         */
        void _MeasureAndArrangeWithoutLayout()
        {
            this->GetInternalArrangeOffsetX() = 0;
            this->GetInternalArrangeOffsetY() = 0;

            int childCount = this->GetChildLayoutCount();

            for (int i = 0; i < childCount; ++i) {
                // measure
                UIElement &item = this->GetChildLayoutAt(i);
                item.Measure(Size{INFINITY, INFINITY});
                // arrange
                Size desireSize      = item.GetDesireSize();
                sw::Rect itemRect    = item.Rect;
                Thickness itemMargin = item.Margin;
                item.Arrange(sw::Rect{itemRect.left - itemMargin.left, itemRect.top - itemMargin.top, desireSize.width, desireSize.height});
            }
        }

        /**
         * @brief 使用设定的布局方式对子元素进行Measure和Arrange，不改变当前的尺寸和DesireSize
         */
        void _MeasureAndArrangeWithoutResize(LayoutHost &layout, const Size &clientSize)
        {
            if (layout.IsAssociated(this)) {
                layout.MeasureOverride(clientSize);
                layout.ArrangeOverride(clientSize);
            }
        }
    };

    /*================================================================================*/

    extern template class Layer<UIElement>;
}

// NotifyIcon.h


namespace sw
{
    class NotifyIcon; // 前向声明

    /**
     * @brief 通知图标鼠标事件参数
     */
    struct NotifyIconMouseEventArgs : EventArgs {
        /**
         * @brief 鼠标位置
         */
        Point mousePosition;

        /**
         * @brief 是否已处理该事件，默认为false
         */
        bool handled = false;
    };

    /**
     * @brief 通知图标鼠标事件处理函数类型
     */
    using NotifyIconMouseEventHandler =
        EventHandler<NotifyIcon, NotifyIconMouseEventArgs>;

    /**
     * @brief 系统托盘通知图标
     */
    class NotifyIcon : public WndBase
    {
    private:
        /**
         * @brief 基类
         */
        using TBase = WndBase;

        /**
         * @brief 通知图标数据
         */
        NOTIFYICONDATAW _nid{};

        /**
         * @brief 右键菜单
         */
        sw::ContextMenu *_contextMenu = nullptr;

        /**
         * @brief 图标单击事件委托
         */
        NotifyIconMouseEventHandler _clicked;

        /**
         * @brief 图标双击事件委托
         */
        NotifyIconMouseEventHandler _doubleClicked;

        /**
         * @brief 正在打开上下文菜单事件委托
         */
        NotifyIconMouseEventHandler _contextMenuOpening;

    public:
        /**
         * @brief 当图标被单击时触发该事件
         */
        const Event<NotifyIconMouseEventHandler> Clicked;

        /**
         * @brief 当图标被双击时触发该事件
         */
        const Event<NotifyIconMouseEventHandler> DoubleClicked;

        /**
         * @brief 打开上下文菜单前触发该事件
         */
        const Event<NotifyIconMouseEventHandler> ContextMenuOpening;

        /**
         * @brief 图标
         */
        const Property<HICON> Icon;

        /**
         * @brief 图标的提示文本
         */
        const Property<std::wstring> ToolTip;

        /**
         * @brief 图标是否可见
         */
        const Property<bool> Visible;

        /**
         * @brief 右键菜单
         */
        const Property<sw::ContextMenu *> ContextMenu;

        /**
         * @brief 图标在屏幕上的位置和尺寸
         */
        const ReadOnlyProperty<sw::Rect> Rect;

    public:
        /**
         * @brief 初始化通知图标
         */
        NotifyIcon();

        /**
         * @brief 析构函数
         */
        ~NotifyIcon();

    protected:
        /**
         * @brief 对WndProc的封装
         */
        virtual LRESULT WndProc(ProcMsg &refMsg) override;

        /**
         * @brief 当WM_COMMAND接收到菜单命令时调用该函数
         * @param id 菜单id
         */
        virtual void OnMenuCommand(int id) override;

        /**
         * @brief 处理通知图标消息
         */
        virtual void OnNotyfyIconMessage(WPARAM wParam, LPARAM lParam);

        /**
         * @brief 鼠标单击图标时调用该函数
         * @param mousePos 鼠标位置
         */
        virtual void OnClicked(const Point &mousePos);

        /**
         * @brief 鼠标双击图标时调用该函数
         * @param mousePos 鼠标位置
         */
        virtual void OnDoubleClicked(const Point &mousePos);

        /**
         * @brief 打开上下文菜单前调用该函数
         * @param mousePos 鼠标位置
         */
        virtual void OnContextMenuOpening(const Point &mousePos);

        /**
         * @brief 获取通知图标数据
         */
        NOTIFYICONDATAW &GetNotifyIconData();

    public:
        /**
         * @brief 显示通知图标
         */
        bool Show();

        /**
         * @brief 隐藏通知图标
         */
        bool Hide();

        /**
         * @brief 销毁通知图标
         * @note 调用该函数后不应继续使用当前对象
         */
        void Destroy();

    private:
        /**
         * @brief 调用Shell_NotifyIcon函数
         */
        bool _ShellNotifyIcon(DWORD dwMessage);

        /**
         * @brief 添加图标
         */
        bool _AddIcon();

        /**
         * @brief 删除图标
         */
        bool _DeleteIcon();

        /**
         * @brief 修改图标
         */
        bool _ModifyIcon();

        /**
         * @brief 设置图标状态
         */
        bool _ModifyState(DWORD dwState, DWORD dwStateMask);

        /**
         * @brief 获取默认图标
         */
        HICON _GetDefaultIcon();
    };
}

// Animation.h


namespace sw
{
    /**
     * @brief 动画控件，仅支持无音频的avi动画
     */
    class Animation : public Control
    {
    public:
        /**
         * @brief 是否将动画居中显示
         */
        const Property<bool> Center;

        /**
         * @brief 是否在打开avi文件后自动播放
         */
        const Property<bool> AutoPlay;

        /**
         * @brief 动画是否正在播放
         */
        const ReadOnlyProperty<bool> IsPlaying;

    public:
        /**
         * @brief 初始化动画控件
         */
        Animation();

        /**
         * @brief 从指定模块中打开avi动画
         * @param hInstance DLL或EXE的模块句柄
         * @param resourceId 动画的资源序号
         * @return 若函数成功则返回true，否则返回false
         */
        bool Open(HINSTANCE hInstance, int resourceId);

        /**
         * @brief 从本地文件打开avi动画
         * @param fileName 动画的文件路径
         * @return 若函数成功则返回true，否则返回false
         */
        bool Open(const std::wstring &fileName);

        /**
         * @brief 从本地文件打开avi动画，传入nullptr可以关闭打开的动画
         * @param fileName 动画的文件路径
         * @return 若函数成功则返回true，否则返回false
         */
        bool Open(const wchar_t *fileName);

        /**
         * @brief 播放动画
         * @param times 循环播放次数，设为-1表示无限循环播放
         * @param beginFrame 从第几帧开始播放，值必须小于65536
         * @param endFrame 播放到第几帧，值必须小于65536，设为-1表示播放到动画结尾
         * @return 若函数成功则返回true，否则返回false
         */
        bool Play(int times, int beginFrame, int endFrame);

        /**
         * @brief 播放动画
         * @param times 循环播放次数，设为-1表示无限循环播放
         * @return 若函数成功则返回true，否则返回false
         */
        bool Play(int times = -1);

        /**
         * @brief 停止播放
         * @return 若函数成功则返回true，否则返回false
         */
        bool Stop();
    };
}

// ButtonBase.h


namespace sw
{
    /**
     * @brief 窗口类名为BUTTON的控件类型的基类
     */
    class ButtonBase : public Control
    {
    private:
        /**
         * @brief 基类别名，方便调用基类函数
         */
        using TBase = Control;

        /**
         * @brief 是否自动调整大小以适应内容
         */
        bool _autoSize = false;

    public:
        /**
         * @brief 是否自动调整大小以适应内容
         */
        const Property<bool> AutoSize;

        /**
         * @brief 文本过长时是否自动换行
         */
        const Property<bool> MultiLine;

        /**
         * @brief 文本边距
         */
        const Property<Thickness> TextMargin;

    public:
        /**
         * @brief 初始化ButtonBase
         */
        ButtonBase();

        /**
         * @brief 析构函数，这里用纯虚函数使该类成为抽象类
         */
        virtual ~ButtonBase() = 0;

    protected:
        /**
         * @brief 初始化控件
         */
        void InitButtonBase(LPCWSTR lpWindowName, DWORD dwStyle, DWORD dwExStyle);

        /**
         * @brief 被单击时调用该函数
         */
        virtual void OnClicked();

        /**
         * @brief 被双击时调用该函数
         */
        virtual void OnDoubleClicked();

        /**
         * @brief 当父窗口接收到控件的WM_COMMAND时调用该函数
         * @param code 通知代码
         */
        virtual void OnCommand(int code) override;

        /**
         * @brief 测量元素所需尺寸，无需考虑边框和边距
         * @param availableSize 可用的尺寸
         * @return 返回元素需要占用的尺寸
         */
        virtual Size MeasureOverride(const Size &availableSize) override;

    private:
        /**
         * @brief 更新布局标记
         */
        void _UpdateLayoutFlags();

        /**
         * @brief 获取理想尺寸
         * @note 该函数发送BCM_GETIDEALSIZE消息
         */
        bool _GetIdealSize(SIZE &size);

        /**
         * @brief 获取按钮控件中绘制文本的边距
         * @note 该函数发送BCM_GETTEXTMARGIN消息
         */
        bool _GetTextMargin(RECT &rect);

        /**
         * @brief 设置按钮控件中绘制文本的边距
         * @note 该函数发送BCM_SETTEXTMARGIN消息
         */
        bool _SetTextMargin(RECT &rect);
    };
}

// DateTimePicker.h


namespace sw
{
    /**
     * @brief 指定DateTimePicker控件显示的日期和时间格式
     */
    enum class DateTimePickerFormat {
        Short,  ///< 以短格式显示日期
        Long,   ///< 以长格式显示日期
        Custom, ///< 自定义格式
    };

    /**
     * @brief 日期和时间选取器
     */
    class DateTimePicker : public Control
    {
    private:
        /**
         * @brief 日期和时间格式
         */
        DateTimePickerFormat _format{DateTimePickerFormat::Short};

        /**
         * @brief 自定义格式字符串
         */
        std::wstring _customFormat{};

    public:
        /**
         * @brief 是否显示上下调整按钮
         */
        const Property<bool> ShowUpDownButton;

        /**
         * @brief 日期和时间格式
         */
        const Property<DateTimePickerFormat> Format;

        /**
         * @brief 自定义日期和时间格式字符串，空字符串表示默认格式
         */
        const Property<std::wstring> CustomFormat;

        /**
         * @brief 当前控件表示的时间
         */
        const Property<SYSTEMTIME> Time;

    public:
        /**
         * @brief 初始化DateTimePicker
         */
        DateTimePicker();

        /**
         * @brief 获取当前控件表示的时间
         * @param out 输出的SYSTEMTIME结构体
         * @return 若获取成功则返回true，否则返回false
         */
        bool GetTime(SYSTEMTIME &out);

        /**
         * @brief 设置当前控件表示的时间
         * @param time 要设置的时间
         * @return 若设置成功则返回true，否则返回false
         */
        bool SetTime(const SYSTEMTIME &time);

        /**
         * @brief 设置可选的时间段
         * @param minTime 最早时间
         * @param maxTime 最晚时间
         * @return 若设置成功则返回true，否则返回false
         */
        bool SetRange(const SYSTEMTIME &minTime, const SYSTEMTIME &maxTime);

    protected:
        /**
         * @brief 父窗口接收到WM_NOTIFY后且父窗口OnNotify函数返回false时调用发出通知控件的该函数
         * @param pNMHDR 包含有关通知消息的信息
         * @param result 函数返回值为true时将该值作为消息的返回值
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnNotified(NMHDR *pNMHDR, LRESULT &result) override;

        /**
         * @brief 当前控件表示的时间改变时调用该函数
         * @param pInfo 发生改变的信息
         */
        virtual void OnTimeChanged(NMDATETIMECHANGE *pInfo);

    private:
        /**
         * @brief 设置格式字符串
         * @param value 要设置的值
         */
        void _SetFormat(const std::wstring &value);

        /**
         * @brief 修改当前控件的样式，该函数会调用ResetHandle
         * @param style 新的样式
         */
        void _UpdateStyle(DWORD style);
    };
}

// HotKeyControl.h


namespace sw
{
    /**
     * @brief 表示一个热键
     */
    struct HotKey {
        VirtualKey key;          // 按键
        HotKeyModifier modifier; // 辅助按键
    };

    /**
     * @brief 热键组合
     */
    enum class HotKeyCombination {
        Alt          = HKCOMB_A,    ///< ALT
        Ctrl         = HKCOMB_C,    ///< CTRL
        CtrlAlt      = HKCOMB_CA,   ///< CTRL+ALT
        None         = HKCOMB_NONE, ///< Unmodified keys
        Shift        = HKCOMB_S,    ///< SHIFT
        ShiftAlt     = HKCOMB_SA,   ///< SHIFT+ALT
        ShiftCtrl    = HKCOMB_SC,   ///< SHIFT+CTRL
        ShiftCtrlAlt = HKCOMB_SCA,  ///< SHIFT+CTRL+ALT
    };

    /**
     * @brief 标记HotKeyCombination枚举类型支持位运算
     */
    _SW_ENUM_ENABLE_BIT_OPERATIONS(HotKeyCombination);

    /**
     * @brief 热键框
     */
    class HotKeyControl : public Control
    {
    private:
        /**
         * @brief 热键的值
         */
        HotKey _value;

    public:
        /**
         * @brief 当前控件所表示的热键值
         */
        const Property<HotKey> Value;

    public:
        /**
         * @brief 初始化热键框
         */
        HotKeyControl();

        /**
         * @brief 设置无效组合与默认值
         * @param invalidComb 无效的组合键
         * @param defaultModifier 用户输入无效组合时使用该组合键替换
         */
        void SetRules(HotKeyCombination invalidComb, HotKeyModifier defaultModifier);

    protected:
        /**
         * @brief 当父窗口接收到控件的WM_COMMAND时调用该函数
         * @param code 通知代码
         */
        virtual void OnCommand(int code) override;

        /**
         * @brief 控件表示的热键值发生改变时调用该函数
         */
        virtual void OnValueChanged(HotKey value);

    private:
        /**
         * @brief 更新_value
         */
        void _UpdateValue();
    };
}

// ItemsControl.h


namespace sw
{
    /**
     * @brief 表示可用于呈现一组项的控件
     */
    class ItemsControl : public Control
    {
    private:
        /**
         * @brief 用户设置的数据源
         */
        IList *_itemsSource = nullptr;

        /**
         * @brief 当前数据源相关的状态
         */
        struct {
            /// @brief 当前数据源的IList指针
            IList *itemsSource = nullptr;

            /// @brief 若当前数据源实现了INotifyCollectionChanged接口，
            ///        则该指针指向该接口以便订阅事件；否则为nullptr
            INotifyCollectionChanged *notifyCollectionChanged = nullptr;
        } _current;

    public:
        /**
         * @brief 数据源
         */
        const Property<IList *> ItemsSource;

        /**
         * @brief 子项数量
         */
        const ReadOnlyProperty<int> ItemsCount;

        /**
         * @brief 选中项的索引，当无选中项时为-1
         */
        const Property<int> SelectedIndex;

        /**
         * @brief 当前选中项的引用，当无选中项时返回空Variant对象
         */
        const ReadOnlyProperty<Variant> SelectedItem;

    public:
        /**
         * @brief 构造函数
         */
        ItemsControl();

    protected:
        /**
         * @brief 获取当前正在使用的数据源
         */
        IList *GetCurrentItemsSource();

        /**
         * @brief 更新当前数据源状态并订阅新数据源的CollectionChanged事件（如果支持）
         */
        void UpdateCurrentItemsSource();

        /**
         * @brief 选中项改变时调用该函数
         */
        virtual void OnSelectionChanged();

    private:
        /**
         * @brief 处理数据源集合变更事件的函数
         * @param sender 事件发送者，即数据源对象
         * @param args 集合变更事件参数
         */
        void _CollectionChangedEventHandler(
            INotifyCollectionChanged &sender, NotifyCollectionChangedEventArgs &args);

    protected:
        /**
         * @brief 获取默认数据源，当ItemsSource未设置时使用该数据源
         * @return 默认数据源的IList指针，若无默认数据源则返回nullptr
         * @note 子类应确保返回的IList在ItemsControl生命周期内始终有效，且保证多次调用返回同一指针
         */
        virtual IList *GetDefaultItemsSource() = 0;

        /**
         * @brief 当前数据源改变时调用该函数
         * @param oldItemsSource 旧的数据源
         * @param newItemsSource 新的数据源
         */
        virtual void OnCurrentItemsSourceChanged(IList *oldItemsSource, IList *newItemsSource) = 0;

        /**
         * @brief 当数据源集合发生变更时调用该函数
         * @param args 包含集合变更信息的事件参数
         */
        virtual void OnCurrentItemsSourceCollectionChanged(const NotifyCollectionChangedEventArgs &args) = 0;

        /**
         * @brief 选中项的索引，当无选中项时为-1
         */
        virtual int GetSelectedIndex() = 0;

        /**
         * @brief 设置选中项索引
         */
        virtual void SetSelectedIndex(int index) = 0;
    };
}

// MonthCalendar.h


namespace sw
{
    /**
     * @brief 月历控件
     */
    class MonthCalendar : public Control
    {
    public:
        /**
         * @brief 是否显示当前日期
         */
        const Property<bool> ShowToday;

        /**
         * @brief 当前控件表示的时间
         */
        const Property<SYSTEMTIME> Time;

    public:
        /**
         * @brief 初始化月历控件
         */
        MonthCalendar();

        /**
         * @brief 获取当前控件的“今天”部分所表示的时间
         * @param out 输出的SYSTEMTIME结构体
         * @return 若获取成功则返回true，否则返回false
         */
        bool GetToday(SYSTEMTIME &out);

        /**
         * @brief 设置当前控件的“今天”部分所表示的时间
         * @param today 要设置的时间
         * @return 若设置成功则返回true，否则返回false
         */
        bool SetToday(const SYSTEMTIME &today);

        /**
         * @brief 获取当前控件表示的时间
         * @param out 输出的SYSTEMTIME结构体
         * @return 若获取成功则返回true，否则返回false
         */
        bool GetTime(SYSTEMTIME &out);

        /**
         * @brief 设置当前控件表示的时间
         * @param time 要设置的时间
         * @return 若设置成功则返回true，否则返回false
         */
        bool SetTime(const SYSTEMTIME &time);

        /**
         * @brief 设置可选的时间段
         * @param minTime 最早时间
         * @param maxTime 最晚时间
         * @return 若设置成功则返回true，否则返回false
         */
        bool SetRange(const SYSTEMTIME &minTime, const SYSTEMTIME &maxTime);

    protected:
        /**
         * @brief 设置背景颜色
         * @param color 要设置的颜色
         * @param redraw 是否重绘
         */
        virtual void OnSetBackColor(Color color, bool redraw) override;

        /**
         * @brief 设置文本颜色
         * @param color 要设置的颜色
         * @param redraw 是否重绘
         */
        virtual void OnSetTextColor(Color color, bool redraw) override;

        /**
         * @brief 父窗口接收到WM_NOTIFY后且父窗口OnNotify函数返回false时调用发出通知控件的该函数
         * @param pNMHDR 包含有关通知消息的信息
         * @param result 函数返回值为true时将该值作为消息的返回值
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnNotified(NMHDR *pNMHDR, LRESULT &result) override;

        /**
         * @brief 当前控件表示的时间改变时调用该函数
         * @param pInfo 发生改变的信息
         */
        virtual void OnTimeChanged(NMSELCHANGE *pInfo);
    };
}

// Panel.h


namespace sw
{
    /**
     * @brief 边框类型
     */
    enum class BorderStyle {
        None   = 0,           ///< 无边框
        Bump   = EDGE_BUMP,   ///< 突出的凸起边框
        Etched = EDGE_ETCHED, ///< 刻痕式边框
        Raised = EDGE_RAISED, ///< 凸起边框
        Sunked = EDGE_SUNKEN, ///< 凹陷边框
    };

    /**
     * @brief 面板
     */
    class Panel : public Layer<Control>
    {
    private:
        /**
         * @brief 边框类型，默认为无边框
         */
        BorderStyle _borderStyle = sw::BorderStyle::None;

        /**
         * @brief 内边距
         */
        Thickness _padding;

    public:
        /**
         * @brief 边框样式
         */
        const Property<sw::BorderStyle> BorderStyle;

        /**
         * @brief 面板的内边距
         */
        const Property<sw::Thickness> Padding;

    public:
        /**
         * @brief 初始化面板
         */
        Panel();

    protected:
        /**
         * @brief 更新边框
         */
        void UpdateBorder();

        /**
         * @brief 对WndProc的封装
         */
        virtual LRESULT WndProc(ProcMsg &refMsg) override;

        /**
         * @brief 接收到WM_ERASEBKGND时调用该函数
         * @param hdc 设备上下文句柄
         * @param result 若已处理该消息则设为非零值，默认值为0
         * @return 若返回true则将result作为消息的返回值，否则使用DefaultWndProc的返回值
         */
        virtual bool OnEraseBackground(HDC hdc, LRESULT &result) override;

        /**
         * @brief 接收到WM_PAINT时调用该函数
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnPaint() override;

        /**
         * @brief 接收到WM_NCPAINT时调用该函数
         * @param hRgn 窗口更新区域的句柄，可能为NULL
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnNcPaint(HRGN hRgn) override;

        /**
         * @brief 绘制边框
         * @param hdc 绘制设备句柄，可能为NULL
         * @param rect 绘制边框的矩形区域，该函数会减去边框厚度
         * @note 若hdc为NULL则不进行绘制，只更新rect
         */
        virtual void OnDrawBorder(HDC hdc, RECT &rect);

        /**
         * @brief 绘制内边距
         * @param hdc 绘制设备句柄，可能为NULL
         * @param rect 绘制内边距的矩形区域，该函数会减去内边距
         * @note 若hdc为NULL则不进行绘制，只更新rect
         */
        virtual void OnDrawPadding(HDC hdc, RECT &rect);
    };
}

// ProgressBar.h


namespace sw
{
    /**
     * @brief 进度条状态
     */
    enum class ProgressBarState {
        Normal = 0x0001, ///< 正常（PBST_NORMAL）
        Error  = 0x0002, ///< 错误（PBST_ERROR）
        Paused = 0x0003, ///< 暂停（PBST_PAUSED）
    };

    /**
     * @brief 进度条控件
     */
    class ProgressBar : public Control
    {
    public:
        /**
         * @brief 进度范围的下限
         */
        const Property<uint16_t> Minimum;

        /**
         * @brief 进度范围的上限
         */
        const Property<uint16_t> Maximum;

        /**
         * @brief 当前进度值
         */
        const Property<uint16_t> Value;

        /**
         * @brief 进度条状态
         */
        const Property<ProgressBarState> State;

        /**
         * @brief 是否显示为垂直进度条
         */
        const Property<bool> Vertical;

    public:
        /**
         * @brief 初始化进度条
         */
        ProgressBar();
    };
}

// Slider.h


namespace sw
{
    /**
     * @brief 滑块控件
     */
    class Slider : public Control
    {
    public:
        /**
         * @brief 最小值
         */
        const Property<int> Minimum;

        /**
         * @brief 最大值
         */
        const Property<int> Maximum;

        /**
         * @brief 当前滑块的值
         */
        const Property<int> Value;

        /**
         * @brief 是否显示为垂直滑块
         */
        const Property<bool> Vertical;

        /**
         * @brief 是否在滑动时显示数值提示
         */
        const Property<bool> ValueTooltips;

    public:
        /**
         * @brief 初始化滑块控件
         */
        Slider();

    protected:
        /**
         * @brief 接收到WM_VSCROLL时调用目标控件的该函数
         * @param event 事件类型，即消息wParam的低字
         * @param pos 当前滚动条的位置，仅当event为SB_THUMBPOSITION或SB_THUMBTRACK时有效
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnVerticalScroll(int event, int pos) override;

        /**
         * @brief 接收到WM_HSCROLL时调用目标控件的该函数
         * @param event 事件类型，即消息wParam的低字
         * @param pos 当前滚动条的位置，仅当event为SB_THUMBPOSITION或SB_THUMBTRACK时有效
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnHorizontalScroll(int event, int pos) override;

        /**
         * @brief 滑块的值改变时调用该函数
         */
        virtual void OnValueChanged();

        /**
         * @brief 释放滑块时调用该函数
         */
        virtual void OnEndTrack();
    };
}

// Splitter.h


namespace sw
{
    /**
     * @brief 分隔条
     */
    class Splitter : public Control
    {
    private:
        /**
         * @brief 记录分隔条方向
         */
        Orientation _orientation = Orientation::Horizontal;

        /**
         * @brief 是否绘制分隔条线条
         */
        bool _drawSplitterLine = true;

    public:
        /**
         * @brief 分隔条的方向，给该属性赋值同时会改变HorizontalAlignment和VerticalAlignment属性的值
         */
        const Property<Orientation> Orientation;

        /**
         * @brief 是否绘制分隔条线条
         */
        const Property<bool> DrawSplitterLine;

    public:
        /**
         * @brief 初始化分隔条
         */
        Splitter();

    protected:
        /**
         * @brief 接收到WM_PAINT时调用该函数
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnPaint() override;

        /**
         * @brief 接收到WM_SIZE时调用该函数
         * @param newClientSize 改变后的用户区尺寸
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnSize(const Size &newClientSize) override;
    };
}

// StaticControl.h


namespace sw
{
    /**
     * @brief 静态控件
     */
    class StaticControl : public Control
    {
    public:
        /**
         * @brief 获取或设置控件的SS_NOTIFY样式
         */
        const Property<bool> Notify;

    public:
        /**
         * @brief 初始化静态控件
         */
        StaticControl();
    };
}

// StatusBar.h


namespace sw
{
    /**
     * @brief 状态栏
     */
    class StatusBar : public Control
    {
    public:
        /**
         * @brief 是否在控件右下角显示大小调整手柄，修改该属性会重置当前控件
         */
        const Property<bool> SizingGrip;

        /**
         * @brief 获取当前控件中的分区数
         */
        const ReadOnlyProperty<int> PartsCount;

        /**
         * @brief 指定控件是否使用Unicode编码
         */
        const Property<bool> UseUnicode;

    public:
        /**
         * @brief 初始化状态栏
         */
        StatusBar();

        /**
         * @brief 设置控件中的分区信息
         * @param parts 每个部分的宽度，用-1表示延伸到最右边，不能多于256个
         * @return 若函数成功则返回true，否则返回false
         */
        bool SetParts(std::initializer_list<double> parts);

        /**
         * @brief 获取指定部分的文本
         * @param index 指定部分的索引
         * @param out 获取到的文本
         * @return 若函数成功则返回true，否则返回false
         */
        bool GetTextAt(uint8_t index, std::wstring &out);

        /**
         * @brief 设置指定部分的文本
         * @param index 指定部分的索引
         * @param text 要设置的文本
         * @return 若函数成功则返回true，否则返回false
         */
        bool SetTextAt(uint8_t index, const std::wstring &text);

        /**
         * @brief 获取指定部分的位置与尺寸
         * @param index 指定部分的索引
         * @param out 获取到的Rect
         * @return 若函数成功则返回true，否则返回false
         */
        bool GetRectAt(uint8_t index, sw::Rect &out);

    protected:
        /**
         * @brief 设置背景颜色
         * @param color 要设置的颜色
         * @param redraw 是否重绘
         */
        virtual void OnSetBackColor(Color color, bool redraw) override;
    };
}

// SysLink.h


namespace sw
{
    /**
     * @brief 包含链接的文本区域
     */
    class SysLink : public Control
    {
    private:
        /**
         * @brief 文本所需的尺寸
         */
        sw::Size _textSize;

        /**
         * @brief 是否根据文本自动调整尺寸
         */
        bool _autoSize = true;

    public:
        /**
         * @brief 是否忽略回车按键，当该属性为true时按下回车键不会触发单击事件
         */
        const Property<bool> IgnoreReturn;

        /**
         * @brief 是否根据内容自动调整尺寸
         */
        const Property<bool> AutoSize;

    public:
        /**
         * @brief 初始化SysLink
         */
        SysLink();

    protected:
        /**
         * @brief Text属性更改时调用此函数
         */
        virtual void OnTextChanged() override;

        /**
         * @brief 字体改变时调用该函数
         * @param hfont 字体句柄
         */
        virtual void FontChanged(HFONT hfont) override;

        /**
         * @brief 测量元素所需尺寸，无需考虑边框和边距
         * @param availableSize 可用的尺寸
         * @return 返回元素需要占用的尺寸
         */
        virtual Size MeasureOverride(const Size &availableSize) override;

        /**
         * @brief 父窗口接收到WM_NOTIFY后且父窗口OnNotify函数返回false时调用发出通知控件的该函数
         * @param pNMHDR 包含有关通知消息的信息
         * @param result 函数返回值为true时将该值作为消息的返回值
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnNotified(NMHDR *pNMHDR, LRESULT &result) override;

        /**
         * @brief 链接被单击时调用该函数
         * @param pNMLink 包含链接相关信息
         */
        virtual void OnClicked(NMLINK *pNMLink);

    private:
        /**
         * @brief 更新_textSize
         */
        void _UpdateTextSize();

        /**
         * @brief 调整尺寸为_textSize
         */
        void _ResizeToTextSize();

        /**
         * @brief 更新LayoutUpdateCondition属性
         */
        void _UpdateLayoutFlags();
    };
}

// TabControl.h


namespace sw
{
    /**
     * @brief TabControl标签的位置
     */
    enum class TabAlignment {
        Top,    ///< 顶部
        Bottom, ///< 底部
        Left,   ///< 左边
        Right   // 右边
    };

    /**
     * @brief 标签页控件
     */
    class TabControl : public Control
    {
    private:
        /**
         * @brief 基类别名，方便调用基类函数
         */
        using TBase = Control;

        /**
         * @brief 是否自动调整大小
         */
        bool _autoSize = true;

    public:
        /**
         * @brief 内容区域位置与尺寸
         */
        const ReadOnlyProperty<sw::Rect> ContentRect;

        /**
         * @brief 当前页面的索引
         */
        const Property<int> SelectedIndex;

        /**
         * @brief 标签的位置
         */
        const Property<TabAlignment> Alignment;

        /**
         * @brief 是否开启多行标签
         */
        const Property<bool> MultiLine;

        /**
         * @brief 是否自动调整大小以适应当前页面内容，默认为true
         */
        const Property<bool> AutoSize;

    public:
        /**
         * @brief 初始化标签页控件
         */
        TabControl();

        /**
         * @brief 获取标签项的数量
         */
        int GetTabCount();

        /**
         * @brief 更新标签项信息
         */
        void UpdateTab();

        /**
         * @brief 更新指定索引处页面项的文本
         * @param index 要更新的索引
         */
        void UpdateTabText(int index);

    protected:
        /**
         * @brief 添加子元素后调用该函数
         * @param element 添加的子元素
         */
        virtual void OnAddedChild(UIElement &element) override;

        /**
         * @brief 移除子元素后调用该函数
         * @param element 移除的子元素
         */
        virtual void OnRemovedChild(UIElement &element) override;

        /**
         * @brief 测量元素所需尺寸，无需考虑边框和边距
         * @param availableSize 可用的尺寸
         * @return 返回元素需要占用的尺寸
         */
        virtual Size MeasureOverride(const Size &availableSize) override;

        /**
         * @brief 安排子元素的位置，可重写该函数以实现自定义布局
         * @param finalSize 可用于排列子元素的最终尺寸
         */
        virtual void ArrangeOverride(const Size &finalSize) override;

        /**
         * @brief 父窗口接收到WM_NOTIFY后且父窗口OnNotify函数返回false时调用发出通知控件的该函数
         * @param pNMHDR 包含有关通知消息的信息
         * @param result 函数返回值为true时将该值作为消息的返回值
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnNotified(NMHDR *pNMHDR, LRESULT &result) override;

        /**
         * @brief SelectedIndex属性更改时调用该函数
         */
        virtual void OnSelectedIndexChanged();

    private:
        /**
         * @brief 设置标签位置
         */
        void _SetTabAlignment(TabAlignment value);

        /**
         * @brief 根据选中的tab更新子元素的Visible属性
         */
        void _UpdateChildVisible(bool invalidMeasure = true);

        /**
         * @brief 发送TCM_INSERTITEMW消息
         */
        int _InsertItem(int index, TCITEMW &item);

        /**
         * @brief 发送TCM_SETITEMW消息
         */
        bool _SetItem(int index, TCITEMW &item);

        /**
         * @brief 发送TCM_DELETEITEM消息
         */
        bool _DeleteItem(int index);

        /**
         * @brief 发送TCM_DELETEALLITEMS消息
         */
        bool _DeleteAllItems();

        /**
         * @brief 计算内容区域的位置和尺寸
         */
        void _CalcContentRect(RECT &rect);

        /**
         * @brief 计算指定内容大小时控件的理想大小
         */
        void _CalcIdealSize(SIZE &size);

        /**
         * @brief 获取当前选中的子元素，若没有选中任何子元素则返回nullptr
         */
        UIElement *_GetSelectedItem();
    };
}

// TextBoxBase.h


namespace sw
{
    /**
     * @brief 窗口类名为EDIT的控件类型的基类
     */
    class TextBoxBase : public Control
    {
    private:
        /**
         * @brief 在读取Text属性时用于判断是否需要更新储存的文本
         */
        bool _isTextChanged = false;

        /**
         * @brief 是否允许输入制表符
         */
        bool _acceptTab = false;

    public:
        /**
         * @brief 是否只读
         */
        const Property<bool> ReadOnly;

        /**
         * @brief 文本的水平对齐方式，可设为左对齐、中心对齐、右对齐
         */
        const Property<sw::HorizontalAlignment> HorizontalContentAlignment;

        /**
         * @brief 是否可以撤销
         */
        const ReadOnlyProperty<bool> CanUndo;

        /**
         * @brief 是否允许输入制表符
         */
        const Property<bool> AcceptTab;

    public:
        /**
         * @brief 初始化TextBoxBase
         */
        TextBoxBase();

        /**
         * @brief 析构函数，这里用纯虚函数使该类成为抽象类
         */
        virtual ~TextBoxBase() = 0;

    protected:
        /**
         * @brief 初始化控件
         */
        void InitTextBoxBase(DWORD dwStyle, DWORD dwExStyle);

        /**
         * @brief 获取窗口文本
         * @return 编辑框的文本内容
         */
        virtual std::wstring &GetInternalText() override;

        /**
         * @brief 当父窗口接收到控件的WM_COMMAND时调用该函数
         * @param code 通知代码
         */
        virtual void OnCommand(int code) override;

        /**
         * @brief 接收到WM_CHAR时调用该函数
         * @param ch 按键的字符代码
         * @param flags 附加信息
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnChar(wchar_t ch, const KeyFlags &flags) override;

        /**
         * @brief 接收到WM_KEYDOWN时调用该函数
         * @param key 虚拟按键
         * @param flags 附加信息
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnKeyDown(VirtualKey key, const KeyFlags &flags) override;

    public:
        /**
         * @brief 选择指定文本内容
         * @param start 起始位置
         * @param length 选择文本的长度
         */
        void Select(int start, int length);

        /**
         * @brief 选中所有文本
         */
        void SelectAll();

        /**
         * @brief 将控件内容滚动到当前插入符号位置
         */
        void ScrollToCaret();

        /**
         * @brief 撤销
         * @return 操作是否成功
         */
        bool Undo();

        /**
         * @brief 清空内容
         */
        void Clear();

    private:
        /**
         * @brief 读取HorizontalContentAlignment属性时调用
         */
        sw::HorizontalAlignment _GetHorzContentAlignment();

        /**
         * @brief 写入HorizontalContentAlignment属性时调用
         */
        void _SetHorzContentAlignment(sw::HorizontalAlignment value);
    };
}

// TreeView.h

namespace sw
{
    /**
     * @brief 树视图的图像列表枚举
     */
    enum class TreeViewImageList {
        Normal = TVSIL_NORMAL, ///< 普通图像列表
        State  = TVSIL_STATE,  ///< 状态映像列表
    };

    /**
     * @brief 树视图项
     */
    class TreeViewNode : public IToString<TreeViewNode>,
                         public IEqualityComparable<TreeViewNode>
    {
    private:
        /**
         * @brief 树视图控件的窗口句柄
         */
        HWND _hwnd;

        /**
         * @brief 树视图项的句柄
         */
        HTREEITEM _hitem;

    public:
        /**
         * @brief 默认构造函数
         */
        TreeViewNode() = default;

        /**
         * @brief 创建TreeViewItem
         */
        TreeViewNode(HWND hwnd, HTREEITEM hitem);

        /**
         * @brief 获取所属树视图控件的窗口句柄
         */
        HWND GetOwnerHandle() const;

        /**
         * @brief 获取当前项的句柄
         */
        HTREEITEM GetHandle() const;

        /**
         * @brief 获取当前项的文本
         */
        std::wstring ToString() const;

        /**
         * @brief 判断当前项与另一个项是否相等
         */
        bool Equals(const TreeViewNode &other) const;

        /**
         * @brief 判断当前项是否为空
         */
        bool IsNull() const;

        /**
         * @brief 判断当前项是否有效
         */
        explicit operator bool() const;

        /**
         * @brief 获取当前项的文本
         */
        std::wstring GetText() const;

        /**
         * @brief 设置当前项的文本
         * @return 操作是否成功
         */
        bool SetText(const std::wstring &text);

        /**
         * @brief 获取父节点
         * @return 父节点，若无父节点则返回空节点
         */
        TreeViewNode GetParent() const;

        /**
         * @brief 获取下一个节点
         * @return 下一个节点，若无下一个节点则返回空节点
         */
        TreeViewNode GetNextNode() const;

        /**
         * @brief 获取上一个节点
         * @return 上一个节点，若无上一个节点则返回空节点
         */
        TreeViewNode GetPreviousNode() const;

        /**
         * @brief 获取第一个子节点
         * @return 第一个子节点，若无子节点则返回空节点
         */
        TreeViewNode GetFirstChildNode() const;

        /**
         * @brief 在当前节点后插入新节点
         * @return 新插入的节点
         */
        TreeViewNode InsertAfter(const std::wstring &text);

        /**
         * @brief 添加子节点到当前节点下
         * @return 新插入的节点
         */
        TreeViewNode AddChild(const std::wstring &text);

        /**
         * @brief 判断当前节点是否被选中
         * @return 若节点被选中则返回true，否则返回false
         */
        bool IsSelected() const;

        /**
         * @brief 选中当前节点
         * @return 操作是否成功
         */
        bool Select();

        /**
         * @brief 删除当前节点
         * @return 操作是否成功
         */
        bool Delete();

        /**
         * @brief 判断当前节点是否展开
         * @return 若节点已展开则返回true，否则返回false
         */
        bool IsExpanded() const;

        /**
         * @brief 设置当前节点展开或折叠
         * @return 操作是否成功
         */
        bool SetExpand(bool expand);

        /**
         * @brief 展开当前节点
         * @return 操作是否成功
         */
        bool Expand();

        /**
         * @brief 折叠当前节点
         * @return 操作是否成功
         */
        bool Collapse();

        /**
         * @brief 获取与当前节点关联的用户数据
         */
        void *GetUserData() const;

        /**
         * @brief 设置与当前节点关联的用户数据
         * @return 操作是否成功
         */
        bool SetUserData(void *data);

        /**
         * @brief 判断当前节点是否被选中复选框
         * @return 若节点被选中则返回true，否则返回false
         */
        bool IsChecked() const;

        /**
         * @brief 设置当前节点的复选框选中状态
         */
        void SetCheck(bool check);

        /**
         * @brief 设置当前节点的图像
         * @return 操作是否成功
         */
        bool SetImages(int imageIndex, int selectedImageIndex);

        /**
         * @brief 获取当前节点的直接子节点数
         */
        int GetChildCount() const;

        /**
         * @brief 删除当前节点的所有子节点
         * @return 删除直接子节点的个数
         */
        int DeleteAllChildren();
    };

    /**
     * @brief 树视图节点正在展开或折叠事件参数类型
     */
    class TreeViewItemExpandingEventArgs : public TypedRoutedEventArgs<TreeView_ItemExpanding, CancelableEventArgs>
    {
    public:
        /// @brief true表示展开，false表示折叠
        bool action;

        /// @brief 正在展开或折叠的节点
        TreeViewNode node;

        /// @brief 构造函数
        /// @param action true表示展开，false表示折叠
        /// @param node 正在展开或折叠的节点
        TreeViewItemExpandingEventArgs(bool action, const TreeViewNode &node)
            : action(action), node(node)
        {
        }
    };

    /**
     * @brief 树视图节点已展开或折叠事件参数类型
     */
    class TreeViewItemExpandedEventArgs : public TypedRoutedEventArgs<TreeView_ItemExpanding>
    {
    public:
        /// @brief true表示展开，false表示折叠
        bool action;

        /// @brief 已展开或折叠的节点
        TreeViewNode node;

        /// @brief 构造函数
        /// @param action true表示展开，false表示折叠
        /// @param node 已展开或折叠的节点
        TreeViewItemExpandedEventArgs(bool action, const TreeViewNode &node)
            : action(action), node(node)
        {
        }
    };

    /**
     * @brief 树视图节点复选框状态改变事件参数类型
     */
    class TreeViewCheckStateChangedEventArgs : public TypedRoutedEventArgs<TreeView_CheckStateChanged>
    {
    public:
        /// @brief 复选框的新状态，0表示未选中，1表示选中，-1表示无复选框
        int checkState;

        /// @brief 复选框状态改变的节点
        TreeViewNode node;

        /// @brief 构造函数
        /// @param checkState 复选框的新状态，0表示未选中，1表示选中，-1表示无复选框
        /// @param node 复选框状态改变的节点
        TreeViewCheckStateChangedEventArgs(int checkState, const TreeViewNode &node)
            : checkState(checkState), node(node)
        {
        }
    };

    /**
     * @brief 树视图控件
     */
    class TreeView : public Control
    {
    private:
        /**
         * @brief 基类别名，方便调用基类函数实现
         */
        using TBase = Control;

    public:
        /**
         * @brief 根节点
         */
        const ReadOnlyProperty<TreeViewNode> Root;

        /**
         * @brief 选中的节点，若无选中节点则返回空节点
         */
        const ReadOnlyProperty<TreeViewNode> SelectedItem;

        /**
         * @brief 所有节点数
         */
        const ReadOnlyProperty<int> AllItemsCount;

        /**
         * @brief 是否在第一列显示复选框
         */
        const Property<bool> CheckBoxes;

        /**
         * @brief 线条颜色
         */
        const Property<Color> LineColor;

        /**
         * @brief 缩进宽度
         */
        const Property<double> IndentWidth;

    public:
        /**
         * @brief 初始化TreeView
         */
        TreeView();

    protected:
        /**
         * @brief 设置背景颜色
         * @param color 要设置的颜色
         * @param redraw 是否重绘
         */
        virtual void OnSetBackColor(Color color, bool redraw) override;

        /**
         * @brief 设置文本颜色
         * @param color 要设置的颜色
         * @param redraw 是否重绘
         */
        virtual void OnSetTextColor(Color color, bool redraw) override;

        /**
         * @brief 父窗口接收到WM_NOTIFY后且父窗口OnNotify函数返回false时调用发出通知控件的该函数
         * @param pNMHDR 包含有关通知消息的信息
         * @param result 函数返回值为true时将该值作为消息的返回值
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnNotified(NMHDR *pNMHDR, LRESULT &result) override;

        /**
         * @brief 选中的节点发生改变时调用该函数
         */
        virtual void OnSelectionChanged();

        /**
         * @brief 控件被单机时调用该函数
         * @param pNMHDR 包含有关通知消息的信息
         * @param result 函数返回值为true时将该值作为消息的返回值
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnClicked(NMHDR *pNMHDR, LRESULT &result);

        /**
         * @brief 控件被双击时调用该函数
         * @param pNMHDR 包含有关通知消息的信息
         * @param result 函数返回值为true时将该值作为消息的返回值
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnDoubleClicked(NMHDR *pNMHDR, LRESULT &result);

        /**
         * @brief 当OnNotified接收到TVN_GETDISPINFO通知时调用该函数
         * @param pNMInfo 包含有关通知消息的信息
         */
        virtual void OnGetDispInfo(NMTVDISPINFOW *pNMInfo);

        /**
         * @brief 节点展开或折叠前调用该函数
         * @param pNMTV 包含有关通知消息的信息
         * @return 若返回true则取消展开或折叠操作，否则继续进行
         */
        virtual bool OnItemExpanding(NMTREEVIEWW *pNMTV);

        /**
         * @brief 节点展开或折叠后调用该函数
         * @param pNMTV 包含有关通知消息的信息
         */
        virtual void OnItemExpanded(NMTREEVIEWW *pNMTV);

        /**
         * @brief 节点某些属性发生变化时调用该函数
         * @param pNMInfo 包含有关通知消息的信息
         */
        virtual void OnItemChanged(NMTVITEMCHANGE *pNMInfo);

    public:
        /**
         * @brief 清空所有节点
         */
        void Clear();

        /**
         * @brief 添加新节点到根节点
         * @param text 节点文本
         * @return 新插入的节点
         */
        TreeViewNode AddItem(const std::wstring &text);

        /**
         * @brief 获取指定类型的图像列表
         * @param imageList 图像列表类型
         */
        ImageList GetImageList(TreeViewImageList imageList);

        /**
         * @brief 设置指定类型的图像列表
         * @param imageList 图像列表类型
         * @param value 要设置的图像列表
         */
        HIMAGELIST SetImageList(TreeViewImageList imageList, HIMAGELIST value);

    private:
        /**
         * @brief 获取根节点
         */
        TreeViewNode _GetRoot();

        /**
         * @brief 获取选中的节点，若无选中节点则返回空节点
         */
        TreeViewNode _GetSelectedItem();

        /**
         * @brief 插入新节点
         */
        TreeViewNode _InsertItem(HTREEITEM hParent, HTREEITEM hInsertAfter, const std::wstring &text);
    };
}

// Window.h


namespace sw
{
    class Menu; // Menu.h

    /**
     * @brief 窗口状态
     */
    enum class WindowState {
        Normal,    ///< 普通窗口
        Minimized, ///< 最小化窗口
        Maximized, ///< 最大化窗口
    };

    /**
     * @brief 窗口启动位置
     */
    enum class WindowStartupLocation {
        Manual,       ///< 使用系统默认或手动设置
        CenterScreen, ///< 屏幕中心
        CenterOwner,  ///< 所有者窗口中心
    };

    /**
     * @brief 窗口
     */
    class Window : public Layer<UIElement>, public IDialog
    {
    private:
        /**
         * @brief 基类别名
         */
        using TBase = Layer;

        /**
         * @brief 是否为第一次显示，该字段使StartupLocation生效
         */
        bool _isFirstShow = true;

        /**
         * @brief 窗口初次启动的位置
         */
        WindowStartupLocation _startupLocation = WindowStartupLocation::Manual;

        /**
         * @brief 窗口顶部菜单
         */
        sw::Menu *_menu = nullptr;

        /**
         * @brief 窗口在失去焦点前保存句柄，用于窗口重新获得焦点时保留原先焦点控件的焦点
         */
        HWND _hPrevFocused = NULL;

        /**
         * @brief 窗口的默认布局方式
         */
        std::unique_ptr<LayoutHost> _defaultLayout;

        /**
         * @brief 当前窗口是否显示为模态窗口
         */
        bool _isModal = false;

        /**
         * @brief 以模态窗口显示时保存所有者窗口
         */
        HWND _hModalOwner = NULL;

        /**
         * @brief 窗口无边框
         */
        bool _isBorderless = false;

        /**
         * @brief 窗口的对话框结果，ShowDialog返回该值
         */
        int _dialogResult = 0;

        /**
         * @brief 窗口是否正在销毁
         */
        bool _isDestroying = false;

        /**
         * @brief 布局禁用计数器
         */
        int _disableLayoutCount = 0;

    public:
        /**
         * @brief 当前线程的活动窗口
         */
        static const ReadOnlyProperty<Window *> ActiveWindow;

        /**
         * @brief 当前线程已创建的窗口数
         * @note 该属性是线程局部的，每个线程有各自独立的值
         */
        static const ReadOnlyProperty<int> WindowCount;

        /**
         * @brief 窗口初次启动的位置
         */
        const Property<WindowStartupLocation> StartupLocation;

        /**
         * @brief 窗口状态
         */
        const Property<WindowState> State;

        /**
         * @brief 窗口是否可调整大小
         */
        const Property<bool> SizeBox;

        /**
         * @brief 最大化按钮是否可用
         */
        const Property<bool> MaximizeBox;

        /**
         * @brief 最小化按钮是否可用
         */
        const Property<bool> MinimizeBox;

        /**
         * @brief 窗口是否置顶
         */
        const Property<bool> Topmost;

        /**
         * @brief 是否显示为ToolWindow (窄边框)
         */
        const Property<bool> ToolWindow;

        /**
         * @brief 窗口顶部的菜单栏
         */
        const Property<sw::Menu *> Menu;

        /**
         * @brief 窗口是否显示为模态窗口，当调用ShowDialog时该属性值为true，否则为false
         */
        const ReadOnlyProperty<bool> IsModal;

        /**
         * @brief 拥有者窗口
         */
        const Property<Window *> Owner;

        /**
         * @brief 窗口是否为分层窗口，即WS_EX_LAYERED样式是否被设置
         */
        const Property<bool> IsLayered;

        /**
         * @brief 窗口的透明度，范围为0.0~1.0
         * @note 只有将IsLayered设为true该属性才生效，初始值为0.0但需手动设置新值后才会生效
         */
        const Property<double> Opacity;

        /**
         * @brief 窗口无边框
         */
        const Property<bool> Borderless;

        /**
         * @brief 窗口的对话框结果，ShowDialog返回该值
         * @note 该属性仅在窗口作为模态对话框显示时有效，默认值为0，该属性一旦被设置则会自动关闭窗口
         */
        const Property<int> DialogResult;

        /**
         * @brief 窗口在最小化或最大化之前的位置和尺寸
         */
        const ReadOnlyProperty<sw::Rect> RestoreRect;

        /**
         * @brief 窗口布局是否被禁用
         */
        const ReadOnlyProperty<bool> IsLayoutDisabled;

    public:
        /**
         * @brief 初始化窗口
         */
        Window();

        /**
         * @brief 析构函数
         */
        ~Window();

    protected:
        /**
         * @brief 对WndProc的封装
         */
        virtual LRESULT WndProc(ProcMsg &refMsg) override;

        /**
         * @brief 获取默认布局对象
         */
        virtual LayoutHost *GetDefaultLayout() override;

        /**
         * @brief 接收到WM_CREATE时调用该函数
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnCreate() override;

        /**
         * @brief 接收到WM_CLOSE时调用该函数
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnClose() override;

        /**
         * @brief 接收到WM_DESTROY时调用该函数
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnDestroy() override;

        /**
         * @brief 接收到WM_ERASEBKGND时调用该函数
         * @param hdc 设备上下文句柄
         * @param result 若已处理该消息则设为非零值，默认值为0
         * @return 若返回true则将result作为消息的返回值，否则使用DefaultWndProc的返回值
         */
        virtual bool OnEraseBackground(HDC hdc, LRESULT &result) override;

        /**
         * @brief 接收到WM_PAINT时调用该函数
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnPaint() override;

        /**
         * @brief 当OnCommand接收到菜单命令时调用该函数
         * @param id 菜单id
         */
        virtual void OnMenuCommand(int id) override;

        /**
         * @brief 当MinWidth、MinHeight、MaxWidth或MaxHeight属性更改时调用此函数
         */
        virtual void OnMinMaxSizeChanged() override;

        /**
         * @brief 窗口第一次显示时调用该函数
         */
        virtual void OnFirstShow();

        /**
         * @brief 窗口成为前台窗口时调用该函数
         */
        virtual void OnActived();

        /**
         * @brief 窗口成为后台窗口时调用该函数
         */
        virtual void OnInactived();

        /**
         * @brief 接收到WM_DPICHANGED时调用该函数
         * @param dpiX 横向DPI
         * @param dpiY 纵向DPI
         * @param newRect 建议的新窗口位置和尺寸
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnDpiChanged(int dpiX, int dpiY, RECT &newRect);

    public:
        /**
         * @brief 关闭窗口
         */
        virtual void Close() override;

        /**
         * @brief 显示窗口
         */
        virtual void Show() override;

        /**
         * @brief 将窗口显示为模式对话框
         * @param owner 窗体的所有者，若为nullptr则使用当前活动窗口
         * @return DialogResult属性的值，若函数失败则返回-1
         * @note 该函数会创建一个新的消息循环并在窗口销毁时退出，只能在创建窗口的线程调用
         */
        virtual int ShowDialog(Window *owner = nullptr) override;

        /**
         * @brief 将窗口显示为模式对话框
         * @param owner 窗体的所有者，窗体显示期间该窗体的Enabled属性将被设为false，该参数不能设为自己
         * @return DialogResult属性的值，若函数失败则返回-1
         * @note 该函数会创建一个新的消息循环并在窗口销毁时退出，只能在创建窗口的线程调用
         */
        virtual int ShowDialog(Window &owner);

        /**
         * @brief 禁用窗口布局
         * @note 需与EnableLayout配对使用，内部维护了一个计数器以支持嵌套调用
         * @note 禁用布局操作只对顶层窗口有效，且只能在窗口所在的线程调用该函数
         * @return 操作是否成功
         */
        bool DisableLayout();

        /**
         * @brief 恢复窗口布局，与DisableLayout配对使用
         * @param reset 若该参数为true则直接将布局禁用计数器重置为0
         * @note 禁用布局操作只对顶层窗口有效，且只能在窗口所在的线程调用该函数
         * @return 操作是否成功
         */
        bool EnableLayout(bool reset = false);

        /**
         * @brief 设置图标
         * @param hIcon 图标句柄
         */
        void SetIcon(HICON hIcon);

        /**
         * @brief 重回窗口的菜单栏
         */
        void DrawMenuBar();

        /**
         * @brief 调整窗口尺寸以适应其内容大小
         * @note 该函数仅对设置了布局方式且AutoSize属性为true的顶级窗口有效
         * @return 若窗口尺寸已被调整则返回true，否则返回false
         */
        bool SizeToContent();

        /**
         * @brief 设置窗口的默认布局方式
         */
        template <typename TLayout>
        auto SetLayout()
            -> typename std::enable_if<std::is_base_of<LayoutHost, TLayout>::value>::type
        {
            auto layout = std::make_unique<TLayout>();
            layout->Associate(this);
            _defaultLayout = std::move(layout);
            InvalidateMeasure();
        }

        /**
         * @brief 取消通过SetLayout设置的布局方式
         */
        template <std::nullptr_t>
        void SetLayout()
        {
            _defaultLayout.reset(nullptr);
            InvalidateMeasure();
        }

    private:
        /**
         * @brief 窗口布局是否被禁用
         */
        bool _IsLayoutDisabled() const noexcept;

        /**
         * @brief 将窗口居中于指定矩形区域
         * @param rect 目标矩形区域
         */
        void _CenterWindow(const sw::Rect &rect);

        /**
         * @brief 限制窗口的最小和最大尺寸
         * @param pInfo 指向MINMAXINFO结构的指针
         */
        void _ClampMinMaxSize(PMINMAXINFO pInfo);

        /**
         * @brief 通过窗口句柄获取Window指针
         * @param hwnd 窗口句柄
         * @return 若函数成功则返回对象的指针，否则返回nullptr
         */
        static Window *_GetWindowPtr(HWND hwnd);

        /**
         * @brief 关联窗口句柄与Window对象
         * @param hwnd 窗口句柄
         * @param wnd 与句柄关联的对象
         */
        static void _SetWindowPtr(HWND hwnd, Window &wnd);

        /**
         * @brief 获取窗口默认图标（即当前exe图标）
         * @return 图标句柄
         */
        static HICON _GetWindowDefaultIcon();
    };
}

// BmpBox.h


namespace sw
{
    /**
     * @brief 指定位图在BmpBox中的定位方式
     */
    enum class BmpBoxSizeMode {
        Normal,       ///< 位图置于BmpBox左上角
        StretchImage, ///< 拉伸位图填充BmpBox
        AutoSize,     ///< 调整BmpBox大小，使其等于所包含位图的大小
        CenterImage,  ///< 将位图居中显示
        Zoom,         ///< 图像按照原来的比例缩放
    };

    /**
     * @brief 用于显示一个位图的控件
     */
    class BmpBox : public StaticControl
    {
    private:
        /**
         * @brief 位图句柄
         */
        HBITMAP _hBitmap{NULL};

        /**
         * @brief 位图尺寸（以像素为单位），调用_UpdateBmpSize更新
         */
        SIZE _bmpSize{0, 0};

        /**
         * @brief 位图定位方式
         */
        BmpBoxSizeMode _sizeMode{BmpBoxSizeMode::Normal};

    public:
        /**
         * @brief 当前控件显示的位图句柄，使用Load函数可以加载位图
         */
        const ReadOnlyProperty<HBITMAP> BmpHandle;

        /**
         * @brief 位图在BmpBox中的定位方式
         */
        const Property<BmpBoxSizeMode> SizeMode;

    public:
        /**
         * @brief 初始化BmpBox
         */
        BmpBox();

        /**
         * @brief 析构函数
         */
        ~BmpBox();

        /**
         * @brief 加载位图，该函数会复制一个位图句柄作为显示的位图
         * @param hBitmap 要加载的位图
         * @return 加载到BmpBox的位图句柄，若加载失败则返回NULL，该资源由BmpBox内部管理，在加载新位图或控件销毁时会自动释放
         */
        HBITMAP Load(HBITMAP hBitmap);

        /**
         * @brief 从指定模块中加载位图
         * @param hInstance DLL或EXE的模块句柄
         * @param resourceId 位图的资源序号
         * @return 加载到BmpBox的位图句柄，若加载失败则返回NULL，该资源由BmpBox内部管理，在加载新位图或控件销毁时会自动释放
         */
        HBITMAP Load(HINSTANCE hInstance, int resourceId);

        /**
         * @brief 从文件加载位图
         * @param fileName 位图文件的路径
         * @return 加载到BmpBox的位图句柄，若加载失败则返回NULL，该资源由BmpBox内部管理，在加载新位图或控件销毁时会自动释放
         */
        HBITMAP Load(const std::wstring &fileName);

        /**
         * @brief 清除加载的位图
         */
        void Clear();

        /**
         * @brief 调整控件尺寸为位图尺寸，未加载位图时该函数不生效
         */
        void SizeToImage();

    protected:
        /**
         * @brief 接收到WM_DESTROY时调用该函数
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnDestroy() override;

        /**
         * @brief 接收到WM_PAINT时调用该函数
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnPaint() override;

        /**
         * @brief 接收到WM_SIZE时调用该函数
         * @param newClientSize 改变后的用户区尺寸
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnSize(const Size &newClientSize) override;

        /**
         * @brief 接收到WM_ERASEBKGND时调用该函数
         * @param hdc 设备上下文句柄
         * @param result 若已处理该消息则设为非零值，默认值为0
         * @return 若返回true则将result作为消息的返回值，否则使用DefaultWndProc的返回值
         */
        virtual bool OnEraseBackground(HDC hdc, LRESULT &result) override;

        /**
         * @brief 测量元素所需尺寸，无需考虑边框和边距
         * @param availableSize 可用的尺寸
         * @return 返回元素需要占用的尺寸
         */
        virtual Size MeasureOverride(const Size &availableSize) override;

    private:
        /**
         * @brief 更新_bmpSize
         */
        void _UpdateBmpSize();

        /**
         * @brief 设置位图
         * @param hBitmap 位图句柄
         */
        void _SetBmp(HBITMAP hBitmap);

        /**
         * @brief 传入的位图不为NULL时调用_SetBmp
         * @param hBitmap 位图句柄
         * @return 传入的位图
         */
        HBITMAP _SetBmpIfNotNull(HBITMAP hBitmap);
    };
}

// Button.h


namespace sw
{
    /**
     * @brief 按钮
     */
    class Button : public ButtonBase
    {
    public:
        /**
         * @brief 初始化按钮
         */
        Button();

    protected:
        /**
         * @brief 更新按钮样式
         * @param focused 是否处于焦点状态
         */
        virtual void UpdateButtonStyle(bool focused);

        /**
         * @brief 接收到WM_SETFOCUS时调用该函数
         * @param hPreFocus 丢失焦点的hwnd，可能为NULL
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnSetFocus(HWND hPreFocus) override;

        /**
         * @brief 接收到WM_KILLFOCUS时调用该函数
         * @param hNextFocus 接收到焦点的hwnd，可能为NULL
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnKillFocus(HWND hNextFocus) override;

        /**
         * @brief 接收到WM_KEYDOWN时调用该函数
         * @param key 虚拟按键
         * @param flags 附加信息
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnKeyDown(VirtualKey key, const KeyFlags &flags) override;
    };
}

// Canvas.h


namespace sw
{
    /**
     * @brief 一种可以为子元素设置绝对位置的面板，与普通Panel不同的是Canvas支持自动滚动条
     */
    class Canvas : public Panel
    {
    private:
        /**
         * @brief 默认布局对象
         */
        CanvasLayout _canvasLayout{};

    public:
        /**
         * @brief 初始化Canvas
         */
        Canvas();

        /**
         * @brief 获取指定元素的布局标记
         */
        static CanvasLayoutTag GetCanvasLayoutTag(UIElement &element);

        /**
         * @brief 给指定元素设置布局标记
         */
        static void SetCanvasLayoutTag(UIElement &element, const CanvasLayoutTag &tag);

    protected:
        /**
         * @brief 获取默认布局对象
         */
        virtual LayoutHost *GetDefaultLayout() override final;
    };
}

// CheckableButton.h


namespace sw
{
    /**
     * @brief 选中状态枚举类型
     */
    enum class CheckState {
        Unchecked     = BST_UNCHECKED,     ///< 未选中
        Checked       = BST_CHECKED,       ///< 已选中
        Indeterminate = BST_INDETERMINATE, ///< 不确定状态
    };

    /**
     * @brief 表示可选中的按钮类型（单选框、复选框等）
     */
    class CheckableButton : public ButtonBase
    {
    public:
        /**
         * @brief 选中状态
         */
        const Property<sw::CheckState> CheckState;

        /**
         * @brief 是否选中
         */
        const Property<bool> IsChecked;

    public:
        /**
         * @brief 初始化CheckableButton
         */
        CheckableButton();

        /**
         * @brief 析构函数，这里用纯虚函数使该类成为抽象类
         */
        virtual ~CheckableButton() = 0;

    protected:
        /**
         * @brief 被单击时调用该函数
         */
        virtual void OnClicked() override;

        /**
         * @brief 被双击时调用该函数
         */
        virtual void OnDoubleClicked() override;
    };
}

// ComboBox.h


namespace sw
{
    /**
     * @brief 组合框
     */
    class ComboBox : public ItemsControl
    {
    private:
        /**
         * @brief 基类别名，方便调用基类函数
         */
        using TBase = ItemsControl;

        /**
         * @brief 当控件为可编辑时标记文本是否被用户修改过
         */
        bool _isTextChanged = false;

        /**
         * @brief 组合框的默认数据源
         */
        ObservableCollection<std::wstring> _items;

    public:
        /**
         * @brief 列表框的子项集合，当未设置ItemsSource时使用该集合作为数据源
         */
        const ReadOnlyProperty<ObservableCollection<std::wstring> *> Items;

        /**
         * @brief 组合框内容是否可编辑
         */
        const Property<bool> IsEditable;

    public:
        /**
         * @brief 初始化组合框
         */
        ComboBox();

        /**
         * @brief 刷新控件以反映数据源的当前状态
         */
        void Refresh();

        /**
         * @brief 显示下拉列表
         */
        void ShowDropDown();

        /**
         * @brief 关闭下拉列表
         */
        void CloseDropDown();

    protected:
        /**
         * @brief 获取默认数据源，当ItemsSource未设置时使用该数据源
         * @return 默认数据源的IList指针，若无默认数据源则返回nullptr
         * @note 子类应确保返回的IList在ItemsControl生命周期内始终有效，且保证多次调用返回同一指针
         */
        virtual IList *GetDefaultItemsSource() override final;

        /**
         * @brief 当前数据源改变时调用该函数
         * @param oldItemsSource 旧的数据源
         * @param newItemsSource 新的数据源
         */
        virtual void OnCurrentItemsSourceChanged(IList *oldItemsSource, IList *newItemsSource) override;

        /**
         * @brief 当数据源集合发生变更时调用该函数
         * @param args 包含集合变更信息的事件参数
         */
        virtual void OnCurrentItemsSourceCollectionChanged(const NotifyCollectionChangedEventArgs &args) override;

        /**
         * @brief 选中项的索引，当无选中项时为-1
         */
        virtual int GetSelectedIndex() override;

        /**
         * @brief 设置选中项索引
         */
        virtual void SetSelectedIndex(int index) override;

        /**
         * @brief 获取窗口文本
         * @return 编辑框的文本内容
         */
        virtual std::wstring &GetInternalText() override;

        /**
         * @brief 当父窗口接收到控件的WM_COMMAND时调用该函数
         * @param code 通知代码
         */
        virtual void OnCommand(int code) override;

        /**
         * @brief 选中项改变时调用该函数
         */
        virtual void OnSelectionChanged() override;

        /**
         * @brief 获取子项要显示的文本
         * @param index 子项索引
         * @param item 包含子项数据的Variant对象
         */
        virtual std::wstring GetDisplayText(int index, const Variant &item);

    private:
        /**
         * @brief 根据当前选中项更新组合框的文本内容
         */
        void _UpdateSelectedText();

        /**
         * @brief 根据数据源更新组合框的项内容
         */
        void _UpdateItems();

        /**
         * @brief 发送CB_ADDSTRING消息添加一个字符串项
         * @param str 要添加的字符串
         */
        void _AddString(const std::wstring &str);

        /**
         * @brief 发送CB_INSERTSTRING消息在指定索引处插入一个字符串项
         * @param index 要插入的项的索引
         * @param str 要插入的字符串
         */
        void _InsertString(int index, const std::wstring &str);

        /**
         * @brief 发送CB_DELETESTRING消息删除指定索引处的项
         * @param index 要删除的项的索引
         */
        void _DeleteString(int index);
    };
}

// CommandLink.h


namespace sw
{
    /**
     * @brief 命令链接按钮
     */
    class CommandLink : public ButtonBase
    {
    public:
        /**
         * @brief 描述文字
         */
        const Property<std::wstring> NoteText;

        /**
         * @brief 初始化按钮
         */
        CommandLink();

    protected:
        /**
         * @brief 更新按钮样式
         * @param focused 是否处于焦点状态
         */
        virtual void UpdateButtonStyle(bool focused);

        /**
         * @brief 接收到WM_SETFOCUS时调用该函数
         * @param hPreFocus 丢失焦点的hwnd，可能为NULL
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnSetFocus(HWND hPreFocus) override;

        /**
         * @brief 接收到WM_KILLFOCUS时调用该函数
         * @param hNextFocus 接收到焦点的hwnd，可能为NULL
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnKillFocus(HWND hNextFocus) override;

        /**
         * @brief 接收到WM_KEYDOWN时调用该函数
         * @param key 虚拟按键
         * @param flags 附加信息
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnKeyDown(VirtualKey key, const KeyFlags &flags) override;
    };
}

// DockPanel.h


namespace sw
{
    /**
     * @brief 停靠面板
     */
    class DockPanel : public Panel
    {
    private:
        /**
         * @brief 默认布局对象
         */
        DockLayout _dockLayout{};

    public:
        /**
         * @brief 最后一个子元素是否填充剩余空间
         */
        const Property<bool> LastChildFill;

    public:
        /**
         * @brief 初始化DockPanel
         */
        DockPanel();

        /**
         * @brief 获取指定元素的Dock
         */
        static DockLayoutTag GetDock(UIElement &element);

        /**
         * @brief 设置指定元素的Dock
         */
        static void SetDock(UIElement &element, DockLayoutTag dock);

    protected:
        /**
         * @brief 获取默认布局对象
         */
        virtual LayoutHost *GetDefaultLayout() override final;
    };
}

// DockSplitter.h


namespace sw
{
    /**
     * @brief 用于在DockLayout布局中调整停靠元素大小的分隔条
     */
    class DockSplitter : public Splitter
    {
    private:
        /**
         * @brief 基类别名，方便调用基类函数实现
         */
        using TBase = Splitter;

        /**
         * @brief 水平分隔条的鼠标样式
         */
        HCURSOR _hCurHorz = NULL;

        /**
         * @brief 垂直分隔条的鼠标样式
         */
        HCURSOR _hCurVert = NULL;

        /**
         * @brief 相关联的停靠元素
         */
        UIElement *_relatedElement = nullptr;

        /**
         * @brief 拖动开始时鼠标的位置
         */
        sw::Point _initialMousePos{};

        /**
         * @brief 拖动开始时相关联的停靠元素的位置和尺寸
         */
        sw::Size _initialRelatedElementSize{};

    public:
        /**
         * @brief 初始化DockSplitter
         */
        DockSplitter();

        /**
         * @brief 取消拖动分隔条
         * @param restoreSize 是否将相关联的元素恢复到拖动开始时的大小
         */
        void CancelDrag(bool restoreSize = false);

    protected:
        /**
         * @brief 接收到WM_LBUTTONDOWN时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState 指示某些按键是否按下
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseLeftButtonDown(const Point &mousePosition, MouseKey keyState) override;

        /**
         * @brief 接收到WM_LBUTTONUP时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState 指示某些按键是否按下
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseLeftButtonUp(const Point &mousePosition, MouseKey keyState) override;

        /**
         * @brief 接收到WM_MOUSEMOVE时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState 指示某些按键是否按下
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseMove(const Point &mousePosition, MouseKey keyState) override;

        /**
         * @brief 接收到WM_KILLFOCUS时调用该函数
         * @param hNextFocus 接收到焦点的hwnd，可能为NULL
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnKillFocus(HWND hNextFocus) override;

        /**
         * @brief 接收到WM_KEYDOWN时调用该函数
         * @param key 虚拟按键
         * @param flags 附加信息
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnKeyDown(VirtualKey key, const KeyFlags &flags) override;

        /**
         * @brief 接收到WM_SETCURSOR消息时调用该函数
         * @param hwnd 鼠标所在窗口的句柄
         * @param hitTest hit-test的结果，详见WM_NCHITTEST消息的返回值
         * @param message 触发该事件的鼠标消息，如WM_MOUSEMOVE
         * @param result 消息的返回值，默认为false
         * @return 若返回true则将result作为消息的返回值，否则使用DefaultWndProc的返回值
         */
        virtual bool OnSetCursor(HWND hwnd, HitTestResult hitTest, int message, bool &result) override;

    private:
        /**
         * @brief 当开始拖动分隔条时调用该函数
         */
        void _OnStartDrag();

        /**
         * @brief 当结束拖动分隔条时调用该函数
         */
        void _OnEndDrag(bool restoreSize);

        /**
         * @brief 当拖动分隔条时调用该函数
         */
        void _OnDragMove();

        /**
         * @brief 更新相关联的停靠元素
         */
        void _UpdateRelatedElement();

        /**
         * @brief 在指定父元素中查找指定布局标记的前一个元素
         */
        UIElement *_FindPreviousElement(UIElement *parent, int startIndex, sw::DockLayoutTag tag);
    };
}

// Grid.h


namespace sw
{
    /**
     * @brief 由列和行组成的灵活的网格区域
     */
    class Grid : public Panel
    {
    private:
        /**
         * @brief 默认布局对象
         */
        GridLayout _gridLayout{};

    public:
        /**
         * @brief 行定义
         */
        const ReadOnlyProperty<ObservableCollection<GridRow> *> Rows;

        /**
         * @brief 列定义
         */
        const ReadOnlyProperty<ObservableCollection<GridColumn> *> Columns;

    public:
        /**
         * @brief 初始化Grid
         */
        Grid();

        /**
         * @brief 设置行信息
         */
        void SetRows(std::initializer_list<GridRow> rows);

        /**
         * @brief 设置列信息
         */
        void SetColumns(std::initializer_list<GridColumn> cols);

        /**
         * @brief 获取指定元素的网格布局标记
         */
        static GridLayoutTag GetGridLayoutTag(UIElement &element);

        /**
         * @brief 给指定元素设置网格布局标记
         */
        static void SetGridLayoutTag(UIElement &element, const GridLayoutTag &tag);

    protected:
        /**
         * @brief 获取默认布局对象
         */
        virtual LayoutHost *GetDefaultLayout() override final;

    private:
        /**
         * @brief 行或列定义变更事件处理函数
         * @param sender 事件发送者
         * @param args 事件参数
         */
        void _RowsColumnsCollectionChangedHandler(
            INotifyCollectionChanged &sender, NotifyCollectionChangedEventArgs &args);
    };
}

// GroupBox.h


namespace sw
{
    /**
     * @brief 组合框
     */
    class GroupBox : public Panel
    {
    private:
        /**
         * @brief 基类别名，方便调用基类函数
         */
        using TBase = Panel;

        /**
         * @brief 标题文本所需大小
         */
        SIZE _textSize;

        /**
         * @brief 默认布局方式
         */
        std::unique_ptr<LayoutHost> _defaultLayout;

    public:
        /**
         * @brief 初始化组合框
         */
        GroupBox();

    protected:
        /**
         * @brief 获取默认布局对象
         */
        virtual LayoutHost *GetDefaultLayout() override;

        /**
         * @brief 绘制边框
         * @param hdc 绘制设备句柄，可能为NULL
         * @param rect 绘制边框的矩形区域，该函数会减去边框厚度
         * @note 若hdc为NULL则不进行绘制，只更新rect
         */
        virtual void OnDrawBorder(HDC hdc, RECT &rect) override;

        /**
         * @brief Text属性更改时调用此函数
         */
        virtual void OnTextChanged() override;

        /**
         * @brief 字体改变时调用该函数
         * @param hfont 字体句柄
         */
        virtual void FontChanged(HFONT hfont) override;

        /**
         * @brief 接收到WM_SIZE时调用该函数
         * @param newClientSize 改变后的用户区尺寸
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnSize(const Size &newClientSize) override;

        /**
         * @brief 设置背景颜色
         * @param color 要设置的颜色
         * @param redraw 是否重绘
         */
        virtual void OnSetBackColor(Color color, bool redraw) override;

        /**
         * @brief 设置文本颜色
         * @param color 要设置的颜色
         * @param redraw 是否重绘
         */
        virtual void OnSetTextColor(Color color, bool redraw) override;

    public:
        /**
         * @brief 设置默认布局方式
         */
        template <typename TLayout>
        auto SetLayout()
            -> typename std::enable_if<std::is_base_of<LayoutHost, TLayout>::value>::type
        {
            auto layout = std::make_unique<TLayout>();
            layout->Associate(this);
            _defaultLayout = std::move(layout);
            InvalidateMeasure();
        }

        /**
         * @brief 取消通过SetLayout设置的布局方式
         */
        template <std::nullptr_t>
        void SetLayout()
        {
            _defaultLayout.reset(nullptr);
            InvalidateMeasure();
        }

    private:
        /**
         * @brief 更新文本大小
         */
        void _UpdateTextSize();
    };
}

// HwndHost.h


namespace sw
{
    /**
     * @brief 将Win32 window托管为SimpleWindow控件
     */
    class HwndHost : public StaticControl
    {
    private:
        /**
         * @brief 基类别名，方便调用基类函数
         */
        using TBase = StaticControl;

        /**
         * @brief 托管的句柄
         */
        HWND _hWindowCore{NULL};

        /**
         * @brief 是否自动填充托管的内容
         */
        bool _fillContent = true;

        /**
         * @brief 是否同步字体
         */
        bool _syncFont = true;

    public:
        /**
         * @brief 是否自动填充托管的内容
         */
        const Property<bool> FillContent;

        /**
         * @brief 是否发送WM_SETFONT消息以同步字体
         */
        const Property<bool> SyncFont;

    public:
        /**
         * @brief 创建HwndHost对象
         */
        HwndHost();

        /**
         * @brief 析构函数
         */
        ~HwndHost();

    protected:
        /**
         * @brief 子类需要调用该函数以初始化HwndHost，该函数会调用BuildWindowCore
         */
        void InitHwndHost();

        /**
         * @brief 字体改变时调用该函数
         * @param hfont 字体句柄
         */
        virtual void FontChanged(HFONT hfont) override;

        /**
         * @brief 接收到WM_SIZE时调用该函数
         * @param newClientSize 改变后的用户区尺寸
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnSize(const Size &newClientSize) override;

        /**
         * @brief 接收到WM_DESTROY时调用该函数
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnDestroy() override;

    private:
        /**
         * @brief 同步托管窗口尺寸
         */
        void _SyncSize(const SIZE &newSize);

        /**
         * @brief 同步托管窗口字体
         */
        void _SyncFont(HFONT hfont);

    protected:
        /**
         * @brief 初始化HwndHost时会调用该函数，需在该函数中创建要被托管的窗口句柄，设置其父窗口并返回被托管的句柄
         * @param hParent 需要给被托管窗口句柄设置的父窗口句柄
         * @return 被托管窗口句柄
         */
        virtual HWND BuildWindowCore(HWND hParent) = 0;

        /**
         * @brief HwndHost被销毁时会调用该函数来销毁被托管的窗口句柄
         * @param hwnd 被托管窗口句柄
         */
        virtual void DestroyWindowCore(HWND hwnd) = 0;
    };
}

// IconBox.h


namespace sw
{
    /**
     * @brief 用于显示一个图标的控件
     */
    class IconBox : public StaticControl
    {
    private:
        /**
         * @brief 图标句柄
         */
        HICON _hIcon{NULL};

    public:
        /**
         * @brief 当前控件显示的图标句柄，使用Load函数可以加载图标
         */
        const ReadOnlyProperty<HICON> IconHandle;

        /**
         * @brief 是否拉伸图标以填充整个控件，值为true时成功调用Load函数会自动调整尺寸为图标尺寸，反之则不会调整尺寸
         */
        const Property<bool> StretchIcon;

    public:
        /**
         * @brief 初始化IconBox
         */
        IconBox();

        /**
         * @brief 析构函数
         */
        ~IconBox();

        /**
         * @brief 加载图标，该函数会复制一个图标句柄作为显示的图标
         * @param hIcon 要加载的图标
         * @return 加载到IconBox的图标句柄，若加载失败则返回NULL，该资源由IconBox内部管理，在加载新图标或控件销毁时会自动释放
         */
        HICON Load(HICON hIcon);

        /**
         * @brief 加载系统标准图标
         * @param icon 图标样式
         * @return 加载到IconBox的图标句柄，若加载失败则返回NULL，该资源由IconBox内部管理，在加载新图标或控件销毁时会自动释放
         */
        HICON Load(StandardIcon icon);

        /**
         * @brief 从指定模块中加载图标
         * @param hInstance DLL或EXE的模块句柄
         * @param resourceId 图标的资源序号
         * @return 加载到IconBox的图标句柄，若加载失败则返回NULL，该资源由IconBox内部管理，在加载新图标或控件销毁时会自动释放
         */
        HICON Load(HINSTANCE hInstance, int resourceId);

        /**
         * @brief 从文件加载图标
         * @param fileName 图标文件的路径
         * @return 加载到IconBox的图标句柄，若加载失败则返回NULL，该资源由IconBox内部管理，在加载新图标或控件销毁时会自动释放
         */
        HICON Load(const std::wstring &fileName);

        /**
         * @brief 清除加载的图标
         */
        void Clear();

        /**
         * @brief 调整控件尺寸为图标尺寸，未加载图标时该函数不生效
         */
        void SizeToIcon();

    protected:
        /**
         * @brief 接收到WM_DESTROY时调用该函数
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnDestroy() override;

    private:
        /**
         * @brief 设置图标
         * @param hIcon 图标句柄
         */
        void _SetIcon(HICON hIcon);

        /**
         * @brief 传入的图标不为NULL时调用_SetIcon
         * @param hIcon 图标句柄
         * @return 传入的图标
         */
        HICON _SetIconIfNotNull(HICON hIcon);
    };
}

// Label.h


namespace sw
{
    /**
     * @brief 文本过长时末尾的处理方式
     */
    enum class TextTrimming {
        None,         ///< 不处理
        EndEllipsis,  ///< 按字符截断并显示“...”
        WordEllipsis, ///< 按单词截断并显示“...”
    };

    /**
     * @brief 标签
     */
    class Label : public StaticControl
    {
    private:
        /**
         * @brief 文本所需的尺寸
         */
        Size _textSize;

        /**
         * @brief 是否根据文本自动调整尺寸
         */
        bool _autoSize = true;

    public:
        /**
         * @brief 文本的水平对齐方式，可设为左对齐、中心对齐、右对齐
         */
        const Property<sw::HorizontalAlignment> HorizontalContentAlignment;

        /**
         * @brief 文本的垂直对齐方式，仅支持顶部对齐和中心对齐，当使用中心对齐时会使自动换行失效
         */
        const Property<sw::VerticalAlignment> VerticalContentAlignment;

        /**
         * @brief 文本过长时末尾的处理方式，当使用截断时水平对齐和自动换行将失效
         */
        const Property<sw::TextTrimming> TextTrimming;

        /**
         * @brief 是否自动换行
         */
        const Property<bool> AutoWrap;

        /**
         * @brief 是否根据文本自动调整尺寸
         */
        const Property<bool> AutoSize;

    public:
        /**
         * @brief 初始化标签
         */
        Label();

    protected:
        /**
         * @brief 接收到WM_SIZE时调用该函数
         * @param newClientSize 改变后的用户区尺寸
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnSize(const Size &newClientSize) override;

        /**
         * @brief Text属性更改时调用此函数
         */
        virtual void OnTextChanged() override;

        /**
         * @brief 字体改变时调用该函数
         * @param hfont 字体句柄
         */
        virtual void FontChanged(HFONT hfont) override;

        /**
         * @brief 测量元素所需尺寸，无需考虑边框和边距
         * @param availableSize 可用的尺寸
         * @return 返回元素需要占用的尺寸
         */
        virtual Size MeasureOverride(const Size &availableSize) override;

    private:
        /**
         * @brief 更新_textSize
         */
        void _UpdateTextSize();

        /**
         * @brief 调整尺寸为_textSize
         */
        void _ResizeToTextSize();

        /**
         * @brief 更新LayoutUpdateCondition属性
         */
        void _UpdateLayoutFlags();

        /**
         * @brief 读取HorizontalContentAlignment属性时调用
         */
        sw::HorizontalAlignment _GetHorzContentAlignment();

        /**
         * @brief 写入HorizontalContentAlignment属性时调用
         */
        void _SetHorzContentAlignment(sw::HorizontalAlignment value);

        /**
         * @brief 读取VerticalContentAlignment属性时调用
         */
        sw::VerticalAlignment _GetVertContentAlignment();

        /**
         * @brief 写入VerticalContentAlignment属性时调用
         */
        void _SetVertContentAlignment(sw::VerticalAlignment value);

        /**
         * @brief 读取TextTrimming属性时调用
         */
        sw::TextTrimming _GetTextTrimming();

        /**
         * @brief 写入TextTrimming属性时调用
         */
        void _SetTextTrimming(sw::TextTrimming value);
    };
}

// ListBox.h


namespace sw
{
    /**
     * @brief 列表框
     */
    class ListBox : public ItemsControl
    {
    private:
        /**
         * @brief 基类别名，方便调用基类函数
         */
        using TBase = ItemsControl;

        /**
         * @brief 列表框的默认数据源
         */
        ObservableCollection<std::wstring> _items;

        /**
         * @brief 列表框子项的高度，负值表示根据字体自动计算高度
         */
        double _itemHeight = -1;

        /**
         * @brief 选中项的背景颜色
         */
        Color _selectedItemBackColor{GetSysColor(COLOR_HIGHLIGHT)};

        /**
         * @brief 选中项的文本颜色
         */
        Color _selectedItemTextColor{GetSysColor(COLOR_HIGHLIGHTTEXT)};

    public:
        /**
         * @brief 列表框的子项集合，当未设置ItemsSource时使用该集合作为数据源
         */
        const ReadOnlyProperty<ObservableCollection<std::wstring> *> Items;

        /**
         * @brief 列表框子项的高度，负值表示根据字体自动计算高度
         */
        const Property<double> ItemHeight;

        /**
         * @brief 当前列表框页面第一个子项的索引
         */
        const Property<int> TopIndex;

        /**
         * @brief 选中项的背景颜色
         */
        const Property<Color> SelectedItemBackColor;

        /**
         * @brief 选中项的文本颜色
         */
        const Property<Color> SelectedItemTextColor;

    public:
        /**
         * @brief 初始化列表框
         */
        ListBox();

        /**
         * @brief 刷新控件以反映数据源的当前状态
         */
        void Refresh();

        /**
         * @brief 获取指定点处子项的索引
         * @param point 相对于列表框用户区左上角点的位置
         */
        int GetItemIndexFromPoint(const Point &point);

    protected:
        /**
         * @brief 获取默认数据源，当ItemsSource未设置时使用该数据源
         * @return 默认数据源的IList指针，若无默认数据源则返回nullptr
         * @note 子类应确保返回的IList在ItemsControl生命周期内始终有效，且保证多次调用返回同一指针
         */
        virtual IList *GetDefaultItemsSource() override final;

        /**
         * @brief 当前数据源改变时调用该函数
         * @param oldItemsSource 旧的数据源
         * @param newItemsSource 新的数据源
         */
        virtual void OnCurrentItemsSourceChanged(IList *oldItemsSource, IList *newItemsSource) override;

        /**
         * @brief 当数据源集合发生变更时调用该函数
         * @param args 包含集合变更信息的事件参数
         */
        virtual void OnCurrentItemsSourceCollectionChanged(const NotifyCollectionChangedEventArgs &args) override;

        /**
         * @brief 选中项的索引，当无选中项时为-1
         */
        virtual int GetSelectedIndex() override;

        /**
         * @brief 设置选中项索引
         */
        virtual void SetSelectedIndex(int index) override;

        /**
         * @brief 字体改变时调用该函数
         * @param hfont 字体句柄
         */
        virtual void FontChanged(HFONT hfont) override;

        /**
         * @brief 当父窗口接收到控件的WM_COMMAND时调用该函数
         * @param code 通知代码
         */
        virtual void OnCommand(int code) override;

        /**
         * @brief 接收到WM_CONTEXTMENU后调用目标控件的该函数
         * @param isKeyboardMsg 消息是否由按下快捷键（Shift+F10、VK_APPS）产生
         * @param mousePosition 鼠标在屏幕中的位置
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnContextMenu(bool isKeyboardMsg, const Point &mousePosition) override;

        /**
         * @brief 父窗口接收到WM_DRAWITEM后且父窗口OnDrawItem函数返回false时调用发出通知控件的该函数
         * @param pDrawItem 包含有关要绘制的项和所需绘图类型的信息的结构体指针
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnDrawItemSelf(DRAWITEMSTRUCT *pDrawItem) override;

        /**
         * @brief 获取子项要显示的文本
         * @param index 子项索引
         * @param item 包含子项数据的Variant对象
         */
        virtual std::wstring GetDisplayText(int index, const Variant &item);

    private:
        /**
         * @brief 设置子项数量
         * @param count 指定列表框中的新项计数
         */
        void _SetCount(int count);

        /**
         * @brief 更新子项数量
         */
        void _UpdateCount();

        /**
         * @brief 更新子项高度
         */
        void _UpdateItemHeight();
    };
}

// ListView.h


namespace sw
{
    /**
     * @brief 列表视图的图像列表枚举
     */
    enum class ListViewImageList {
        Normal      = LVSIL_NORMAL,      ///< 包含大图标的图像列表
        Small       = LVSIL_SMALL,       ///< 包含小图标的图像列表
        State       = LVSIL_STATE,       ///< 包含状态图像的图像列表
        GroupHeader = LVSIL_GROUPHEADER, ///< 组标头的图像列表
    };

    /**
     * @brief 列表视图的列对齐方式
     */
    enum class ListViewColumnAlignment {
        Left   = LVCFMT_LEFT,   ///< 左对齐
        Right  = LVCFMT_RIGHT,  ///< 右对齐
        Center = LVCFMT_CENTER, ///< 居中
    };

    /**
     * @brief 列表视图的列信息
     */
    struct ListViewColumn {
        /// @brief 列标题
        std::wstring header;

        /// @brief 列宽度
        double width;

        /// @brief 关联图像在图像列表中的索引，-1表示无图像
        int imageIndex;

        /// @brief 列对齐方式
        ListViewColumnAlignment alignment;

        /// @brief 构造函数，默认宽度100，左对齐
        ListViewColumn(const wchar_t *header, double width = 100,
                       ListViewColumnAlignment alignment = ListViewColumnAlignment::Left);

        /// @brief 构造函数，默认宽度100，左对齐
        ListViewColumn(const std::wstring &header, double width = 100,
                       ListViewColumnAlignment alignment = ListViewColumnAlignment::Left);
    };

    /**
     * @brief 列表视图的子项信息
     */
    struct ListViewItem {
        /// @brief 子项文本列表，索引0为主项文本
        List<std::wstring> subItems;

        /// @brief 关联图像在图像列表中的索引，-1表示无图像
        int imageIndex = -1;

        /// @brief 复选框选中状态
        bool checked = false;

        /// @brief 默认构造函数
        ListViewItem() = default;

        /// @brief 默认拷贝构造函数
        ListViewItem(const ListViewItem &other) = default;

        /// @brief 移动构造函数
        ListViewItem(ListViewItem &&other) noexcept;

        /// @brief 使用初始化列表构造
        ListViewItem(std::initializer_list<std::wstring> subItems);
    };

    /**
     * @brief 列表视图
     */
    class ListView : public ItemsControl
    {
    private:
        /**
         * @brief 基类别名，方便调用基类函数
         */
        using TBase = ItemsControl;

        /**
         * @brief 列表项集合
         */
        ObservableCollection<ListViewItem> _items;

        /**
         * @brief 列集合
         */
        ObservableCollection<ListViewColumn> _columns;

        /**
         * @brief 用于给GetDisplayInfo提供显示信息的缓冲对象
         */
        ListViewItem _itemDisplayBuffer;

    public:
        /**
         * @brief 列表项集合，当未设置ItemsSource时使用该集合作为数据源
         */
        const ReadOnlyProperty<ObservableCollection<ListViewItem> *> Items;

        /**
         * @brief 列集合
         */
        const ReadOnlyProperty<ObservableCollection<ListViewColumn> *> Columns;

        /**
         * @brief 是否显示网格线
         */
        const Property<bool> GridLines;

        /**
         * @brief 是否允许多选
         */
        const Property<bool> MultiSelect;

        /**
         * @brief 多选状态下可通过该属性获取选中项的个数
         */
        const ReadOnlyProperty<int> SelectedCount;

        /**
         * @brief 是否在第一列显示复选框
         */
        const Property<bool> CheckBoxes;

        /**
         * @brief 当前列表框页面第一个子项的索引
         */
        const ReadOnlyProperty<int> TopIndex;

        /**
         * @brief 允许将同一图像列表与多个列表视图控件配合使用，控件销毁时若该属性为true则不会销毁图像列表
         */
        const Property<bool> ShareImageLists;

        /**
         * @brief 是否允许编辑
         */
        const Property<bool> Editable;

    public:
        /**
         * @brief 初始化ListView
         */
        ListView();

        /**
         * @brief 刷新控件以反映数据源的当前状态
         * @param refreshColumns 是否刷新列信息
         */
        void Refresh(bool refreshColumns = true);

        /**
         * @brief 获取所有选中项的索引
         */
        List<int> GetSelectedIndices();

        /**
         * @brief 获取指定索引项的复选框是否选中
         */
        bool GetItemCheckState(int index);

        /**
         * @brief 设置指定索引项复选框的选中状态
         */
        void SetItemCheckState(int index, bool value);

        /**
         * @brief 获取指定点处子项的索引
         * @param point 相对于用户区左上角点的位置
         */
        int GetItemIndexFromPoint(const Point &point);

        /**
         * @brief 获取列表视图的图像列表
         * @param imageList 要获取的图像列表类型
         */
        ImageList GetImageList(ListViewImageList imageList);

        /**
         * @brief 设置列表视图的图像列表
         * @param imageList 要设置的图像列表类型
         * @param value 要设置的图像列表的句柄
         * @return 若函数成功则返回之前与控件关联的图像列表，否则返回NULL
         */
        HIMAGELIST SetImageList(ListViewImageList imageList, HIMAGELIST value);

        /**
         * @brief 进入编辑模式，调用该函数前需要将Editable属性设为true
         * @param index 编辑项的索引
         * @return 操作是否成功
         */
        bool EditItem(int index);

        /**
         * @brief 取消编辑
         */
        void CancelEdit();

        /**
         * @brief 确保指定索引的项可见
         * @param index 要确保可见的项的索引
         * @param partialOK 是否允许部分可见，默认为false表示必须完全可见
         * @return 操作是否成功，若index无效或无法确保可见则返回false
         */
        bool EnsureVisible(int index, bool partialOK = false);

    protected:
        /**
         * @brief 获取默认数据源，当ItemsSource未设置时使用该数据源
         * @return 默认数据源的IList指针，若无默认数据源则返回nullptr
         * @note 子类应确保返回的IList在ItemsControl生命周期内始终有效，且保证多次调用返回同一指针
         */
        virtual IList *GetDefaultItemsSource() override final;

        /**
         * @brief 当前数据源改变时调用该函数
         * @param oldItemsSource 旧的数据源
         * @param newItemsSource 新的数据源
         */
        virtual void OnCurrentItemsSourceChanged(IList *oldItemsSource, IList *newItemsSource) override;

        /**
         * @brief 当数据源集合发生变更时调用该函数
         * @param args 包含集合变更信息的事件参数
         */
        virtual void OnCurrentItemsSourceCollectionChanged(const NotifyCollectionChangedEventArgs &args) override;

        /**
         * @brief 选中项的索引，当无选中项时为-1
         */
        virtual int GetSelectedIndex() override;

        /**
         * @brief 设置选中项索引
         */
        virtual void SetSelectedIndex(int index) override;

        /**
         * @brief 设置背景颜色
         * @param color 要设置的颜色
         * @param redraw 是否重绘
         */
        virtual void OnSetBackColor(Color color, bool redraw) override;

        /**
         * @brief 设置文本颜色
         * @param color 要设置的颜色
         * @param redraw 是否重绘
         */
        virtual void OnSetTextColor(Color color, bool redraw) override;

        /**
         * @brief 接收到WM_NOTIFY后调用该函数
         * @param pNMHDR 包含有关通知消息的信息
         * @param result 函数返回值为true时将该值作为消息的返回值，默认值为0
         * @return 若已处理该消息则返回true，否则调用发出通知控件的OnNotified函数，依据其返回值判断是否调用DefaultWndProc
         */
        virtual bool OnNotify(NMHDR *pNMHDR, LRESULT &result) override;

        /**
         * @brief 父窗口接收到WM_NOTIFY后且父窗口OnNotify函数返回false时调用发出通知控件的该函数
         * @param pNMHDR 包含有关通知消息的信息
         * @param result 函数返回值为true时将该值作为消息的返回值
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnNotified(NMHDR *pNMHDR, LRESULT &result) override;

        /**
         * @brief 绘制控件前调用该函数
         * @param pNMCD 包含有关自定义绘制的信息
         * @param result 函数返回值为true时将该值作为NM_CUSTOMDRAW消息的返回值
         * @return 若已完成绘制则返回true，否则返回false以使用默认绘制
         */
        virtual bool OnPrePaint(NMCUSTOMDRAW *pNMCD, LRESULT &result) override;

        /**
         * @brief 绘制子项前调用该函数
         * @param pNMCD 包含有关自定义绘制的信息
         * @param subItem 若消息包含CDDS_SUBITEM标志则该值为true，否则为false
         * @param result 函数返回值为true时将该值作为NM_CUSTOMDRAW消息的返回值
         * @return 若已完成绘制则返回true，否则返回false以使用默认绘制
         */
        virtual bool OnItemPrePaint(NMCUSTOMDRAW *pNMCD, bool subItem, LRESULT &result) override;

        /**
         * @brief 列表项某些属性发生变化时调用该函数
         * @param pNMLV 包含有关列表项变化的信息
         */
        virtual void OnItemChanged(NMLISTVIEW *pNMLV);

        /**
         * @brief 复选框选中状态发生改变
         * @param index 改变项的索引
         */
        virtual void OnCheckStateChanged(int index);

        /**
         * @brief 接收到内部Header控件发送的HDN_ITEMCHANGINGW通知时调用该函数
         * @param pNMH 包含有关列标题变化的信息
         */
        virtual void OnHeaderItemChanging(NMHEADERW *pNMH);

        /**
         * @brief 接收到内部Header控件发送的HDN_ITEMCHANGEDW通知时调用该函数
         * @param pNMH 包含有关列标题变化的信息
         */
        virtual void OnHeaderItemChanged(NMHEADERW *pNMH);

        /**
         * @brief 鼠标左键单击列标题时调用该函数
         * @note 内部Header控件接收到HDN_ITEMCLICKW通知后会调用该函数
         */
        virtual void OnHeaderItemClicked(NMHEADERW *pNMH);

        /**
         * @brief 鼠标左键双击列标题时调用该函数
         * @note 内部Header控件接收到HDN_ITEMDBLCLICKW通知后会调用该函数
         */
        virtual void OnHeaderItemDoubleClicked(NMHEADERW *pNMH);

        /**
         * @brief 鼠标左键单击某一项时调用该函数
         * @note 控件接收到NM_CLICK通知后会调用该函数
         */
        virtual void OnItemClicked(NMITEMACTIVATE *pNMIA);

        /**
         * @brief 鼠标左键双击某一项调用该函数
         * @note 控件接收到NM_DBLCLK通知后会调用该函数
         */
        virtual void OnItemDoubleClicked(NMITEMACTIVATE *pNMIA);

        /**
         * @brief 复选框被点击时调用该函数
         * @param index 被点击项的索引
         */
        virtual void OnCheckBoxClicked(int index);

        /**
         * @brief 编辑状态结束后调用该函数
         * @param args 包含编辑结果的事件参数
         */
        virtual void OnEndEdit(ListViewEndEditEventArgs &args);

        /**
         * @brief 应用编辑结果时调用该函数
         * @param index 编辑项的索引
         * @param newText 编辑后的文本
         */
        virtual void OnApplyEdit(int index, const std::wstring &newText);

        /**
         * @brief 当OnNotified接收到LVN_GETDISPINFOW通知时调用该函数
         * @param pNMInfo 包含有关通知消息的信息
         */
        virtual void OnGetDispInfo(NMLVDISPINFOW *pNMInfo);

        /**
         * @brief 获取指定子项要显示的信息
         * @param index 子项索引
         * @param item 包含子项数据的Variant对象
         * @param pNMInfo 包含有关通知消息的信息，修改该结构体以提供要显示的信息
         */
        virtual void GetDisplayInfo(int index, const Variant &item, NMLVDISPINFOW *pNMInfo);

        /**
         * @brief 获取指定子项要显示的信息，子类可以重写该函数以显示自定义类型的数据
         * @param index 子项索引
         * @param item 包含子项数据的Variant对象
         * @param listViewItem 用于接收要显示信息的ListViewItem结构体，修改该结构体以提供要显示的信息
         * @return 若已提供显示信息则返回true，否则返回false以使用默认显示信息
         */
        virtual bool GetDisplayInfo(int index, const Variant &item, ListViewItem &listViewItem);

        /**
         * @brief 获取指定索引项的复选框矩形
         * @param index 项索引
         * @param rect 用于接收复选框矩形的RECT结构体
         */
        virtual void GetCheckBoxRect(int index, RECT &rect);

        /**
         * @brief 获取指定索引项的复选框是否选中
         * @param index 项索引
         * @param checked 用于接收复选框选中状态的布尔变量
         */
        virtual void OnGetItemCheckState(int index, bool &checked);

        /**
         * @brief 设置指定索引项复选框的选中状态
         * @param index 项索引
         * @param checked 要设置的选中状态
         */
        virtual void OnSetItemCheckState(int index, bool checked);

    private:
        /**
         * @brief 设置子项数量
         * @param count 指定列表视图中的新项计数
         */
        void _SetCount(int count);

        /**
         * @brief 更新子项数量以匹配数据源
         */
        void _UpdateCount();

        /**
         * @brief 获取行数
         */
        int _GetRowCount();

        /**
         * @brief 获取列数
         */
        int _GetColCount();

        /**
         * @brief 获取ListView扩展样式
         */
        DWORD _GetExtendedListViewStyle();

        /**
         * @brief 设置ListView扩展样式
         * @return 先前的样式
         */
        DWORD _SetExtendedListViewStyle(DWORD style);

        /**
         * @brief 同步ListViewItem数据到Windows消息结构体
         * @param item 包含要显示信息的ListViewItem结构体
         * @param pNMInfo 要同步到的NMLVDISPINFOW结构体指针
         */
        void _ApplyDispInfo(const ListViewItem &item, NMLVDISPINFOW *pNMInfo);

        /**
         * @brief 同步ListViewColumn数据到LVCOLUMNW结构体
         * @param column 包含要显示信息的ListViewColumn结构体
         * @param pLvc 要同步到的LVCOLUMNW结构体指针
         */
        void _ApplyColumnInfo(const ListViewColumn &column, LVCOLUMNW *pLvc);

        /**
         * @brief 插入列
         * @param index 列索引
         * @param column 列信息
         */
        bool _InsertColumn(int index, const ListViewColumn &column);

        /**
         * @brief 移除列
         * @param index 列索引
         */
        bool _DeleteColumn(int index);

        /**
         * @brief 设置列信息
         * @param index 列索引
         * @param column 列信息
         */
        bool _SetColumn(int index, const ListViewColumn &column);

        /**
         * @brief 更新列以匹配数据源
         */
        void _UpdateColumns();

        /**
         * @brief 列集合变更处理函数
         * @param sender 事件发送者，即数据源对象
         * @param args 集合变更事件参数
         */
        void _ColumnsCollectionChangedHandler(
            INotifyCollectionChanged &sender, NotifyCollectionChangedEventArgs &args);
    };
};

// PasswordBox.h


namespace sw
{
    /**
     * @brief 密码框
     */
    class PasswordBox : public TextBoxBase
    {
    public:
        /**
         * @brief 密码框显示的字符
         */
        const Property<wchar_t> PasswordChar;

    public:
        /**
         * @brief 初始化密码框
         */
        PasswordBox();
    };
}

// SpinBox.h


namespace sw
{
    /**
     * @brief 数值调节框
     */
    class SpinBox : public TextBoxBase
    {
    private:
        /**
         * @brief UpDown控件的句柄
         */
        HWND _hUpDown = NULL;

        /**
         * @brief 记录UpDown控件的宽度
         */
        int _upDownWidth = 0;

        /**
         * @brief 储存加速信息
         */
        ObservableCollection<UDACCEL> _accels;

    public:
        /**
         * @brief 最小值，默认为0
         */
        const Property<int> Minimum;

        /**
         * @brief 最大值，默认为100
         */
        const Property<int> Maximum;

        /**
         * @brief 当前值，默认为0
         */
        const Property<int> Value;

        /**
         * @brief 是否显示为十六进制，默认为false
         */
        const Property<bool> Hexadecimal;

        /**
         * @brief 每次点击UpDown控件时增量，默认为1
         */
        const Property<uint32_t> Increment;

        /**
         * @brief 加速信息集合，默认为包含一个元素（0秒后增量为1）的集合
         */
        const ReadOnlyProperty<ObservableCollection<UDACCEL> *> Accelerations;

    public:
        /**
         * @brief 初始化数值调节框
         */
        SpinBox();

    protected:
        /**
         * @brief Text属性更改时调用此函数
         */
        virtual void OnTextChanged() override;

        /**
         * @brief 控件句柄发生改变时调用该函数
         * @param hwnd 新的控件句柄
         */
        virtual void OnHandleChanged(HWND hwnd) override;

        /**
         * @brief 接收到WM_MOVE时调用该函数
         * @param newClientPosition 移动后用户区左上角的位置
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMove(const Point &newClientPosition) override;

        /**
         * @brief 接收到WM_SIZE时调用该函数
         * @param newClientSize 改变后的用户区尺寸
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnSize(const Size &newClientSize) override;

        /**
         * @brief 接收到WM_KILLFOCUS时调用该函数
         * @param hNextFocus 接收到焦点的hwnd，可能为NULL
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnKillFocus(HWND hNextFocus) override;

    private:
        /**
         * @brief 初始化
         */
        void _InitSpinBox();

        /**
         * @brief 创建UpDown控件
         */
        void _InitUpDownControl();

        /**
         * @brief 获取当前值
         * @return 当前值
         */
        int _GetPos32();

        /**
         * @brief 设置当前值
         * @param pos 新的值
         */
        void _SetPos32(int pos);

        /**
         * @brief 获取最小值和最大值
         * @param min 用于接收最小值的指针
         * @param max 用于接收最大值的指针
         * @note 若不需要获取某个值则传入nullptr
         */
        void _GetRange32(int *min, int *max);

        /**
         * @brief 设置最小值和最大值
         * @param min 新的最小值
         * @param max 新的最大值
         */
        void _SetRange32(int min, int max);

        /**
         * @brief 设置加速信息
         * @param count 加速信息的数量
         * @param pAccels 指向UDACCEL数组的指针
         */
        void _SetAccel(size_t count, UDACCEL *pAccels);

        /**
         * @brief 更新UpDown控件的位置
         */
        void _UpdateUpDownPos();

        /**
         * @brief Accelerations集合变更时调用该函数
         * @param sender 事件的发送者
         * @param args 包含集合变更信息的事件参数
         */
        void _AccelerationCollectionChangedHandler(
            INotifyCollectionChanged &sender, NotifyCollectionChangedEventArgs &args);
    };
};

// StackPanel.h


namespace sw
{
    /**
     * @brief 堆叠面板
     */
    class StackPanel : public Panel
    {
    private:
        /**
         * @brief 默认布局对象
         */
        StackLayout _stackLayout{};

    public:
        /**
         * @brief 排列方式
         */
        const Property<sw::Orientation> Orientation;

    public:
        /**
         * @brief 初始化StackPanel
         */
        StackPanel();

    protected:
        /**
         * @brief 获取默认布局对象
         */
        virtual LayoutHost *GetDefaultLayout() override final;
    };
}

// TextBox.h


namespace sw
{
    /**
     * @brief 编辑框
     */
    class TextBox : public TextBoxBase
    {
    private:
        /**
         * @brief 是否自动换行
         */
        bool _autoWrap = false;

    public:
        /**
         * @brief 是否自动换行，仅在MultiLine属性为true时有效
         */
        const Property<bool> AutoWrap;

        /**
         * @brief 是否允许多行文本
         */
        const Property<bool> MultiLine;

        /**
         * @brief 是否显示横向滚动条
         */
        const Property<bool> HorizontalScrollBar;

        /**
         * @brief 是否显示纵向滚动条
         */
        const Property<bool> VerticalScrollBar;

    public:
        /**
         * @brief 初始化编辑框
         */
        TextBox();
    };
}

// UniformGrid.h


namespace sw
{
    /**
     * @brief 提供一种在网格（网格中的所有单元格都具有相同的大小）中排列内容的方法
     */
    class UniformGrid : public Panel
    {
    private:
        /**
         * @brief 默认布局对象
         */
        UniformGridLayout _uniformGridLayout{};

    public:
        /**
         * @brief 行数
         */
        const Property<int> Rows;

        /**
         * @brief 列数
         */
        const Property<int> Columns;

        /**
         * @brief 网格第一行中前导空白单元格的数量
         */
        const Property<int> FirstColumn;

    public:
        /**
         * @brief 初始化UniformGrid
         */
        UniformGrid();

    protected:
        /**
         * @brief 获取默认布局对象
         */
        virtual LayoutHost *GetDefaultLayout() override final;
    };
}

// WrapPanel.h


namespace sw
{
    /**
     * @brief 自动换行面板
     */
    class WrapPanel : public Panel
    {
    private:
        /**
         * @brief 默认布局对象
         */
        WrapLayout _wrapLayout{};

    public:
        /**
         * @brief 排列方式
         */
        const Property<sw::Orientation> Orientation;

    public:
        /**
         * @brief 初始化WrapPanel
         */
        WrapPanel();

    protected:
        /**
         * @brief 获取默认布局对象
         */
        virtual LayoutHost *GetDefaultLayout() override final;
    };
}

// CheckBox.h


namespace sw
{
    /**
     * @brief 复选框
     */
    class CheckBox : public CheckableButton
    {
    public:
        /**
         * @brief 是否为三态复选框
         */
        const Property<bool> ThreeState;

    public:
        /**
         * @brief 初始化复选框
         */
        CheckBox();
    };
}

// IPAddressControl.h


namespace sw
{
    /**
     * @brief IP地址框
     */
    class IPAddressControl : public HwndHost
    {
    private:
        /**
         * @brief 基类别名
         */
        using TBase = HwndHost;

        /**
         * @brief IP地址框的句柄
         */
        HWND _hIPAddrCtrl{NULL};

    public:
        /**
         * @brief 内容是否为空
         */
        const ReadOnlyProperty<bool> IsBlank;

        /**
         * @brief 当前内容所表示的IP地址
         */
        const Property<uint32_t> Address;

    public:
        /**
         * @brief 初始化IP地址框
         */
        IPAddressControl();

    public:
        /**
         * @brief 清空输入的内容
         */
        void Clear();

        /**
         * @brief 限制某个IP段的值范围
         * @param field 要限制的IP段的索引
         * @param min 最小值
         * @param max 最大值
         * @return 若函数成功则返回true，否则返回false
         */
        bool SetRange(int field, uint8_t min, uint8_t max);

    protected:
        /**
         * @brief 初始化HwndHost时会调用该函数，需在该函数中创建要被托管的窗口句柄，设置其父窗口并返回被托管的句柄
         * @param hParent 需要给被托管窗口句柄设置的父窗口句柄
         * @return 被托管窗口句柄
         */
        virtual HWND BuildWindowCore(HWND hParent) override;

        /**
         * @brief HwndHost被销毁时会调用该函数来销毁被托管的窗口句柄
         * @param hwnd 被托管窗口句柄
         */
        virtual void DestroyWindowCore(HWND hwnd) override;

        /**
         * @brief 接收到WM_SIZE时调用该函数
         * @param newClientSize 改变后的用户区尺寸
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnSize(const Size &newClientSize) override;

        /**
         * @brief 接收到WM_SETFOCUS时调用该函数
         * @param hPrevFocus 丢失焦点的hwnd，可能为NULL
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnSetFocus(HWND hPrevFocus) override;

        /**
         * @brief 接收到WM_NOTIFY后调用该函数
         * @param pNMHDR 包含有关通知消息的信息
         * @param result 函数返回值为true时将该值作为消息的返回值，默认值为0
         * @return 若已处理该消息则返回true，否则调用发出通知控件的OnNotified函数，依据其返回值判断是否调用DefaultWndProc
         */
        virtual bool OnNotify(NMHDR *pNMHDR, LRESULT &result) override;

        /**
         * @brief 地址改变时调用该函数
         */
        virtual void OnAddressChanged();

    private:
        /**
         * @brief 处理Tab键按下事件
         */
        void _OnTabKeyDown();

        /**
         * @brief 子类化内部编辑框的窗口过程函数
         */
        static LRESULT CALLBACK _FieldsEditSubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    };
}

// RadioButton.h


namespace sw
{
    /**
     * @brief 单选框
     */
    class RadioButton : public CheckableButton
    {
    public:
        /**
         * @brief 初始化单选框
         */
        RadioButton();
    };
}

// SplitButton.h


namespace sw
{
    /**
     * @brief 分割按钮
     * @note 下拉箭头部分单击时默认显示ContextMenu
     */
    class SplitButton : public Button
    {
    public:
        /**
         * @brief 初始化分割按钮
         */
        SplitButton();

        /**
         * @brief 显示下拉菜单
         * @return 若成功显示菜单则返回true，否则返回false
         */
        bool ShowDropDownMenu();

    protected:
        /**
         * @brief 当下拉箭头被单击时调用该函数
         * @param pInfo 包含有关下拉事件的信息
         */
        virtual void OnDropDown(NMBCDROPDOWN *pInfo);

        /**
         * @brief 更新按钮样式
         * @param focused 是否处于焦点状态
         */
        virtual void UpdateButtonStyle(bool focused) override;

        /**
         * @brief 父窗口接收到WM_NOTIFY后且父窗口OnNotify函数返回false时调用发出通知控件的该函数
         * @param pNMHDR 包含有关通知消息的信息
         * @param result 函数返回值为true时将该值作为消息的返回值
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnNotified(NMHDR *pNMHDR, LRESULT &result) override;

        /**
         * @brief 接收到WM_CONTEXTMENU后调用目标控件的该函数
         * @param isKeyboardMsg 消息是否由按下快捷键（Shift+F10、VK_APPS）产生
         * @param mousePosition 鼠标在屏幕中的位置
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnContextMenu(bool isKeyboardMsg, const Point &mousePosition) override;
    };
}

// SimpleWindow.h

/**
 * @namespace sw
 * @brief SimpleWindow框架的顶级命名空间，所有公共类型、控件、枚举和工具函数均定义于此。
 */


#pragma comment(linker, "\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#pragma comment(lib, "comctl32.lib")
