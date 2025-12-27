#pragma once

#include "INotifyPropertyChanged.h"
#include "Property.h"

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

/*================================================================================*/

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
        -> typename std::enable_if<!_HasUserSetter_##field<T>::value && sw::_EqOperationHelper<T, U>::value>::type  \
    {                                                                                                               \
        if (!(_Get_##field(self) == value)) {                                                                       \
            self.##field = std::forward<U>(value);                                                                  \
            if (self.PropertyChanged) self.PropertyChanged(self, sw::Reflection::GetFieldId(&T::##name));           \
        }                                                                                                           \
    }                                                                                                               \
    template <typename T, typename U>                                                                               \
    static auto _Set_##field(T &self, U &&value)                                                                    \
        -> typename std::enable_if<!_HasUserSetter_##field<T>::value && !sw::_EqOperationHelper<T, U>::value>::type \
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
 * 定义简单属性，直接访问指定路径的字段，不支持自定义Getter和Setter
 */
#define SW_DEFINE_SIMPLE_PROPERTY(name, path)    \
    sw::Property<decltype(path)> name            \
    {                                            \
        sw::Property<decltype(path)>::Init(this) \
            .Getter([](auto self) {              \
                return self->##path;             \
            })                                   \
            .Setter([](auto self, auto value) {  \
                self->##path = value;            \
            })                                   \
    }

/**
 * 定义简单只读属性，直接访问指定路径的字段，不支持自定义Getter
 */
#define SW_DEFINE_SIMPLE_READONLY_PROPERTY(name, path) \
    sw::ReadOnlyProperty<decltype(path)> name          \
    {                                                  \
        sw::Property<decltype(path)>::Init(this)       \
            .Getter([](auto self) {                    \
                return self->##path;                   \
            })                                         \
    }

/**
 * 定义简单只写属性，直接访问指定路径的字段，不支持自定义Setter
 */
#define SW_DEFINE_SIMPLE_WRITEONLY_PROPERTY(name, path) \
    sw::WriteOnlyProperty<decltype(path)> name          \
    {                                                   \
        sw::Property<decltype(path)>::Init(this)        \
            .Setter([](auto self, auto value) {         \
                self->##path = value;                   \
            })                                          \
    }

/**
 * 定义简单通知属性，直接访问指定路径的字段，不支持自定义Getter和Setter
 * 该宏应在实现了INotifyPropertyChanged接口的类中使用，若未自定义Setter则会尝试比较并在字段发生变更时触发属性变更通知
 */
#define SW_DEFINE_SIMPLE_NOTIFY_PROPERTY(name, path)                                                      \
    template <typename T, typename U>                                                                     \
    static auto _Set_##name(T &self, U &&value)                                                           \
        -> typename std::enable_if<sw::_EqOperationHelper<U, U>::value>::type                             \
    {                                                                                                     \
        if (!(self.##path == value)) {                                                                    \
            self.##path = std::forward<U>(value);                                                         \
            if (self.PropertyChanged) self.PropertyChanged(self, sw::Reflection::GetFieldId(&T::##name)); \
        }                                                                                                 \
    }                                                                                                     \
    template <typename T, typename U>                                                                     \
    static auto _Set_##name(T &self, U &&value)                                                           \
        -> typename std::enable_if<!sw::_EqOperationHelper<U, U>::value>::type                            \
    {                                                                                                     \
        self.##path = std::forward<U>(value);                                                             \
        if (self.PropertyChanged) self.PropertyChanged(self, sw::Reflection::GetFieldId(&T::##name));     \
    }                                                                                                     \
    sw::Property<decltype(path)> name                                                                     \
    {                                                                                                     \
        sw::Property<decltype(path)>::Init(this)                                                          \
            .Getter([](auto self) {                                                                       \
                return self->##path;                                                                      \
            })                                                                                            \
            .Setter([](auto self, auto value) {                                                           \
                _Set_##name(*self, value);                                                                \
            })                                                                                            \
    }

/*================================================================================*/
