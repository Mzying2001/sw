#pragma once

#include "Delegate.h"
#include <type_traits>

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
    // 向前声明
    template <typename T, typename TDerived>
    class PropertyBase;

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
     * @brief 修复_HasArrowOperator模板在VS2015环境下的兼容性问题
     * @note  VS2015似乎对decltype(void(...))支持不完整，用其判断operator->会导致编译错误，
     * @note  这个模板通过重载决议来判断类型是否有operator->，以兼容VS2015。
     */
    template <typename T>
    struct _HasArrowOperatorVs2015Fix {
    private:
        template <typename U>
        static auto test(int) -> decltype(std::declval<U>().operator->(), std::true_type{});

        template <typename>
        static auto test(...) -> std::false_type;

    public:
        static constexpr bool value = decltype(test<T>(0))::value;
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
        // T, decltype(void(std::declval<T>().operator->()))> : std::true_type {
        T, typename std::enable_if<_HasArrowOperatorVs2015Fix<T>::value>::type> : std::true_type {
        using type = decltype(std::declval<T>().operator->());
    };

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
            return value;
        }

        /**
         * @brief 非指针类型，且无operator->，返回值的地址
         */
        template <typename U = T>
        typename std::enable_if<!std::is_pointer<U>::value && !_HasArrowOperator<U>::value, U *>::type operator->()
        {
            return &value;
        }

        /**
         * @brief 非指针类型，且有operator->，转发operator->
         */
        template <typename U = T>
        typename std::enable_if<!std::is_pointer<U>::value && _HasArrowOperator<U>::value, typename _HasArrowOperator<U>::type>::type operator->()
        {
            return value.operator->();
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

        // 使用默认构造函数
        PropertyBase() = default;

        // 删除移动构造
        PropertyBase(PropertyBase &&) = delete;

        // 删除拷贝构造
        PropertyBase(const PropertyBase &) = delete;

        // 删除移动赋值
        PropertyBase &operator=(PropertyBase &&) = delete;

        // /**
        //  * @brief 获取属性值，由子类实现
        //  */
        // T GetterImpl() const;

        // /**
        //  * @brief 设置属性值，由子类实现
        //  */
        // void SetterImpl(const T &value) const;

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
        void Set(const T &value) const
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
        TDerived &operator=(const T &value)
        {
            this->Set(value);
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 设置属性值
         */
        const TDerived &operator=(const T &value) const
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
        template <typename U = T>
        typename std::enable_if<_AddOperationHelper<T, U>::value, TDerived &>::type operator+=(const U &value)
        {
            this->Set(this->Get() + value);
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 加赋值运算
         */
        template <typename U = T>
        typename std::enable_if<_AddOperationHelper<T, U>::value, const TDerived &>::type operator+=(const U &value) const
        {
            this->Set(this->Get() + value);
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 加赋值运算
         */
        template <typename D, typename U = T>
        typename std::enable_if<_AddOperationHelper<T, U>::value, TDerived &>::type operator+=(const PropertyBase<U, D> &prop)
        {
            this->Set(this->Get() + prop.Get());
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 加赋值运算
         */
        template <typename D, typename U = T>
        typename std::enable_if<_AddOperationHelper<T, U>::value, const TDerived &>::type operator+=(const PropertyBase<U, D> &prop) const
        {
            this->Set(this->Get() + prop.Get());
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 减赋值运算
         */
        template <typename U = T>
        typename std::enable_if<_SubOperationHelper<T, U>::value, TDerived &>::type operator-=(const U &value)
        {
            this->Set(this->Get() - value);
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 减赋值运算
         */
        template <typename U = T>
        typename std::enable_if<_SubOperationHelper<T, U>::value, const TDerived &>::type operator-=(const U &value) const
        {
            this->Set(this->Get() - value);
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 减赋值运算
         */
        template <typename D, typename U = T>
        typename std::enable_if<_SubOperationHelper<T, U>::value, TDerived &>::type operator-=(const PropertyBase<U, D> &prop)
        {
            this->Set(this->Get() - prop.Get());
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 减赋值运算
         */
        template <typename D, typename U = T>
        typename std::enable_if<_SubOperationHelper<T, U>::value, const TDerived &>::type operator-=(const PropertyBase<U, D> &prop) const
        {
            this->Set(this->Get() - prop.Get());
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 乘赋值运算
         */
        template <typename U = T>
        typename std::enable_if<_MulOperationHelper<T, U>::value, TDerived &>::type operator*=(const U &value)
        {
            this->Set(this->Get() * value);
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 乘赋值运算
         */
        template <typename U = T>
        typename std::enable_if<_MulOperationHelper<T, U>::value, const TDerived &>::type operator*=(const U &value) const
        {
            this->Set(this->Get() * value);
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 乘赋值运算
         */
        template <typename D, typename U = T>
        typename std::enable_if<_MulOperationHelper<T, U>::value, TDerived &>::type operator*=(const PropertyBase<U, D> &prop)
        {
            this->Set(this->Get() * prop.Get());
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 乘赋值运算
         */
        template <typename D, typename U = T>
        typename std::enable_if<_MulOperationHelper<T, U>::value, const TDerived &>::type operator*=(const PropertyBase<U, D> &prop) const
        {
            this->Set(this->Get() * prop.Get());
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 除赋值运算
         */
        template <typename U = T>
        typename std::enable_if<_DivOperationHelper<T, U>::value, TDerived &>::type operator/=(const U &value)
        {
            this->Set(this->Get() / value);
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 除赋值运算
         */
        template <typename U = T>
        typename std::enable_if<_DivOperationHelper<T, U>::value, const TDerived &>::type operator/=(const U &value) const
        {
            this->Set(this->Get() / value);
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 除赋值运算
         */
        template <typename D, typename U = T>
        typename std::enable_if<_DivOperationHelper<T, U>::value, TDerived &>::type operator/=(const PropertyBase<U, D> &prop)
        {
            this->Set(this->Get() / prop.Get());
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 除赋值运算
         */
        template <typename D, typename U = T>
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
        template <typename U = T>
        typename std::enable_if<_BitAndOperationHelper<T, U>::value, TDerived &>::type operator&=(const U &value)
        {
            this->Set(this->Get() & value);
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 按位与赋值运算
         */
        template <typename U = T>
        typename std::enable_if<_BitAndOperationHelper<T, U>::value, const TDerived &>::type operator&=(const U &value) const
        {
            this->Set(this->Get() & value);
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 按位与赋值运算
         */
        template <typename D, typename U = T>
        typename std::enable_if<_BitAndOperationHelper<T, U>::value, TDerived &>::type operator&=(const PropertyBase<U, D> &prop)
        {
            this->Set(this->Get() & prop.Get());
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 按位与赋值运算
         */
        template <typename D, typename U = T>
        typename std::enable_if<_BitAndOperationHelper<T, U>::value, const TDerived &>::type operator&=(const PropertyBase<U, D> &prop) const
        {
            this->Set(this->Get() & prop.Get());
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 按位或赋值运算
         */
        template <typename U = T>
        typename std::enable_if<_BitOrOperationHelper<T, U>::value, TDerived &>::type operator|=(const U &value)
        {
            this->Set(this->Get() | value);
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 按位或赋值运算
         */
        template <typename U = T>
        typename std::enable_if<_BitOrOperationHelper<T, U>::value, const TDerived &>::type operator|=(const U &value) const
        {
            this->Set(this->Get() | value);
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 按位或赋值运算
         */
        template <typename D, typename U = T>
        typename std::enable_if<_BitOrOperationHelper<T, U>::value, TDerived &>::type operator|=(const PropertyBase<U, D> &prop)
        {
            this->Set(this->Get() | prop.Get());
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 按位或赋值运算
         */
        template <typename D, typename U = T>
        typename std::enable_if<_BitOrOperationHelper<T, U>::value, const TDerived &>::type operator|=(const PropertyBase<U, D> &prop) const
        {
            this->Set(this->Get() | prop.Get());
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 按位异或赋值运算
         */
        template <typename U = T>
        typename std::enable_if<_BitXorOperationHelper<T, U>::value, TDerived &>::type operator^=(const U &value)
        {
            this->Set(this->Get() ^ value);
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 按位异或赋值运算
         */
        template <typename U = T>
        typename std::enable_if<_BitXorOperationHelper<T, U>::value, const TDerived &>::type operator^=(const U &value) const
        {
            this->Set(this->Get() ^ value);
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 按位异或赋值运算
         */
        template <typename D, typename U = T>
        typename std::enable_if<_BitXorOperationHelper<T, U>::value, TDerived &>::type operator^=(const PropertyBase<U, D> &prop)
        {
            this->Set(this->Get() ^ prop.Get());
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 按位异或赋值运算
         */
        template <typename D, typename U = T>
        typename std::enable_if<_BitXorOperationHelper<T, U>::value, const TDerived &>::type operator^=(const PropertyBase<U, D> &prop) const
        {
            this->Set(this->Get() ^ prop.Get());
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 左移赋值运算
         */
        template <typename U = T>
        typename std::enable_if<_ShlOperationHelper<T, U>::value, TDerived &>::type operator<<=(const U &value)
        {
            this->Set(this->Get() << value);
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 左移赋值运算
         */
        template <typename U = T>
        typename std::enable_if<_ShlOperationHelper<T, U>::value, const TDerived &>::type operator<<=(const U &value) const
        {
            this->Set(this->Get() << value);
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 左移赋值运算
         */
        template <typename D, typename U = T>
        typename std::enable_if<_ShlOperationHelper<T, U>::value, TDerived &>::type operator<<=(const PropertyBase<U, D> &prop)
        {
            this->Set(this->Get() << prop.Get());
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 左移赋值运算
         */
        template <typename D, typename U = T>
        typename std::enable_if<_ShlOperationHelper<T, U>::value, const TDerived &>::type operator<<=(const PropertyBase<U, D> &prop) const
        {
            this->Set(this->Get() << prop.Get());
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 右移赋值运算
         */
        template <typename U = T>
        typename std::enable_if<_ShrOperationHelper<T, U>::value, TDerived &>::type operator>>=(const U &value)
        {
            this->Set(this->Get() >> value);
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 右移赋值运算
         */
        template <typename U = T>
        typename std::enable_if<_ShrOperationHelper<T, U>::value, const TDerived &>::type operator>>=(const U &value) const
        {
            this->Set(this->Get() >> value);
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 右移赋值运算
         */
        template <typename D, typename U = T>
        typename std::enable_if<_ShrOperationHelper<T, U>::value, TDerived &>::type operator>>=(const PropertyBase<U, D> &prop)
        {
            this->Set(this->Get() >> prop.Get());
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 右移赋值运算
         */
        template <typename D, typename U = T>
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
         * @brief 一元加运算
         */
        template <typename U = T>
        typename std::enable_if<_UnaryPlusOperationHelper<U>::value, typename _UnaryPlusOperationHelper<U>::type>::type operator+() const
        {
            return +this->Get();
        }

        /**
         * @brief 一元减运算
         */
        template <typename U = T>
        typename std::enable_if<_UnaryMinusOperationHelper<U>::value, typename _UnaryMinusOperationHelper<U>::type>::type operator-() const
        {
            return -this->Get();
        }

        /**
         * @brief 加法运算
         */
        template <typename U = T>
        typename std::enable_if<_AddOperationHelper<T, U>::value, typename _AddOperationHelper<T, U>::type>::type operator+(const U &value) const
        {
            return this->Get() + value;
        }

        /**
         * @brief 加法运算
         */
        template <typename D, typename U = T>
        typename std::enable_if<_AddOperationHelper<T, U>::value, typename _AddOperationHelper<T, U>::type>::type operator+(const PropertyBase<U, D> &prop) const
        {
            return this->Get() + prop.Get();
        }

        /**
         * @brief 减法运算
         */
        template <typename U = T>
        typename std::enable_if<_SubOperationHelper<T, U>::value, typename _SubOperationHelper<T, U>::type>::type operator-(const U &value) const
        {
            return this->Get() - value;
        }

        /**
         * @brief 减法运算
         */
        template <typename D, typename U = T>
        typename std::enable_if<_SubOperationHelper<T, U>::value, typename _SubOperationHelper<T, U>::type>::type operator-(const PropertyBase<U, D> &prop) const
        {
            return this->Get() - prop.Get();
        }

        /**
         * @brief 乘法运算
         */
        template <typename U = T>
        typename std::enable_if<_MulOperationHelper<T, U>::value, typename _MulOperationHelper<T, U>::type>::type operator*(const U &value) const
        {
            return this->Get() * value;
        }

        /**
         * @brief 乘法运算
         */
        template <typename D, typename U = T>
        typename std::enable_if<_MulOperationHelper<T, U>::value, typename _MulOperationHelper<T, U>::type>::type operator*(const PropertyBase<U, D> &prop) const
        {
            return this->Get() * prop.Get();
        }

        /**
         * @brief 除法运算
         */
        template <typename U = T>
        typename std::enable_if<_DivOperationHelper<T, U>::value, typename _DivOperationHelper<T, U>::type>::type operator/(const U &value) const
        {
            return this->Get() / value;
        }

        /**
         * @brief 除法运算
         */
        template <typename D, typename U = T>
        typename std::enable_if<_DivOperationHelper<T, U>::value, typename _DivOperationHelper<T, U>::type>::type operator/(const PropertyBase<U, D> &prop) const
        {
            return this->Get() / prop.Get();
        }

        /**
         * @brief 取模运算
         */
        template <typename U = T>
        typename std::enable_if<_ModOperationHelper<T, U>::value, typename _ModOperationHelper<T, U>::type>::type operator%(const U &value) const
        {
            return this->Get() % value;
        }

        /**
         * @brief 取模运算
         */
        template <typename D, typename U = T>
        typename std::enable_if<_ModOperationHelper<T, U>::value, typename _ModOperationHelper<T, U>::type>::type operator%(const PropertyBase<U, D> &prop) const
        {
            return this->Get() % prop.Get();
        }

        /**
         * @brief 等于运算
         */
        template <typename U = T>
        typename std::enable_if<_EqOperationHelper<T, U>::value, typename _EqOperationHelper<T, U>::type>::type operator==(const U &value) const
        {
            return this->Get() == value;
        }

        /**
         * @brief 等于运算
         */
        template <typename D, typename U = T>
        typename std::enable_if<_EqOperationHelper<T, U>::value, typename _EqOperationHelper<T, U>::type>::type operator==(const PropertyBase<U, D> &prop) const
        {
            return this->Get() == prop.Get();
        }

        ///**
        // * @brief 不等于运算
        // */
        // template <typename U = T>
        // typename std::enable_if<_NeOperationHelper<T, U>::value, typename _NeOperationHelper<T, U>::type>::type operator!=(const U &value) const
        //{
        //    return this->Get() != value;
        //}

        ///**
        // * @brief 不等于运算
        // */
        // template <typename D, typename U = T>
        // typename std::enable_if<_NeOperationHelper<T, U>::value, typename _NeOperationHelper<T, U>::type>::type operator!=(const PropertyBase<U, D> &prop) const
        //{
        //    return this->Get() != prop.Get();
        //}

        /**
         * @brief 不等于运算
         * @note  避免与c++20自动生成的!=冲突，改为通过==取反实现
         */
        template <typename U = T>
        typename std::enable_if<_EqOperationHelper<T, U>::value, typename _EqOperationHelper<T, U>::type>::type operator!=(const U &value) const
        {
            return !(*this == value);
        }

        /**
         * @brief 不等于运算
         * @note  避免与c++20自动生成的!=冲突，改为通过==取反实现
         */
        template <typename D, typename U = T>
        typename std::enable_if<_EqOperationHelper<T, U>::value, typename _EqOperationHelper<T, U>::type>::type operator!=(const PropertyBase<U, D> &prop) const
        {
            return !(*this == prop);
        }

        /**
         * @brief 小于运算
         */
        template <typename U = T>
        typename std::enable_if<_LtOperationHelper<T, U>::value, typename _LtOperationHelper<T, U>::type>::type operator<(const U &value) const
        {
            return this->Get() < value;
        }

        /**
         * @brief 小于运算
         */
        template <typename D, typename U = T>
        typename std::enable_if<_LtOperationHelper<T, U>::value, typename _LtOperationHelper<T, U>::type>::type operator<(const PropertyBase<U, D> &prop) const
        {
            return this->Get() < prop.Get();
        }

        /**
         * @brief 小于等于运算
         */
        template <typename U = T>
        typename std::enable_if<_LeOperationHelper<T, U>::value, typename _LeOperationHelper<T, U>::type>::type operator<=(const U &value) const
        {
            return this->Get() <= value;
        }

        /**
         * @brief 小于等于运算
         */
        template <typename D, typename U = T>
        typename std::enable_if<_LeOperationHelper<T, U>::value, typename _LeOperationHelper<T, U>::type>::type operator<=(const PropertyBase<U, D> &prop) const
        {
            return this->Get() <= prop.Get();
        }

        /**
         * @brief 大于运算
         */
        template <typename U = T>
        typename std::enable_if<_GtOperationHelper<T, U>::value, typename _GtOperationHelper<T, U>::type>::type operator>(const U &value) const
        {
            return this->Get() > value;
        }

        /**
         * @brief 大于运算
         */
        template <typename D, typename U = T>
        typename std::enable_if<_GtOperationHelper<T, U>::value, typename _GtOperationHelper<T, U>::type>::type operator>(const PropertyBase<U, D> &prop) const
        {
            return this->Get() > prop.Get();
        }

        /**
         * @brief 大于等于运算
         */
        template <typename U = T>
        typename std::enable_if<_GeOperationHelper<T, U>::value, typename _GeOperationHelper<T, U>::type>::type operator>=(const U &value) const
        {
            return this->Get() >= value;
        }

        /**
         * @brief 大于等于运算
         */
        template <typename D, typename U = T>
        typename std::enable_if<_GeOperationHelper<T, U>::value, typename _GeOperationHelper<T, U>::type>::type operator>=(const PropertyBase<U, D> &prop) const
        {
            return this->Get() >= prop.Get();
        }

        /**
         * @brief 按位与运算
         */
        template <typename U = T>
        typename std::enable_if<_BitAndOperationHelper<T, U>::value, typename _BitAndOperationHelper<T, U>::type>::type operator&(const U &value) const
        {
            return this->Get() & value;
        }

        /**
         * @brief 按位与运算
         */
        template <typename D, typename U = T>
        typename std::enable_if<_BitAndOperationHelper<T, U>::value, typename _BitAndOperationHelper<T, U>::type>::type operator&(const PropertyBase<U, D> &prop) const
        {
            return this->Get() & prop.Get();
        }

        /**
         * @brief 按位或运算
         */
        template <typename U = T>
        typename std::enable_if<_BitOrOperationHelper<T, U>::value, typename _BitOrOperationHelper<T, U>::type>::type operator|(const U &value) const
        {
            return this->Get() | value;
        }

        /**
         * @brief 按位或运算
         */
        template <typename D, typename U = T>
        typename std::enable_if<_BitOrOperationHelper<T, U>::value, typename _BitOrOperationHelper<T, U>::type>::type operator|(const PropertyBase<U, D> &prop) const
        {
            return this->Get() | prop.Get();
        }

        /**
         * @brief 按位异或运算
         */
        template <typename U = T>
        typename std::enable_if<_BitXorOperationHelper<T, U>::value, typename _BitXorOperationHelper<T, U>::type>::type operator^(const U &value) const
        {
            return this->Get() ^ value;
        }

        /**
         * @brief 按位异或运算
         */
        template <typename D, typename U = T>
        typename std::enable_if<_BitXorOperationHelper<T, U>::value, typename _BitXorOperationHelper<T, U>::type>::type operator^(const PropertyBase<U, D> &prop) const
        {
            return this->Get() ^ prop.Get();
        }

        /**
         * @brief 左移运算
         */
        template <typename U = T>
        typename std::enable_if<_ShlOperationHelper<T, U>::value, typename _ShlOperationHelper<T, U>::type>::type operator<<(const U &value) const
        {
            return this->Get() << value;
        }

        /**
         * @brief 左移运算
         */
        template <typename D, typename U = T>
        typename std::enable_if<_ShlOperationHelper<T, U>::value, typename _ShlOperationHelper<T, U>::type>::type operator<<(const PropertyBase<U, D> &prop) const
        {
            return this->Get() << prop.Get();
        }

        /**
         * @brief 右移运算
         */
        template <typename U = T>
        typename std::enable_if<_ShrOperationHelper<T, U>::value, typename _ShrOperationHelper<T, U>::type>::type operator>>(const U &value) const
        {
            return this->Get() >> value;
        }

        /**
         * @brief 右移运算
         */
        template <typename D, typename U = T>
        typename std::enable_if<_ShrOperationHelper<T, U>::value, typename _ShrOperationHelper<T, U>::type>::type operator>>(const PropertyBase<U, D> &prop) const
        {
            return this->Get() >> prop.Get();
        }

        /**
         * @brief 逻辑与运算
         */
        template <typename U = T>
        typename std::enable_if<_LogicAndOperationHelper<T, U>::value, typename _LogicAndOperationHelper<T, U>::type>::type operator&&(const U &value) const
        {
            return this->Get() && value;
        }

        /**
         * @brief 逻辑与运算
         */
        template <typename D, typename U = T>
        typename std::enable_if<_LogicAndOperationHelper<T, U>::value, typename _LogicAndOperationHelper<T, U>::type>::type operator&&(const PropertyBase<U, D> &prop) const
        {
            return this->Get() && prop.Get();
        }

        /**
         * @brief 逻辑或运算
         */
        template <typename U = T>
        typename std::enable_if<_LogicOrOperationHelper<T, U>::value, typename _LogicOrOperationHelper<T, U>::type>::type operator||(const U &value) const
        {
            return this->Get() || value;
        }

        /**
         * @brief 逻辑或运算
         */
        template <typename D, typename U = T>
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
                !std::is_pointer<T>::value &&
                !std::is_reference<typename _BracketOperationHelper<T, U>::type>::value,
            typename _BracketOperationHelper<T, U>::type>::type
        operator[](const U &value) const
        {
            return this->Get()[value];
        }

        /**
         * @brief 下标运算
         */
        template <typename D, typename U>
        typename std::enable_if<
            _BracketOperationHelper<T, U>::value &&
                !std::is_pointer<T>::value &&
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
        operator[](const U &value) const
        {
            return this->Get()[value];
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
    };

    /*================================================================================*/

    /**
     * @brief 加法运算
     */
    template <typename D, typename T, typename U = T>
    typename std::enable_if<!_IsProperty<T>::value && _AddOperationHelper<T &, U>::value, typename _AddOperationHelper<T &, U>::type>::type
    operator+(T &left, const PropertyBase<U, D> &right)
    {
        return left + right.Get();
    }

    /**
     * @brief 加法运算
     */
    template <typename D, typename T, typename U = T>
    typename std::enable_if<!_IsProperty<T>::value && _AddOperationHelper<const T &, U>::value, typename _AddOperationHelper<const T &, U>::type>::type
    operator+(const T &left, const PropertyBase<U, D> &right)
    {
        return left + right.Get();
    }

    /**
     * @brief 减法运算
     */
    template <typename D, typename T, typename U = T>
    typename std::enable_if<!_IsProperty<T>::value && _SubOperationHelper<T &, U>::value, typename _SubOperationHelper<T &, U>::type>::type
    operator-(T &left, const PropertyBase<U, D> &right)
    {
        return left - right.Get();
    }

    /**
     * @brief 减法运算
     */
    template <typename D, typename T, typename U = T>
    typename std::enable_if<!_IsProperty<T>::value && _SubOperationHelper<const T &, U>::value, typename _SubOperationHelper<const T &, U>::type>::type
    operator-(const T &left, const PropertyBase<U, D> &right)
    {
        return left - right.Get();
    }

    /**
     * @brief 乘法运算
     */
    template <typename D, typename T, typename U = T>
    typename std::enable_if<!_IsProperty<T>::value && _MulOperationHelper<T &, U>::value, typename _MulOperationHelper<T &, U>::type>::type
    operator*(T &left, const PropertyBase<U, D> &right)
    {
        return left * right.Get();
    }

    /**
     * @brief 乘法运算
     */
    template <typename D, typename T, typename U = T>
    typename std::enable_if<!_IsProperty<T>::value && _MulOperationHelper<const T &, U>::value, typename _MulOperationHelper<const T &, U>::type>::type
    operator*(const T &left, const PropertyBase<U, D> &right)
    {
        return left * right.Get();
    }

    /**
     * @brief 除法运算
     */
    template <typename D, typename T, typename U = T>
    typename std::enable_if<!_IsProperty<T>::value && _DivOperationHelper<T &, U>::value, typename _DivOperationHelper<T &, U>::type>::type
    operator/(T &left, const PropertyBase<U, D> &right)
    {
        return left / right.Get();
    }

    /**
     * @brief 除法运算
     */
    template <typename D, typename T, typename U = T>
    typename std::enable_if<!_IsProperty<T>::value && _DivOperationHelper<const T &, U>::value, typename _DivOperationHelper<const T &, U>::type>::type
    operator/(const T &left, const PropertyBase<U, D> &right)
    {
        return left / right.Get();
    }

    /**
     * @brief 取模运算
     */
    template <typename D, typename T, typename U = T>
    typename std::enable_if<!_IsProperty<T>::value && _ModOperationHelper<T &, U>::value, typename _ModOperationHelper<T &, U>::type>::type
    operator%(T &left, const PropertyBase<U, D> &right)
    {
        return left % right.Get();
    }

    /**
     * @brief 取模运算
     */
    template <typename D, typename T, typename U = T>
    typename std::enable_if<!_IsProperty<T>::value && _ModOperationHelper<const T &, U>::value, typename _ModOperationHelper<const T &, U>::type>::type
    operator%(const T &left, const PropertyBase<U, D> &right)
    {
        return left % right.Get();
    }

    /**
     * @brief 等于运算
     */
    template <typename D, typename T, typename U = T>
    typename std::enable_if<!_IsProperty<T>::value && _EqOperationHelper<T &, U>::value, typename _EqOperationHelper<T &, U>::type>::type
    operator==(T &left, const PropertyBase<U, D> &right)
    {
        return left == right.Get();
    }

    /**
     * @brief 等于运算
     */
    template <typename D, typename T, typename U = T>
    typename std::enable_if<!_IsProperty<T>::value && _EqOperationHelper<const T &, U>::value, typename _EqOperationHelper<const T &, U>::type>::type
    operator==(const T &left, const PropertyBase<U, D> &right)
    {
        return left == right.Get();
    }

    ///**
    // * @brief 不等于运算
    // */
    // template <typename D, typename T, typename U = T>
    // typename std::enable_if<!_IsProperty<T>::value && _NeOperationHelper<T &, U>::value, typename _NeOperationHelper<T &, U>::type>::type
    // operator!=(T &left, const PropertyBase<U, D> &right)
    //{
    //    return left != right.Get();
    //}

    ///**
    // * @brief 不等于运算
    // */
    // template <typename D, typename T, typename U = T>
    // typename std::enable_if<!_IsProperty<T>::value && _NeOperationHelper<const T &, U>::value, typename _NeOperationHelper<const T &, U>::type>::type
    // operator!=(const T &left, const PropertyBase<U, D> &right)
    //{
    //    return left != right.Get();
    //}

    /**
     * @brief 不等于运算
     * @note  避免与c++20自动生成的!=冲突，改为通过==取反实现
     */
    template <typename D, typename T, typename U = T>
    typename std::enable_if<!_IsProperty<T>::value && _EqOperationHelper<T &, U>::value, typename _EqOperationHelper<T &, U>::type>::type
    operator!=(T &left, const PropertyBase<U, D> &right)
    {
        return !(left == right);
    }

    /**
     * @brief 不等于运算
     * @note  避免与c++20自动生成的!=冲突，改为通过==取反实现
     */
    template <typename D, typename T, typename U = T>
    typename std::enable_if<!_IsProperty<T>::value && _EqOperationHelper<const T &, U>::value, typename _EqOperationHelper<const T &, U>::type>::type
    operator!=(const T &left, const PropertyBase<U, D> &right)
    {
        return !(left == right);
    }

    /**
     * @brief 小于运算
     */
    template <typename D, typename T, typename U = T>
    typename std::enable_if<!_IsProperty<T>::value && _LtOperationHelper<T &, U>::value, typename _LtOperationHelper<T &, U>::type>::type
    operator<(T &left, const PropertyBase<U, D> &right)
    {
        return left < right.Get();
    }

    /**
     * @brief 小于运算
     */
    template <typename D, typename T, typename U = T>
    typename std::enable_if<!_IsProperty<T>::value && _LtOperationHelper<const T &, U>::value, typename _LtOperationHelper<const T &, U>::type>::type
    operator<(const T &left, const PropertyBase<U, D> &right)
    {
        return left < right.Get();
    }

    /**
     * @brief 小于等于运算
     */
    template <typename D, typename T, typename U = T>
    typename std::enable_if<!_IsProperty<T>::value && _LeOperationHelper<T &, U>::value, typename _LeOperationHelper<T &, U>::type>::type
    operator<=(T &left, const PropertyBase<U, D> &right)
    {
        return left <= right.Get();
    }

    /**
     * @brief 小于等于运算
     */
    template <typename D, typename T, typename U = T>
    typename std::enable_if<!_IsProperty<T>::value && _LeOperationHelper<const T &, U>::value, typename _LeOperationHelper<const T &, U>::type>::type
    operator<=(const T &left, const PropertyBase<U, D> &right)
    {
        return left <= right.Get();
    }

    /**
     * @brief 大于运算
     */
    template <typename D, typename T, typename U = T>
    typename std::enable_if<!_IsProperty<T>::value && _GtOperationHelper<T &, U>::value, typename _GtOperationHelper<T &, U>::type>::type
    operator>(T &left, const PropertyBase<U, D> &right)
    {
        return left > right.Get();
    }

    /**
     * @brief 大于运算
     */
    template <typename D, typename T, typename U = T>
    typename std::enable_if<!_IsProperty<T>::value && _GtOperationHelper<const T &, U>::value, typename _GtOperationHelper<const T &, U>::type>::type
    operator>(const T &left, const PropertyBase<U, D> &right)
    {
        return left > right.Get();
    }

    /**
     * @brief 大于等于运算
     */
    template <typename D, typename T, typename U = T>
    typename std::enable_if<!_IsProperty<T>::value && _GeOperationHelper<T &, U>::value, typename _GeOperationHelper<T &, U>::type>::type
    operator>=(T &left, const PropertyBase<U, D> &right)
    {
        return left >= right.Get();
    }

    /**
     * @brief 大于等于运算
     */
    template <typename D, typename T, typename U = T>
    typename std::enable_if<!_IsProperty<T>::value && _GeOperationHelper<const T &, U>::value, typename _GeOperationHelper<const T &, U>::type>::type
    operator>=(const T &left, const PropertyBase<U, D> &right)
    {
        return left >= right.Get();
    }

    /**
     * @brief 按位与运算
     */
    template <typename D, typename T, typename U = T>
    typename std::enable_if<!_IsProperty<T>::value && _BitAndOperationHelper<T &, U>::value, typename _BitAndOperationHelper<T &, U>::type>::type
    operator&(T &left, const PropertyBase<U, D> &right)
    {
        return left & right.Get();
    }

    /**
     * @brief 按位与运算
     */
    template <typename D, typename T, typename U = T>
    typename std::enable_if<!_IsProperty<T>::value && _BitAndOperationHelper<const T &, U>::value, typename _BitAndOperationHelper<const T &, U>::type>::type
    operator&(const T &left, const PropertyBase<U, D> &right)
    {
        return left & right.Get();
    }

    /**
     * @brief 按位或运算
     */
    template <typename D, typename T, typename U = T>
    typename std::enable_if<!_IsProperty<T>::value && _BitOrOperationHelper<T &, U>::value, typename _BitOrOperationHelper<T &, U>::type>::type
    operator|(T &left, const PropertyBase<U, D> &right)
    {
        return left | right.Get();
    }

    /**
     * @brief 按位或运算
     */
    template <typename D, typename T, typename U = T>
    typename std::enable_if<!_IsProperty<T>::value && _BitOrOperationHelper<const T &, U>::value, typename _BitOrOperationHelper<const T &, U>::type>::type
    operator|(const T &left, const PropertyBase<U, D> &right)
    {
        return left | right.Get();
    }

    /**
     * @brief 按位异或运算
     */
    template <typename D, typename T, typename U = T>
    typename std::enable_if<!_IsProperty<T>::value && _BitXorOperationHelper<T &, U>::value, typename _BitXorOperationHelper<T &, U>::type>::type
    operator^(T &left, const PropertyBase<U, D> &right)
    {
        return left ^ right.Get();
    }

    /**
     * @brief 按位异或运算
     */
    template <typename D, typename T, typename U = T>
    typename std::enable_if<!_IsProperty<T>::value && _BitXorOperationHelper<const T &, U>::value, typename _BitXorOperationHelper<const T &, U>::type>::type
    operator^(const T &left, const PropertyBase<U, D> &right)
    {
        return left ^ right.Get();
    }

    /**
     * @brief 左移运算
     */
    template <typename D, typename T, typename U = T>
    typename std::enable_if<!_IsProperty<T>::value && _ShlOperationHelper<T &, U>::value, typename _ShlOperationHelper<T &, U>::type>::type
    operator<<(T &left, const PropertyBase<U, D> &right)
    {
        return left << right.Get();
    }

    /**
     * @brief 左移运算
     */
    template <typename D, typename T, typename U = T>
    typename std::enable_if<!_IsProperty<T>::value && _ShlOperationHelper<const T &, U>::value, typename _ShlOperationHelper<const T &, U>::type>::type
    operator<<(const T &left, const PropertyBase<U, D> &right)
    {
        return left << right.Get();
    }

    /**
     * @brief 右移运算
     */
    template <typename D, typename T, typename U = T>
    typename std::enable_if<!_IsProperty<T>::value && _ShrOperationHelper<T &, U>::value, typename _ShrOperationHelper<T &, U>::type>::type
    operator>>(T &left, const PropertyBase<U, D> &right)
    {
        return left >> right.Get();
    }

    /**
     * @brief 右移运算
     */
    template <typename D, typename T, typename U = T>
    typename std::enable_if<!_IsProperty<T>::value && _ShrOperationHelper<const T &, U>::value, typename _ShrOperationHelper<const T &, U>::type>::type
    operator>>(const T &left, const PropertyBase<U, D> &right)
    {
        return left >> right.Get();
    }

    /**
     * @brief 逻辑与运算
     */
    template <typename D, typename T, typename U = T>
    typename std::enable_if<!_IsProperty<T>::value && _LogicAndOperationHelper<T &, U>::value, typename _LogicAndOperationHelper<T &, U>::type>::type
    operator&&(T &left, const PropertyBase<U, D> &right)
    {
        return left && right.Get();
    }

    /**
     * @brief 逻辑与运算
     */
    template <typename D, typename T, typename U = T>
    typename std::enable_if<!_IsProperty<T>::value && _LogicAndOperationHelper<const T &, U>::value, typename _LogicAndOperationHelper<const T &, U>::type>::type
    operator&&(const T &left, const PropertyBase<U, D> &right)
    {
        return left && right.Get();
    }

    /**
     * @brief 逻辑或运算
     */
    template <typename D, typename T, typename U = T>
    typename std::enable_if<!_IsProperty<T>::value && _LogicOrOperationHelper<T &, U>::value, typename _LogicOrOperationHelper<T &, U>::type>::type
    operator||(T &left, const PropertyBase<U, D> &right)
    {
        return left || right.Get();
    }

    /**
     * @brief 逻辑或运算
     */
    template <typename D, typename T, typename U = T>
    typename std::enable_if<!_IsProperty<T>::value && _LogicOrOperationHelper<const T &, U>::value, typename _LogicOrOperationHelper<const T &, U>::type>::type
    operator||(const T &left, const PropertyBase<U, D> &right)
    {
        return left || right.Get();
    }

    /*================================================================================*/

    /**
     * @brief 属性
     */
    template <typename T>
    class Property : public PropertyBase<T, Property<T>>
    {
    public:
        using TBase = PropertyBase<T, Property<T>>;
        using FnGet = Func<T>;
        using FnSet = Action<const T &>;
        using TBase::operator=;

    private:
        FnGet _getter;
        FnSet _setter;

    public:
        /**
         * @brief 构造属性
         */
        Property(const FnGet &getter, const FnSet &setter)
            : _getter(getter), _setter(setter)
        {
        }

        /**
         * @brief 获取属性值
         */
        T GetterImpl() const
        {
            return this->_getter();
        }

        /**
         * @brief 设置属性值
         */
        void SetterImpl(const T &value) const
        {
            this->_setter(value);
        }
    };

    /**
     * @brief 只读属性
     */
    template <typename T>
    class ReadOnlyProperty : public PropertyBase<T, ReadOnlyProperty<T>>
    {
    public:
        using TBase = PropertyBase<T, ReadOnlyProperty<T>>;
        using FnGet = Func<T>;

    private:
        FnGet _getter;

    public:
        /**
         * @brief 构造只读属性
         */
        ReadOnlyProperty(const FnGet &getter)
            : _getter(getter)
        {
        }

        /**
         * @brief 获取属性值
         */
        T GetterImpl() const
        {
            return this->_getter();
        }
    };

    /**
     * @brief 只写属性
     */
    template <typename T>
    class WriteOnlyProperty : public PropertyBase<T, WriteOnlyProperty<T>>
    {
    public:
        using TBase = PropertyBase<T, WriteOnlyProperty<T>>;
        using FnSet = Action<const T &>;
        using TBase::operator=;

    private:
        FnSet _setter;

    public:
        /**
         * @brief 构造属性
         */
        WriteOnlyProperty(const FnSet &setter)
            : _setter(setter)
        {
        }

        /**
         * @brief 设置属性值
         */
        void SetterImpl(const T &value) const
        {
            this->_setter(value);
        }
    };
}

/*================================================================================*/

/**
 * 将常用类型的属性类声明为外部模板实例，以减少编译时间
 */

#include <cstdint>
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
