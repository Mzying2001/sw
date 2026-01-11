#pragma once

#include "INotifyPropertyChanged.h"
#include "Property.h"

/*================================================================================*/

/**
 * SW_DEFINE_*_PROPERTY系列宏：
 * 用于简化对字段的属性封装，该系列宏均支持自定义Getter和Setter函数，只需在类中定义对应的Get_{field name}和
 * Set_{field name}函数即可，生成的属性会自动调用这些函数，否则会直接访问字段本身。
 *
 * - SW_DEFINE_PROPERTY(name, field)
 * - SW_DEFINE_READONLY_PROPERTY(name, field)
 * - SW_DEFINE_WRITEONLY_PROPERTY(name, field)
 * - SW_DEFINE_NOTIFY_PROPERTY(name, field)
 *
 * SW_DEFINE_EXPR_*_PROPERTY系列宏：
 * 用于简化对this->{expr}表达式的属性封装，当表达式结果是属性时会进行转发调用，否则直接访问表达式本身，该系列宏
 * 不支持自定义Getter和Setter函数。
 *
 * - SW_DEFINE_EXPR_PROPERTY(name, expr)
 * - SW_DEFINE_EXPR_READONLY_PROPERTY(name, expr)
 * - SW_DEFINE_EXPR_WRITEONLY_PROPERTY(name, expr)
 * - SW_DEFINE_EXPR_NOTIFY_PROPERTY(name, expr)
 */

/*================================================================================*/

/**
 * _Get_{field name}: 当前类有自定义的Get_{field name}函数时，调用该函数获取field值，否则直接访问field字段
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
        return self.##field;                                                                             \
    }

/**
 * _Set_{field name}: 当前类有自定义的Set_{field name}函数时，调用该函数设置field值，否则直接访问field字段
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
        self.##field = std::forward<U>(value);                                           \
    }

/*================================================================================*/

/**
 * 定义基于字段的属性，若类中有自定义的Get_{field name}和Set_{field name}函数，则会调用这些函数，否则直接访问字段
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
 * 定义基于字段的只读属性，若类中有自定义的Get_{field name}函数，则会调用该函数，否则直接访问字段
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
 * 定义基于字段的只写属性，若类中有自定义的Set_{field name}函数，则会调用该函数，否则直接访问字段
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
 * 定义基于字段的通知属性，若类中有自定义的Get_{field name}和Set_{field name}函数，则会调用这些函数，否则直接访问字段
 * 该宏应在实现了INotifyPropertyChanged接口的类中使用，若未自定义Setter则会尝试比较并在字段发生变更时触发属性变更通知
 */
#define SW_DEFINE_NOTIFY_PROPERTY(name, field)                                                                      \
    _SW_DEFINE_STATIC_GETTER(field);                                                                                \
    template <typename T, typename = void>                                                                          \
    struct _HasUserSetter_##field : std::false_type {                                                               \
    };                                                                                                              \
    template <typename T>                                                                                           \
    struct _HasUserSetter_##field<T, decltype(void(&T::Set_##field))> : std::true_type {                            \
    };                                                                                                              \
    template <typename T, typename U>                                                                               \
    static auto _Set_##field(T &self, U &&value)                                                                    \
        -> typename std::enable_if<_HasUserSetter_##field<T>::value>::type                                          \
    {                                                                                                               \
        self.Set_##field(std::forward<U>(value));                                                                   \
    }                                                                                                               \
    template <typename T, typename U>                                                                               \
    static auto _Set_##field(T &self, U &&value)                                                                    \
        -> typename std::enable_if<!_HasUserSetter_##field<T>::value && sw::_EqOperationHelper<U, U>::value>::type  \
    {                                                                                                               \
        if (!(_Get_##field(self) == value)) {                                                                       \
            self.##field = std::forward<U>(value);                                                                  \
            if (self.PropertyChanged) self.PropertyChanged(self, sw::Reflection::GetFieldId(&T::##name));           \
        }                                                                                                           \
    }                                                                                                               \
    template <typename T, typename U>                                                                               \
    static auto _Set_##field(T &self, U &&value)                                                                    \
        -> typename std::enable_if<!_HasUserSetter_##field<T>::value && !sw::_EqOperationHelper<U, U>::value>::type \
    {                                                                                                               \
        self.##field = std::forward<U>(value);                                                                      \
        if (self.PropertyChanged) self.PropertyChanged(self, sw::Reflection::GetFieldId(&T::##name));               \
    }                                                                                                               \
    sw::Property<decltype(field)> name                                                                              \
    {                                                                                                               \
        sw::Property<decltype(field)>::Init(this)                                                                   \
            .Getter([](auto self) {                                                                                 \
                return _Get_##field(*self);                                                                         \
            })                                                                                                      \
            .Setter([](auto self, auto value) {                                                                     \
                _Set_##field(*self, value);                                                                         \
            })                                                                                                      \
    }

/*================================================================================*/

/**
 * 定义表达式属性值类型辅助模板结构体
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
 * 计算表达式的值类型，若表达式是属性则取其属性值类型，否则取表达式类型本身
 */
#define _SW_EXPR_PROPERTY_VALUETYPE(propname, expr) \
    typename _ExprPropertyValueTypeHelper_##propname<decltype(expr)>::type

/**
 * _Get_{propname}: 若表达式是属性则调用其Get函数获取值，否则直接访问表达式
 */
#define _SW_DEFINE_EXPR_STATIC_GETTER(propname, expr)                                   \
    template <typename T, typename U = decltype(expr)>                                  \
    static auto _Get_##propname(T &self)                                                \
        -> typename std::enable_if<sw::_IsProperty<U>::value, typename U::TValue>::type \
    {                                                                                   \
        return (self.##expr).Get();                                                     \
    }                                                                                   \
    template <typename T, typename U = decltype(expr)>                                  \
    static auto _Get_##propname(T &self)                                                \
        -> typename std::enable_if<!sw::_IsProperty<U>::value, U>::type                 \
    {                                                                                   \
        return (self.##expr);                                                           \
    }

/**
 * _Set_{propname}: 若表达式是属性则调用其Set函数设置值，否则直接访问表达式
 */
#define _SW_DEFINE_EXPR_STATIC_SETTER(propname, expr)                \
    template <typename T, typename U, typename V = decltype(expr)>   \
    static auto _Set_##propname(T &self, U &&value)                  \
        -> typename std::enable_if<sw::_IsProperty<V>::value>::type  \
    {                                                                \
        (self.##expr).Set(std::forward<U>(value));                   \
    }                                                                \
    template <typename T, typename U, typename V = decltype(expr)>   \
    static auto _Set_##propname(T &self, U &&value)                  \
        -> typename std::enable_if<!sw::_IsProperty<V>::value>::type \
    {                                                                \
        (self.##expr) = std::forward<U>(value);                      \
    }

/*================================================================================*/

/**
 * 定义基于表达式的属性，访问this->{expr}，不支持自定义Getter和Setter
 * 当表达式为属性时，Getter和Setter会调用该属性的Get和Set函数，否则直接访问表达式
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
 * 定义基于表达式的只读属性，访问this->{expr}，不支持自定义Getter
 * 当表达式为属性时，Getter会调用该属性的Get函数，否则直接访问表达式
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
 * 定义基于表达式的只写属性，访问this->{expr}，不支持自定义Setter
 * 当表达式为属性时，Setter会调用该属性的Set函数，否则直接访问表达式
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
 * 定义基于表达式的通知属性，访问this->{expr}，不支持自定义Getter和Setter
 * 当表达式为属性时，Getter和Setter会调用该属性的Get和Set函数，否则直接访问表达式
 * 该宏应在实现了INotifyPropertyChanged接口的类中使用，Setter会尝试比较并在表达式的值发生变更时触发属性变更通知
 */
#define SW_DEFINE_EXPR_NOTIFY_PROPERTY(name, expr)                                                           \
    _SW_DEFINE_EXPR_PROPERTY_VALUETYPE_HELPER(name);                                                         \
    _SW_DEFINE_EXPR_STATIC_GETTER(name, expr);                                                               \
    template <typename T, typename U, typename V = decltype(expr)>                                           \
    static auto _Set_##name(T &self, U &&value)                                                              \
        -> typename std::enable_if<sw::_IsProperty<V>::value && sw::_EqOperationHelper<U, U>::value>::type   \
    {                                                                                                        \
        if (!((self.##expr).Get() == value)) {                                                               \
            (self.##expr).Set(std::forward<U>(value));                                                       \
            if (self.PropertyChanged) self.PropertyChanged(self, sw::Reflection::GetFieldId(&T::##name));    \
        }                                                                                                    \
    }                                                                                                        \
    template <typename T, typename U, typename V = decltype(expr)>                                           \
    static auto _Set_##name(T &self, U &&value)                                                              \
        -> typename std::enable_if<sw::_IsProperty<V>::value && !sw::_EqOperationHelper<U, U>::value>::type  \
    {                                                                                                        \
        (self.##expr).Set(std::forward<U>(value));                                                           \
        if (self.PropertyChanged) self.PropertyChanged(self, sw::Reflection::GetFieldId(&T::##name));        \
    }                                                                                                        \
    template <typename T, typename U, typename V = decltype(expr)>                                           \
    static auto _Set_##name(T &self, U &&value)                                                              \
        -> typename std::enable_if<!sw::_IsProperty<V>::value && sw::_EqOperationHelper<U, U>::value>::type  \
    {                                                                                                        \
        if (!((self.##expr) == value)) {                                                                     \
            (self.##expr) = std::forward<U>(value);                                                          \
            if (self.PropertyChanged) self.PropertyChanged(self, sw::Reflection::GetFieldId(&T::##name));    \
        }                                                                                                    \
    }                                                                                                        \
    template <typename T, typename U, typename V = decltype(expr)>                                           \
    static auto _Set_##name(T &self, U &&value)                                                              \
        -> typename std::enable_if<!sw::_IsProperty<V>::value && !sw::_EqOperationHelper<U, U>::value>::type \
    {                                                                                                        \
        (self.##expr) = std::forward<U>(value);                                                              \
        if (self.PropertyChanged) self.PropertyChanged(self, sw::Reflection::GetFieldId(&T::##name));        \
    }                                                                                                        \
    sw::Property<_SW_EXPR_PROPERTY_VALUETYPE(name, expr)> name                                               \
    {                                                                                                        \
        sw::Property<_SW_EXPR_PROPERTY_VALUETYPE(name, expr)>::Init(this)                                    \
            .Getter([](auto self) {                                                                          \
                return _Get_##name(*self);                                                                   \
            })                                                                                               \
            .Setter([](auto self, auto value) {                                                              \
                _Set_##name(*self, value);                                                                   \
            })                                                                                               \
    }

/*================================================================================*/
