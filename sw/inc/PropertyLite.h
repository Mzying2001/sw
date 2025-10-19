#pragma once

#include "Property.h"
#include <cassert>
#include <limits>

namespace sw
{
    // 向前声明

    template <typename T, typename TDerived>
    class PropertyLiteBase;

    template <typename T>
    class PropertyLite;

    template <typename T>
    class ReadOnlyPropertyLite;

    template <typename T>
    class WriteOnlyPropertyLite;

    /*================================================================================*/

    /**
     * @brief 成员属性初始化器
     */
    template <typename TOwner, typename TValue>
    class MemberPropertyLiteInitializer
    {
        friend class PropertyLite<TValue>;
        friend class ReadOnlyPropertyLite<TValue>;
        friend class WriteOnlyPropertyLite<TValue>;

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
        void (*_setter)(TOwner *, const TValue &);

    public:
        /**
         * @brief 构造成员属性初始化器
         */
        MemberPropertyLiteInitializer(TOwner *owner)
            : _owner(owner), _getter(nullptr), _setter(nullptr)
        {
        }

        /**
         * @brief 设置getter
         */
        MemberPropertyLiteInitializer &Getter(TValue (*getter)(TOwner *))
        {
            _getter = getter;
            return *this;
        }

        /**
         * @brief 设置setter
         */
        MemberPropertyLiteInitializer &Setter(void (*setter)(TOwner *, const TValue &))
        {
            _setter = setter;
            return *this;
        }

        /**
         * @brief 设置成员函数getter
         */
        template <TValue (TOwner::*getter)()>
        MemberPropertyLiteInitializer &Getter()
        {
            return Getter(
                [](TOwner *owner) -> TValue {
                    return (owner->*getter)();
                });
        }

        /**
         * @brief 设置成员函数setter
         */
        template <void (TOwner::*setter)(const TValue &)>
        MemberPropertyLiteInitializer &Setter()
        {
            return Setter(
                [](TOwner *owner, const TValue &value) {
                    (owner->*setter)(value);
                });
        }
    };

    /**
     * @brief 静态属性初始化器
     */
    template <typename TValue>
    class StaticPropertyLiteInitializer
    {
        friend class PropertyLite<TValue>;
        friend class ReadOnlyPropertyLite<TValue>;
        friend class WriteOnlyPropertyLite<TValue>;

    private:
        /**
         * @brief getter函数指针
         */
        TValue (*_getter)();

        /**
         * @brief setter函数指针
         */
        void (*_setter)(const TValue &);

    public:
        /**
         * @brief 构造静态属性初始化器
         */
        StaticPropertyLiteInitializer()
            : _getter(nullptr), _setter(nullptr)
        {
        }

        /**
         * @brief 设置getter
         */
        StaticPropertyLiteInitializer &Getter(TValue (*getter)())
        {
            _getter = getter;
            return *this;
        }

        /**
         * @brief 设置setter
         */
        StaticPropertyLiteInitializer &Setter(void (*setter)(const TValue &))
        {
            _setter = setter;
            return *this;
        }
    };

    /*================================================================================*/

    /**
     * @brief 轻量版属性基类
     * @note  轻量版属性相比普通属性，不再依赖Delegate而是直接使用函数指针实现，更高效且所有者对象可以正常拷贝和移动
     */
    template <typename T, typename TDerived>
    class PropertyLiteBase : public PropertyBase<T, TDerived>
    {
    public:
        /**
         * @brief 基类类型别名
         */
        using TBase = PropertyBase<T, TDerived>;

        /**
         * @brief 继承父类operator=
         */
        using TBase::operator=;

        /**
         * @brief 使用默认构造函数
         */
        PropertyLiteBase() = default;

        /**
         * @brief   恢复被基类删除的拷贝构造函数
         * @warning 不应使用该函数创建新的属性对象，属性应由所有者对象进行初始化
         */
        PropertyLiteBase(const PropertyLiteBase &other) : TBase()
        {
            _offset = other._offset;
        }

        /**
         * @brief   恢复被基类删除的移动构造函数
         * @warning 不应使用该函数创建新的属性对象，属性应由所有者对象进行初始化
         */
        PropertyLiteBase(PropertyLiteBase &&other) : TBase()
        {
            _offset = other._offset;
        }

        /**
         * @brief   恢复被基类删除的拷贝赋值运算符
         * @warning 该函数在任何情况下都不应被调用，仅用于恢复基类接口完整性
         */
        PropertyLiteBase &operator=(PropertyLiteBase &&other)
        {
            // _offset = other._offset;
            return *this;
        }

    protected:
        /**
         * @brief 无效的偏移量值
         */
        static constexpr std::ptrdiff_t _invalidOffset =
            (std::numeric_limits<std::ptrdiff_t>::max)();

        /**
         * @brief 所有者对象相对于当前属性对象的偏移量
         */
        std::ptrdiff_t _offset{_invalidOffset};

        /**
         * @brief 判断属性是否为静态属性
         */
        bool IsStatic() const noexcept
        {
            return _offset == _invalidOffset;
        }

        /**
         * @brief 设置属性所有者对象，nullptr表示静态属性
         */
        void SetOwner(void *owner) noexcept
        {
            if (owner == nullptr) {
                _offset = _invalidOffset;
            } else {
                _offset = reinterpret_cast<uint8_t *>(owner) - reinterpret_cast<uint8_t *>(this);
            }
        }

        /**
         * @brief 获取属性所有者对象，当属性为静态属性时返回nullptr
         */
        void *GetOwner() const noexcept
        {
            if (IsStatic()) {
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
        static MemberPropertyLiteInitializer<TOwner, T> Init(TOwner *owner)
        {
            return MemberPropertyLiteInitializer<TOwner, T>(owner);
        }

        /**
         * @brief 获取静态属性初始化器
         */
        static StaticPropertyLiteInitializer<T> Init()
        {
            return StaticPropertyLiteInitializer<T>();
        }
    };

    /**
     * @brief 属性（轻量版）
     * @note  轻量版属性相比普通属性，不再依赖Delegate而是直接使用函数指针实现，更高效且所有者对象可以正常拷贝和移动
     */
    template <typename T>
    class PropertyLite : public PropertyLiteBase<T, PropertyLite<T>>
    {
    public:
        using TBase         = PropertyLiteBase<T, PropertyLite<T>>;
        using TGetter       = T (*)(void *);
        using TSetter       = void (*)(void *, const T &);
        using TStaticGetter = T (*)();
        using TStaticSetter = void (*)(const T &);

    private:
        /**
         * @brief getter函数指针
         */
        void *_getter;

        /**
         * @brief setter函数指针
         */
        void *_setter;

    public:
        /**
         * @brief 继承父类operator=
         */
        using TBase::operator=;

        /**
         * @brief 构造成员属性
         */
        template <typename TOwner>
        explicit PropertyLite(const MemberPropertyLiteInitializer<TOwner, T> &initializer)
        {
            assert(initializer._owner != nullptr);
            assert(initializer._getter != nullptr);
            assert(initializer._setter != nullptr);

            this->SetOwner(initializer._owner);
            _getter = reinterpret_cast<void *>(initializer._getter);
            _setter = reinterpret_cast<void *>(initializer._setter);
        }

        /**
         * @brief 构造静态属性
         */
        explicit PropertyLite(const StaticPropertyLiteInitializer<T> &initializer)
        {
            assert(initializer._getter != nullptr);
            assert(initializer._setter != nullptr);

            this->SetOwner(nullptr);
            _getter = reinterpret_cast<void *>(initializer._getter);
            _setter = reinterpret_cast<void *>(initializer._setter);
        }

        /**
         * @brief 获取属性值
         */
        T GetterImpl() const
        {
            if (this->IsStatic()) {
                return reinterpret_cast<TStaticGetter>(_getter)();
            } else {
                return reinterpret_cast<TGetter>(_getter)(this->GetOwner());
            }
        }

        /**
         * @brief 设置属性值
         */
        void SetterImpl(const T &value) const
        {
            if (this->IsStatic()) {
                reinterpret_cast<TStaticSetter>(_setter)(value);
            } else {
                reinterpret_cast<TSetter>(_setter)(this->GetOwner(), value);
            }
        }
    };

    /**
     * @brief 只读属性（轻量版)
     * @note  轻量版属性相比普通属性，不再依赖Delegate而是直接使用函数指针实现，更高效且所有者对象可以正常拷贝和移动
     */
    template <typename T>
    class ReadOnlyPropertyLite : public PropertyLiteBase<T, ReadOnlyPropertyLite<T>>
    {
    public:
        using TBase         = PropertyLiteBase<T, ReadOnlyPropertyLite<T>>;
        using TGetter       = T (*)(void *);
        using TStaticGetter = T (*)();

    private:
        /**
         * @brief getter函数指针
         */
        void *_getter;

    public:
        /**
         * @brief 构造成员属性
         */
        template <typename TOwner>
        explicit ReadOnlyPropertyLite(const MemberPropertyLiteInitializer<TOwner, T> &initializer)
        {
            assert(initializer._owner != nullptr);
            assert(initializer._getter != nullptr);

            this->SetOwner(initializer._owner);
            _getter = reinterpret_cast<void *>(initializer._getter);
        }

        /**
         * @brief 构造静态属性
         */
        explicit ReadOnlyPropertyLite(const StaticPropertyLiteInitializer<T> &initializer)
        {
            assert(initializer._getter != nullptr);

            this->SetOwner(nullptr);
            _getter = reinterpret_cast<void *>(initializer._getter);
        }

        /**
         * @brief 获取属性值
         */
        T GetterImpl() const
        {
            if (this->IsStatic()) {
                return reinterpret_cast<TStaticGetter>(_getter)();
            } else {
                return reinterpret_cast<TGetter>(_getter)(this->GetOwner());
            }
        }
    };

    /**
     * @brief 只写属性（轻量版)
     * @note  轻量版属性相比普通属性，不再依赖Delegate而是直接使用函数指针实现，更高效且所有者对象可以正常拷贝和移动
     */
    template <typename T>
    class WriteOnlyPropertyLite : public PropertyLiteBase<T, WriteOnlyPropertyLite<T>>
    {
    public:
        using TBase         = PropertyLiteBase<T, WriteOnlyPropertyLite<T>>;
        using TSetter       = void (*)(void *, const T &);
        using TStaticSetter = void (*)(const T &);

    private:
        /**
         * @brief setter函数指针
         */
        void *_setter;

    public:
        /**
         * @brief 继承父类operator=
         */
        using TBase::operator=;

        /**
         * @brief 构造成员属性
         */
        template <typename TOwner>
        explicit WriteOnlyPropertyLite(const MemberPropertyLiteInitializer<TOwner, T> &initializer)
        {
            assert(initializer._owner != nullptr);
            assert(initializer._setter != nullptr);

            this->SetOwner(initializer._owner);
            _setter = reinterpret_cast<void *>(initializer._setter);
        }

        /**
         * @brief 构造静态属性
         */
        explicit WriteOnlyPropertyLite(const StaticPropertyLiteInitializer<T> &initializer)
        {
            assert(initializer._setter != nullptr);

            this->SetOwner(nullptr);
            _setter = reinterpret_cast<void *>(initializer._setter);
        }

        /**
         * @brief 设置属性值
         */
        void SetterImpl(const T &value) const
        {
            if (this->IsStatic()) {
                reinterpret_cast<TStaticSetter>(_setter)(value);
            } else {
                reinterpret_cast<TSetter>(_setter)(this->GetOwner(), value);
            }
        }
    };
}
