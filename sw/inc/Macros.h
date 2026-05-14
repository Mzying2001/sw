#pragma once

#include "INotifyPropertyChanged.h"
#include "Property.h"

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
