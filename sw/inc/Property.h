#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <utility>

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
    _SW_DEFINE_UNARY_OPERATION_HELPER(_AddrOperationHelper, &);
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
     * @brief 属性setter参数类型辅助模板
     */
    template <typename T>
    struct _PropertySetterParamTypeHelper {
        using type = typename std::conditional<
            std::is_scalar<T>::value, T, const T &>::type;
    };

    /**
     * @brief 属性setter参数类型
     */
    template <typename T>
    using _PropertySetterParamType =
        typename _PropertySetterParamTypeHelper<typename std::decay<T>::type>::type;

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
        typename std::enable_if<std::is_pointer<U>::value, U>::type operator->()
        {
            return this->value;
        }

        /**
         * @brief 非指针类型，且无operator->，返回值的地址
         */
        template <typename U = T>
        typename std::enable_if<!std::is_pointer<U>::value && !_HasArrowOperator<U>::value, U *>::type operator->()
        {
            return &this->value;
        }

        /**
         * @brief 非指针类型，且有operator->，转发operator->
         */
        template <typename U = T>
        typename std::enable_if<!std::is_pointer<U>::value && _HasArrowOperator<U>::value, typename _HasArrowOperator<U>::type>::type operator->()
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
        typename std::enable_if<_AddOperationHelper<T, U>::value, TDerived &>::type operator+=(U &&value)
        {
            this->Set(this->Get() + std::forward<U>(value));
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 加赋值运算
         */
        template <typename U>
        typename std::enable_if<_AddOperationHelper<T, U>::value, const TDerived &>::type operator+=(U &&value) const
        {
            this->Set(this->Get() + std::forward<U>(value));
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 加赋值运算
         */
        template <typename D, typename U>
        typename std::enable_if<_AddOperationHelper<T, U>::value, TDerived &>::type operator+=(const PropertyBase<U, D> &prop)
        {
            this->Set(this->Get() + prop.Get());
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 加赋值运算
         */
        template <typename D, typename U>
        typename std::enable_if<_AddOperationHelper<T, U>::value, const TDerived &>::type operator+=(const PropertyBase<U, D> &prop) const
        {
            this->Set(this->Get() + prop.Get());
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 减赋值运算
         */
        template <typename U>
        typename std::enable_if<_SubOperationHelper<T, U>::value, TDerived &>::type operator-=(U &&value)
        {
            this->Set(this->Get() - std::forward<U>(value));
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 减赋值运算
         */
        template <typename U>
        typename std::enable_if<_SubOperationHelper<T, U>::value, const TDerived &>::type operator-=(U &&value) const
        {
            this->Set(this->Get() - std::forward<U>(value));
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 减赋值运算
         */
        template <typename D, typename U>
        typename std::enable_if<_SubOperationHelper<T, U>::value, TDerived &>::type operator-=(const PropertyBase<U, D> &prop)
        {
            this->Set(this->Get() - prop.Get());
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 减赋值运算
         */
        template <typename D, typename U>
        typename std::enable_if<_SubOperationHelper<T, U>::value, const TDerived &>::type operator-=(const PropertyBase<U, D> &prop) const
        {
            this->Set(this->Get() - prop.Get());
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 乘赋值运算
         */
        template <typename U>
        typename std::enable_if<_MulOperationHelper<T, U>::value, TDerived &>::type operator*=(U &&value)
        {
            this->Set(this->Get() * std::forward<U>(value));
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 乘赋值运算
         */
        template <typename U>
        typename std::enable_if<_MulOperationHelper<T, U>::value, const TDerived &>::type operator*=(U &&value) const
        {
            this->Set(this->Get() * std::forward<U>(value));
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 乘赋值运算
         */
        template <typename D, typename U>
        typename std::enable_if<_MulOperationHelper<T, U>::value, TDerived &>::type operator*=(const PropertyBase<U, D> &prop)
        {
            this->Set(this->Get() * prop.Get());
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 乘赋值运算
         */
        template <typename D, typename U>
        typename std::enable_if<_MulOperationHelper<T, U>::value, const TDerived &>::type operator*=(const PropertyBase<U, D> &prop) const
        {
            this->Set(this->Get() * prop.Get());
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 除赋值运算
         */
        template <typename U>
        typename std::enable_if<_DivOperationHelper<T, U>::value, TDerived &>::type operator/=(U &&value)
        {
            this->Set(this->Get() / std::forward<U>(value));
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 除赋值运算
         */
        template <typename U>
        typename std::enable_if<_DivOperationHelper<T, U>::value, const TDerived &>::type operator/=(U &&value) const
        {
            this->Set(this->Get() / std::forward<U>(value));
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 除赋值运算
         */
        template <typename D, typename U>
        typename std::enable_if<_DivOperationHelper<T, U>::value, TDerived &>::type operator/=(const PropertyBase<U, D> &prop)
        {
            this->Set(this->Get() / prop.Get());
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 除赋值运算
         */
        template <typename D, typename U>
        typename std::enable_if<_DivOperationHelper<T, U>::value, const TDerived &>::type operator/=(const PropertyBase<U, D> &prop) const
        {
            this->Set(this->Get() / prop.Get());
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 前置自增运算
         */
        template <typename U = T>
        typename std::enable_if<_AddOperationHelper<U, int>::value, TDerived &>::type operator++()
        {
            this->Set(this->Get() + 1);
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 前置自增运算
         */
        template <typename U = T>
        typename std::enable_if<_AddOperationHelper<U, int>::value, const TDerived &>::type operator++() const
        {
            this->Set(this->Get() + 1);
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 前置自减运算
         */
        template <typename U = T>
        typename std::enable_if<_SubOperationHelper<U, int>::value, TDerived &>::type operator--()
        {
            this->Set(this->Get() - 1);
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 前置自减运算
         */
        template <typename U = T>
        typename std::enable_if<_SubOperationHelper<U, int>::value, const TDerived &>::type operator--() const
        {
            this->Set(this->Get() - 1);
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 后置自增运算
         */
        template <typename U = T>
        typename std::enable_if<_AddOperationHelper<U, int>::value, T>::type operator++(int) const
        {
            T oldval = this->Get();
            this->Set(oldval + 1);
            return oldval;
        }

        /**
         * @brief 后置自减运算
         */
        template <typename U = T>
        typename std::enable_if<_SubOperationHelper<U, int>::value, T>::type operator--(int) const
        {
            T oldval = this->Get();
            this->Set(oldval - 1);
            return oldval;
        }

        /**
         * @brief 按位与赋值运算
         */
        template <typename U>
        typename std::enable_if<_BitAndOperationHelper<T, U>::value, TDerived &>::type operator&=(U &&value)
        {
            this->Set(this->Get() & std::forward<U>(value));
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 按位与赋值运算
         */
        template <typename U>
        typename std::enable_if<_BitAndOperationHelper<T, U>::value, const TDerived &>::type operator&=(U &&value) const
        {
            this->Set(this->Get() & std::forward<U>(value));
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 按位与赋值运算
         */
        template <typename D, typename U>
        typename std::enable_if<_BitAndOperationHelper<T, U>::value, TDerived &>::type operator&=(const PropertyBase<U, D> &prop)
        {
            this->Set(this->Get() & prop.Get());
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 按位与赋值运算
         */
        template <typename D, typename U>
        typename std::enable_if<_BitAndOperationHelper<T, U>::value, const TDerived &>::type operator&=(const PropertyBase<U, D> &prop) const
        {
            this->Set(this->Get() & prop.Get());
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 按位或赋值运算
         */
        template <typename U>
        typename std::enable_if<_BitOrOperationHelper<T, U>::value, TDerived &>::type operator|=(U &&value)
        {
            this->Set(this->Get() | std::forward<U>(value));
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 按位或赋值运算
         */
        template <typename U>
        typename std::enable_if<_BitOrOperationHelper<T, U>::value, const TDerived &>::type operator|=(U &&value) const
        {
            this->Set(this->Get() | std::forward<U>(value));
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 按位或赋值运算
         */
        template <typename D, typename U>
        typename std::enable_if<_BitOrOperationHelper<T, U>::value, TDerived &>::type operator|=(const PropertyBase<U, D> &prop)
        {
            this->Set(this->Get() | prop.Get());
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 按位或赋值运算
         */
        template <typename D, typename U>
        typename std::enable_if<_BitOrOperationHelper<T, U>::value, const TDerived &>::type operator|=(const PropertyBase<U, D> &prop) const
        {
            this->Set(this->Get() | prop.Get());
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 按位异或赋值运算
         */
        template <typename U>
        typename std::enable_if<_BitXorOperationHelper<T, U>::value, TDerived &>::type operator^=(U &&value)
        {
            this->Set(this->Get() ^ std::forward<U>(value));
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 按位异或赋值运算
         */
        template <typename U>
        typename std::enable_if<_BitXorOperationHelper<T, U>::value, const TDerived &>::type operator^=(U &&value) const
        {
            this->Set(this->Get() ^ std::forward<U>(value));
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 按位异或赋值运算
         */
        template <typename D, typename U>
        typename std::enable_if<_BitXorOperationHelper<T, U>::value, TDerived &>::type operator^=(const PropertyBase<U, D> &prop)
        {
            this->Set(this->Get() ^ prop.Get());
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 按位异或赋值运算
         */
        template <typename D, typename U>
        typename std::enable_if<_BitXorOperationHelper<T, U>::value, const TDerived &>::type operator^=(const PropertyBase<U, D> &prop) const
        {
            this->Set(this->Get() ^ prop.Get());
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 左移赋值运算
         */
        template <typename U>
        typename std::enable_if<_ShlOperationHelper<T, U>::value, TDerived &>::type operator<<=(U &&value)
        {
            this->Set(this->Get() << std::forward<U>(value));
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 左移赋值运算
         */
        template <typename U>
        typename std::enable_if<_ShlOperationHelper<T, U>::value, const TDerived &>::type operator<<=(U &&value) const
        {
            this->Set(this->Get() << std::forward<U>(value));
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 左移赋值运算
         */
        template <typename D, typename U>
        typename std::enable_if<_ShlOperationHelper<T, U>::value, TDerived &>::type operator<<=(const PropertyBase<U, D> &prop)
        {
            this->Set(this->Get() << prop.Get());
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 左移赋值运算
         */
        template <typename D, typename U>
        typename std::enable_if<_ShlOperationHelper<T, U>::value, const TDerived &>::type operator<<=(const PropertyBase<U, D> &prop) const
        {
            this->Set(this->Get() << prop.Get());
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 右移赋值运算
         */
        template <typename U>
        typename std::enable_if<_ShrOperationHelper<T, U>::value, TDerived &>::type operator>>=(U &&value)
        {
            this->Set(this->Get() >> std::forward<U>(value));
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 右移赋值运算
         */
        template <typename U>
        typename std::enable_if<_ShrOperationHelper<T, U>::value, const TDerived &>::type operator>>=(U &&value) const
        {
            this->Set(this->Get() >> std::forward<U>(value));
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 右移赋值运算
         */
        template <typename D, typename U>
        typename std::enable_if<_ShrOperationHelper<T, U>::value, TDerived &>::type operator>>=(const PropertyBase<U, D> &prop)
        {
            this->Set(this->Get() >> prop.Get());
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 右移赋值运算
         */
        template <typename D, typename U>
        typename std::enable_if<_ShrOperationHelper<T, U>::value, const TDerived &>::type operator>>=(const PropertyBase<U, D> &prop) const
        {
            this->Set(this->Get() >> prop.Get());
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 逻辑非运算
         */
        template <typename U = T>
        typename std::enable_if<_LogicNotOperationHelper<U>::value, typename _LogicNotOperationHelper<U>::type>::type operator!() const
        {
            return !this->Get();
        }

        /**
         * @brief 按位非运算
         */
        template <typename U = T>
        typename std::enable_if<_BitNotOperationHelper<U>::value, typename _BitNotOperationHelper<U>::type>::type operator~() const
        {
            return ~this->Get();
        }

        /**
         * @brief 解引用运算
         */
        template <typename U = T>
        typename std::enable_if<_DerefOperationHelper<U>::value, typename _DerefOperationHelper<U>::type>::type operator*() const
        {
            return *this->Get();
        }

        /**
         * @brief 地址运算
         */
        template <typename U = T>
        typename std::enable_if<_AddrOperationHelper<U>::value, typename _AddrOperationHelper<U>::type>::type operator&() const
        {
            return &this->Get();
        }

        /**
         * @brief 正号运算
         */
        template <typename U = T>
        typename std::enable_if<_UnaryPlusOperationHelper<U>::value, typename _UnaryPlusOperationHelper<U>::type>::type operator+() const
        {
            return +this->Get();
        }

        /**
         * @brief 负号运算
         */
        template <typename U = T>
        typename std::enable_if<_UnaryMinusOperationHelper<U>::value, typename _UnaryMinusOperationHelper<U>::type>::type operator-() const
        {
            return -this->Get();
        }

        /**
         * @brief 加法运算
         */
        template <typename U>
        typename std::enable_if<_AddOperationHelper<T, U>::value, typename _AddOperationHelper<T, U>::type>::type operator+(U &&value) const
        {
            return this->Get() + std::forward<U>(value);
        }

        /**
         * @brief 加法运算
         */
        template <typename D, typename U>
        typename std::enable_if<_AddOperationHelper<T, U>::value, typename _AddOperationHelper<T, U>::type>::type operator+(const PropertyBase<U, D> &prop) const
        {
            return this->Get() + prop.Get();
        }

        /**
         * @brief 减法运算
         */
        template <typename U>
        typename std::enable_if<_SubOperationHelper<T, U>::value, typename _SubOperationHelper<T, U>::type>::type operator-(U &&value) const
        {
            return this->Get() - std::forward<U>(value);
        }

        /**
         * @brief 减法运算
         */
        template <typename D, typename U>
        typename std::enable_if<_SubOperationHelper<T, U>::value, typename _SubOperationHelper<T, U>::type>::type operator-(const PropertyBase<U, D> &prop) const
        {
            return this->Get() - prop.Get();
        }

        /**
         * @brief 乘法运算
         */
        template <typename U>
        typename std::enable_if<_MulOperationHelper<T, U>::value, typename _MulOperationHelper<T, U>::type>::type operator*(U &&value) const
        {
            return this->Get() * std::forward<U>(value);
        }

        /**
         * @brief 乘法运算
         */
        template <typename D, typename U>
        typename std::enable_if<_MulOperationHelper<T, U>::value, typename _MulOperationHelper<T, U>::type>::type operator*(const PropertyBase<U, D> &prop) const
        {
            return this->Get() * prop.Get();
        }

        /**
         * @brief 除法运算
         */
        template <typename U>
        typename std::enable_if<_DivOperationHelper<T, U>::value, typename _DivOperationHelper<T, U>::type>::type operator/(U &&value) const
        {
            return this->Get() / std::forward<U>(value);
        }

        /**
         * @brief 除法运算
         */
        template <typename D, typename U>
        typename std::enable_if<_DivOperationHelper<T, U>::value, typename _DivOperationHelper<T, U>::type>::type operator/(const PropertyBase<U, D> &prop) const
        {
            return this->Get() / prop.Get();
        }

        /**
         * @brief 取模运算
         */
        template <typename U>
        typename std::enable_if<_ModOperationHelper<T, U>::value, typename _ModOperationHelper<T, U>::type>::type operator%(U &&value) const
        {
            return this->Get() % std::forward<U>(value);
        }

        /**
         * @brief 取模运算
         */
        template <typename D, typename U>
        typename std::enable_if<_ModOperationHelper<T, U>::value, typename _ModOperationHelper<T, U>::type>::type operator%(const PropertyBase<U, D> &prop) const
        {
            return this->Get() % prop.Get();
        }

        /**
         * @brief 等于运算
         */
        template <typename U>
        typename std::enable_if<_EqOperationHelper<T, U>::value, typename _EqOperationHelper<T, U>::type>::type operator==(U &&value) const
        {
            return this->Get() == std::forward<U>(value);
        }

        /**
         * @brief 等于运算
         */
        template <typename D, typename U>
        typename std::enable_if<_EqOperationHelper<T, U>::value, typename _EqOperationHelper<T, U>::type>::type operator==(const PropertyBase<U, D> &prop) const
        {
            return this->Get() == prop.Get();
        }

        /**
         * @brief 不等于运算
         * @note  避免与c++20自动生成的!=冲突，通过==取反实现
         */
        template <typename U>
        typename std::enable_if<_EqOperationHelper<T, U>::value, typename _EqOperationHelper<T, U>::type>::type operator!=(U &&value) const
        {
            return !(*this == std::forward<U>(value));
        }

        /**
         * @brief 不等于运算
         * @note  避免与c++20自动生成的!=冲突，通过==取反实现
         */
        template <typename D, typename U>
        typename std::enable_if<_EqOperationHelper<T, U>::value, typename _EqOperationHelper<T, U>::type>::type operator!=(const PropertyBase<U, D> &prop) const
        {
            return !(*this == prop);
        }

        /**
         * @brief 小于运算
         */
        template <typename U>
        typename std::enable_if<_LtOperationHelper<T, U>::value, typename _LtOperationHelper<T, U>::type>::type operator<(U &&value) const
        {
            return this->Get() < std::forward<U>(value);
        }

        /**
         * @brief 小于运算
         */
        template <typename D, typename U>
        typename std::enable_if<_LtOperationHelper<T, U>::value, typename _LtOperationHelper<T, U>::type>::type operator<(const PropertyBase<U, D> &prop) const
        {
            return this->Get() < prop.Get();
        }

        /**
         * @brief 小于等于运算
         */
        template <typename U>
        typename std::enable_if<_LeOperationHelper<T, U>::value, typename _LeOperationHelper<T, U>::type>::type operator<=(U &&value) const
        {
            return this->Get() <= std::forward<U>(value);
        }

        /**
         * @brief 小于等于运算
         */
        template <typename D, typename U>
        typename std::enable_if<_LeOperationHelper<T, U>::value, typename _LeOperationHelper<T, U>::type>::type operator<=(const PropertyBase<U, D> &prop) const
        {
            return this->Get() <= prop.Get();
        }

        /**
         * @brief 大于运算
         */
        template <typename U>
        typename std::enable_if<_GtOperationHelper<T, U>::value, typename _GtOperationHelper<T, U>::type>::type operator>(U &&value) const
        {
            return this->Get() > std::forward<U>(value);
        }

        /**
         * @brief 大于运算
         */
        template <typename D, typename U>
        typename std::enable_if<_GtOperationHelper<T, U>::value, typename _GtOperationHelper<T, U>::type>::type operator>(const PropertyBase<U, D> &prop) const
        {
            return this->Get() > prop.Get();
        }

        /**
         * @brief 大于等于运算
         */
        template <typename U>
        typename std::enable_if<_GeOperationHelper<T, U>::value, typename _GeOperationHelper<T, U>::type>::type operator>=(U &&value) const
        {
            return this->Get() >= std::forward<U>(value);
        }

        /**
         * @brief 大于等于运算
         */
        template <typename D, typename U>
        typename std::enable_if<_GeOperationHelper<T, U>::value, typename _GeOperationHelper<T, U>::type>::type operator>=(const PropertyBase<U, D> &prop) const
        {
            return this->Get() >= prop.Get();
        }

        /**
         * @brief 按位与运算
         */
        template <typename U>
        typename std::enable_if<_BitAndOperationHelper<T, U>::value, typename _BitAndOperationHelper<T, U>::type>::type operator&(U &&value) const
        {
            return this->Get() & std::forward<U>(value);
        }

        /**
         * @brief 按位与运算
         */
        template <typename D, typename U>
        typename std::enable_if<_BitAndOperationHelper<T, U>::value, typename _BitAndOperationHelper<T, U>::type>::type operator&(const PropertyBase<U, D> &prop) const
        {
            return this->Get() & prop.Get();
        }

        /**
         * @brief 按位或运算
         */
        template <typename U>
        typename std::enable_if<_BitOrOperationHelper<T, U>::value, typename _BitOrOperationHelper<T, U>::type>::type operator|(U &&value) const
        {
            return this->Get() | std::forward<U>(value);
        }

        /**
         * @brief 按位或运算
         */
        template <typename D, typename U>
        typename std::enable_if<_BitOrOperationHelper<T, U>::value, typename _BitOrOperationHelper<T, U>::type>::type operator|(const PropertyBase<U, D> &prop) const
        {
            return this->Get() | prop.Get();
        }

        /**
         * @brief 按位异或运算
         */
        template <typename U>
        typename std::enable_if<_BitXorOperationHelper<T, U>::value, typename _BitXorOperationHelper<T, U>::type>::type operator^(U &&value) const
        {
            return this->Get() ^ std::forward<U>(value);
        }

        /**
         * @brief 按位异或运算
         */
        template <typename D, typename U>
        typename std::enable_if<_BitXorOperationHelper<T, U>::value, typename _BitXorOperationHelper<T, U>::type>::type operator^(const PropertyBase<U, D> &prop) const
        {
            return this->Get() ^ prop.Get();
        }

        /**
         * @brief 左移运算
         */
        template <typename U>
        typename std::enable_if<_ShlOperationHelper<T, U>::value, typename _ShlOperationHelper<T, U>::type>::type operator<<(U &&value) const
        {
            return this->Get() << std::forward<U>(value);
        }

        /**
         * @brief 左移运算
         */
        template <typename D, typename U>
        typename std::enable_if<_ShlOperationHelper<T, U>::value, typename _ShlOperationHelper<T, U>::type>::type operator<<(const PropertyBase<U, D> &prop) const
        {
            return this->Get() << prop.Get();
        }

        /**
         * @brief 右移运算
         */
        template <typename U>
        typename std::enable_if<_ShrOperationHelper<T, U>::value, typename _ShrOperationHelper<T, U>::type>::type operator>>(U &&value) const
        {
            return this->Get() >> std::forward<U>(value);
        }

        /**
         * @brief 右移运算
         */
        template <typename D, typename U>
        typename std::enable_if<_ShrOperationHelper<T, U>::value, typename _ShrOperationHelper<T, U>::type>::type operator>>(const PropertyBase<U, D> &prop) const
        {
            return this->Get() >> prop.Get();
        }

        /**
         * @brief 逻辑与运算
         */
        template <typename U>
        typename std::enable_if<_LogicAndOperationHelper<T, U>::value, typename _LogicAndOperationHelper<T, U>::type>::type operator&&(U &&value) const
        {
            return this->Get() && std::forward<U>(value);
        }

        /**
         * @brief 逻辑与运算
         */
        template <typename D, typename U>
        typename std::enable_if<_LogicAndOperationHelper<T, U>::value, typename _LogicAndOperationHelper<T, U>::type>::type operator&&(const PropertyBase<U, D> &prop) const
        {
            return this->Get() && prop.Get();
        }

        /**
         * @brief 逻辑或运算
         */
        template <typename U>
        typename std::enable_if<_LogicOrOperationHelper<T, U>::value, typename _LogicOrOperationHelper<T, U>::type>::type operator||(U &&value) const
        {
            return this->Get() || std::forward<U>(value);
        }

        /**
         * @brief 逻辑或运算
         */
        template <typename D, typename U>
        typename std::enable_if<_LogicOrOperationHelper<T, U>::value, typename _LogicOrOperationHelper<T, U>::type>::type operator||(const PropertyBase<U, D> &prop) const
        {
            return this->Get() || prop.Get();
        }

        /**
         * @brief 下标运算
         */
        template <typename U>
        typename std::enable_if<
            _BracketOperationHelper<T, U>::value &&
                !std::is_reference<typename _BracketOperationHelper<T, U>::type>::value,
            typename _BracketOperationHelper<T, U>::type>::type
        operator[](U &&value) const
        {
            return this->Get()[std::forward<U>(value)];
        }

        /**
         * @brief 下标运算
         */
        template <typename D, typename U>
        typename std::enable_if<
            _BracketOperationHelper<T, U>::value &&
                !std::is_reference<typename _BracketOperationHelper<T, U>::type>::value,
            typename _BracketOperationHelper<T, U>::type>::type
        operator[](const PropertyBase<U, D> &prop) const
        {
            return this->Get()[prop.Get()];
        }

        /**
         * @brief 指针下标运算
         */
        template <typename U>
        typename std::enable_if<
            _BracketOperationHelper<T, U>::value && std::is_pointer<T>::value,
            typename _BracketOperationHelper<T, U>::type>::type
        operator[](U &&value) const
        {
            return this->Get()[std::forward<U>(value)];
        }

        /**
         * @brief 指针下标运算
         */
        template <typename D, typename U>
        typename std::enable_if<
            _BracketOperationHelper<T, U>::value && std::is_pointer<T>::value,
            typename _BracketOperationHelper<T, U>::type>::type
        operator[](const PropertyBase<U, D> &prop) const
        {
            return this->Get()[prop.Get()];
        }

    protected:
        /**
         * @brief 静态属性偏移量标记
         */
        static constexpr std::ptrdiff_t _STATICOFFSET = (std::numeric_limits<std::ptrdiff_t>::max)();

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
        static MemberPropertyInitializer<TOwner, T> Init(TOwner *owner)
        {
            return MemberPropertyInitializer<TOwner, T>(owner);
        }

        /**
         * @brief 获取静态属性初始化器
         */
        static StaticPropertyInitializer<T> Init()
        {
            return StaticPropertyInitializer<T>();
        }
    };

    /*================================================================================*/

    /**
     * @brief 加法运算
     */
    template <typename D, typename T, typename U>
    typename std::enable_if<!_IsProperty<T>::value && _AddOperationHelper<T, U>::value, typename _AddOperationHelper<T, U>::type>::type
    operator+(T &&left, const PropertyBase<U, D> &right)
    {
        return std::forward<T>(left) + right.Get();
    }

    /**
     * @brief 减法运算
     */
    template <typename D, typename T, typename U>
    typename std::enable_if<!_IsProperty<T>::value && _SubOperationHelper<T, U>::value, typename _SubOperationHelper<T, U>::type>::type
    operator-(T &&left, const PropertyBase<U, D> &right)
    {
        return std::forward<T>(left) - right.Get();
    }

    /**
     * @brief 乘法运算
     */
    template <typename D, typename T, typename U>
    typename std::enable_if<!_IsProperty<T>::value && _MulOperationHelper<T, U>::value, typename _MulOperationHelper<T, U>::type>::type
    operator*(T &&left, const PropertyBase<U, D> &right)
    {
        return std::forward<T>(left) * right.Get();
    }

    /**
     * @brief 除法运算
     */
    template <typename D, typename T, typename U>
    typename std::enable_if<!_IsProperty<T>::value && _DivOperationHelper<T, U>::value, typename _DivOperationHelper<T, U>::type>::type
    operator/(T &&left, const PropertyBase<U, D> &right)
    {
        return std::forward<T>(left) / right.Get();
    }

    /**
     * @brief 取模运算
     */
    template <typename D, typename T, typename U>
    typename std::enable_if<!_IsProperty<T>::value && _ModOperationHelper<T, U>::value, typename _ModOperationHelper<T, U>::type>::type
    operator%(T &&left, const PropertyBase<U, D> &right)
    {
        return std::forward<T>(left) % right.Get();
    }

    /**
     * @brief 等于运算
     */
    template <typename D, typename T, typename U>
    typename std::enable_if<!_IsProperty<T>::value && _EqOperationHelper<T, U>::value, typename _EqOperationHelper<T, U>::type>::type
    operator==(T &&left, const PropertyBase<U, D> &right)
    {
        return std::forward<T>(left) == right.Get();
    }

    /**
     * @brief 不等于运算
     * @note  避免与c++20自动生成的!=冲突，通过==取反实现
     */
    template <typename D, typename T, typename U>
    typename std::enable_if<!_IsProperty<T>::value && _EqOperationHelper<T, U>::value, typename _EqOperationHelper<T, U>::type>::type
    operator!=(T &&left, const PropertyBase<U, D> &right)
    {
        return !(std::forward<T>(left) == right);
    }

    /**
     * @brief 小于运算
     */
    template <typename D, typename T, typename U>
    typename std::enable_if<!_IsProperty<T>::value && _LtOperationHelper<T, U>::value, typename _LtOperationHelper<T, U>::type>::type
    operator<(T &&left, const PropertyBase<U, D> &right)
    {
        return std::forward<T>(left) < right.Get();
    }

    /**
     * @brief 小于等于运算
     */
    template <typename D, typename T, typename U>
    typename std::enable_if<!_IsProperty<T>::value && _LeOperationHelper<T, U>::value, typename _LeOperationHelper<T, U>::type>::type
    operator<=(T &&left, const PropertyBase<U, D> &right)
    {
        return std::forward<T>(left) <= right.Get();
    }

    /**
     * @brief 大于运算
     */
    template <typename D, typename T, typename U>
    typename std::enable_if<!_IsProperty<T>::value && _GtOperationHelper<T, U>::value, typename _GtOperationHelper<T, U>::type>::type
    operator>(T &&left, const PropertyBase<U, D> &right)
    {
        return std::forward<T>(left) > right.Get();
    }

    /**
     * @brief 大于等于运算
     */
    template <typename D, typename T, typename U>
    typename std::enable_if<!_IsProperty<T>::value && _GeOperationHelper<T, U>::value, typename _GeOperationHelper<T, U>::type>::type
    operator>=(T &&left, const PropertyBase<U, D> &right)
    {
        return std::forward<T>(left) >= right.Get();
    }

    /**
     * @brief 按位与运算
     */
    template <typename D, typename T, typename U>
    typename std::enable_if<!_IsProperty<T>::value && _BitAndOperationHelper<T, U>::value, typename _BitAndOperationHelper<T, U>::type>::type
    operator&(T &&left, const PropertyBase<U, D> &right)
    {
        return std::forward<T>(left) & right.Get();
    }

    /**
     * @brief 按位或运算
     */
    template <typename D, typename T, typename U>
    typename std::enable_if<!_IsProperty<T>::value && _BitOrOperationHelper<T, U>::value, typename _BitOrOperationHelper<T, U>::type>::type
    operator|(T &&left, const PropertyBase<U, D> &right)
    {
        return std::forward<T>(left) | right.Get();
    }

    /**
     * @brief 按位异或运算
     */
    template <typename D, typename T, typename U>
    typename std::enable_if<!_IsProperty<T>::value && _BitXorOperationHelper<T, U>::value, typename _BitXorOperationHelper<T, U>::type>::type
    operator^(T &&left, const PropertyBase<U, D> &right)
    {
        return std::forward<T>(left) ^ right.Get();
    }

    /**
     * @brief 左移运算
     */
    template <typename D, typename T, typename U>
    typename std::enable_if<!_IsProperty<T>::value && _ShlOperationHelper<T, U>::value, typename _ShlOperationHelper<T, U>::type>::type
    operator<<(T &&left, const PropertyBase<U, D> &right)
    {
        return std::forward<T>(left) << right.Get();
    }

    /**
     * @brief 右移运算
     */
    template <typename D, typename T, typename U>
    typename std::enable_if<!_IsProperty<T>::value && _ShrOperationHelper<T, U>::value, typename _ShrOperationHelper<T, U>::type>::type
    operator>>(T &&left, const PropertyBase<U, D> &right)
    {
        return std::forward<T>(left) >> right.Get();
    }

    /**
     * @brief 逻辑与运算
     */
    template <typename D, typename T, typename U>
    typename std::enable_if<!_IsProperty<T>::value && _LogicAndOperationHelper<T, U>::value, typename _LogicAndOperationHelper<T, U>::type>::type
    operator&&(T &&left, const PropertyBase<U, D> &right)
    {
        return std::forward<T>(left) && right.Get();
    }

    /**
     * @brief 逻辑或运算
     */
    template <typename D, typename T, typename U>
    typename std::enable_if<!_IsProperty<T>::value && _LogicOrOperationHelper<T, U>::value, typename _LogicOrOperationHelper<T, U>::type>::type
    operator||(T &&left, const PropertyBase<U, D> &right)
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
        using TGetter       = T (*)(void *);
        using TSetter       = void (*)(void *, TSetterParam);
        using TStaticGetter = T (*)();
        using TStaticSetter = void (*)(TSetterParam);

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
        explicit Property(const MemberPropertyInitializer<TOwner, T> &initializer)
        {
            assert(initializer._owner != nullptr);
            assert(initializer._getter != nullptr);
            assert(initializer._setter != nullptr);

            this->SetOwner(initializer._owner);
            this->_getter = reinterpret_cast<void *>(initializer._getter);
            this->_setter = reinterpret_cast<void *>(initializer._setter);
        }

        /**
         * @brief 构造静态属性
         */
        explicit Property(const StaticPropertyInitializer<T> &initializer)
        {
            assert(initializer._getter != nullptr);
            assert(initializer._setter != nullptr);

            this->SetOwner(nullptr);
            this->_getter = reinterpret_cast<void *>(initializer._getter);
            this->_setter = reinterpret_cast<void *>(initializer._setter);
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
        explicit ReadOnlyProperty(const MemberPropertyInitializer<TOwner, T> &initializer)
        {
            assert(initializer._owner != nullptr);
            assert(initializer._getter != nullptr);

            this->SetOwner(initializer._owner);
            this->_getter = reinterpret_cast<void *>(initializer._getter);
        }

        /**
         * @brief 构造静态属性
         */
        explicit ReadOnlyProperty(const StaticPropertyInitializer<T> &initializer)
        {
            assert(initializer._getter != nullptr);

            this->SetOwner(nullptr);
            this->_getter = reinterpret_cast<void *>(initializer._getter);
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
        using TSetter       = void (*)(void *, TSetterParam);
        using TStaticSetter = void (*)(TSetterParam);

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
        explicit WriteOnlyProperty(const MemberPropertyInitializer<TOwner, T> &initializer)
        {
            assert(initializer._owner != nullptr);
            assert(initializer._setter != nullptr);

            this->SetOwner(initializer._owner);
            this->_setter = reinterpret_cast<void *>(initializer._setter);
        }

        /**
         * @brief 构造静态属性
         */
        explicit WriteOnlyProperty(const StaticPropertyInitializer<T> &initializer)
        {
            assert(initializer._setter != nullptr);

            this->SetOwner(nullptr);
            this->_setter = reinterpret_cast<void *>(initializer._setter);
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

#include <string>

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
