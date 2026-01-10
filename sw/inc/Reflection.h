#pragma once

// 定义SW_DISABLE_REFLECTION可以禁用反射相关功能
// 若该宏被定义，则相关功能会抛出runtime_error异常
// #define SW_DISABLE_REFLECTION

#include "Delegate.h"
#include "IComparable.h"
#include "IToString.h"
#include "Internal.h"
#include "Property.h"
#include <type_traits>
#include <typeindex>

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
        virtual ~DynamicObject() noexcept
        {
        }

        /**
         * @brief  判断对象是否为装箱对象
         * @return 如果对象为装箱对象则返回true，否则返回false
         */
        bool IsBoxedObject() const noexcept
        {
            return _isBoxedObject;
        }

        /**
         * @brief  获取对象的类型索引
         * @return 对象的类型索引
         */
        std::type_index GetTypeIndex() const
        {
#if defined(SW_DISABLE_REFLECTION)
            throw std::runtime_error("Reflection is disabled, cannot get type index.");
#else
            return typeid(*this);
#endif
        }

        /**
         * @brief      判断对象是否为指定类型
         * @tparam T   目标类型
         * @param pout 如果不为nullptr，则将转换后的指针赋值给该参数
         * @return     如果对象为指定类型则返回true，否则返回false
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
         * @brief      判断对象是否为指定类型
         * @tparam T   目标类型
         * @param pout 如果不为nullptr，则将转换后的指针赋值给该参数
         * @return     如果对象为指定类型则返回true，否则返回false
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
         * @brief    将对象动态转换为指定类型的引用
         * @tparam T 目标类型
         * @return   指定类型的引用
         * @throws   std::bad_cast 如果转换失败
         */
        template <typename T>
        auto DynamicCast()
            -> typename std::enable_if<std::is_base_of<DynamicObject, T>::value && _IsDynamicCastable<DynamicObject *, T *>::value, T &>::type
        {
#if defined(SW_DISABLE_REFLECTION)
            throw std::runtime_error("Reflection is disabled, cannot perform dynamic cast.");
#else
            return dynamic_cast<T &>(*this);
#endif
        }

        /**
         * @brief    将对象动态转换为指定类型的引用
         * @tparam T 目标类型
         * @return   指定类型的引用
         * @throws   std::bad_cast 如果转换失败
         */
        template <typename T>
        auto DynamicCast()
            -> typename std::enable_if<std::is_base_of<DynamicObject, T>::value && !_IsDynamicCastable<DynamicObject *, T *>::value, T &>::type
        {
#if defined(SW_DISABLE_REFLECTION)
            throw std::runtime_error("Reflection is disabled, cannot perform dynamic cast.");
#else
            throw std::bad_cast();
#endif
        }

        /**
         * @brief    将对象动态转换为指定类型的常量引用
         * @tparam T 目标类型
         * @return   指定类型的常量引用
         * @throws   std::bad_cast 如果转换失败
         */
        template <typename T>
        auto DynamicCast() const
            -> typename std::enable_if<std::is_base_of<DynamicObject, T>::value && _IsDynamicCastable<DynamicObject *, T *>::value, const T &>::type
        {
#if defined(SW_DISABLE_REFLECTION)
            throw std::runtime_error("Reflection is disabled, cannot perform dynamic cast.");
#else
            return dynamic_cast<const T &>(*this);
#endif
        }

        /**
         * @brief    将对象动态转换为指定类型的常量引用
         * @tparam T 目标类型
         * @return   指定类型的常量引用
         * @throws   std::bad_cast 如果转换失败
         */
        template <typename T>
        auto DynamicCast() const
            -> typename std::enable_if<std::is_base_of<DynamicObject, T>::value && !_IsDynamicCastable<DynamicObject *, T *>::value, const T &>::type
        {
#if defined(SW_DISABLE_REFLECTION)
            throw std::runtime_error("Reflection is disabled, cannot perform dynamic cast.");
#else
            throw std::bad_cast();
#endif
        }

        /**
         * @brief    将对象不安全地转换为指定类型的引用
         * @tparam T 目标类型
         * @return   指定类型的引用
         * @note     若目标类型与当前类型不兼容，则行为未定义
         */
        template <typename T>
        auto UnsafeCast()
            -> typename std::enable_if<
                std::is_base_of<DynamicObject, T>::value && _IsStaticCastable<DynamicObject *, T *>::value, T &>::type
        {
            return static_cast<T &>(*this);
        }

        /**
         * @brief    将对象不安全地转换为指定类型的引用
         * @tparam T 目标类型
         * @return   指定类型的引用
         * @note     若目标类型与当前类型不兼容，则行为未定义
         */
        template <typename T>
        auto UnsafeCast()
            -> typename std::enable_if<
                std::is_base_of<DynamicObject, T>::value && !_IsStaticCastable<DynamicObject *, T *>::value, T &>::type
        {
            return DynamicCast<T>();
        }

        /**
         * @brief    将对象不安全地转换为指定类型的引用
         * @tparam T 目标类型
         * @return   指定类型的引用
         * @note     若目标类型与当前类型不兼容，则行为未定义
         */
        template <typename T>
        auto UnsafeCast() const
            -> typename std::enable_if<
                std::is_base_of<DynamicObject, T>::value && _IsStaticCastable<DynamicObject *, T *>::value, const T &>::type
        {
            return static_cast<const T &>(*this);
        }

        /**
         * @brief    将对象不安全地转换为指定类型的引用
         * @tparam T 目标类型
         * @return   指定类型的引用
         * @note     若目标类型与当前类型不兼容，则行为未定义
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
         * @brief      判断对象是否为指定类型
         * @tparam T   目标类型
         * @param pout 如果不为nullptr，则将转换后的指针赋值给该参数
         * @return     如果对象为指定类型则返回true，否则返回false
         */
        template <typename T>
        auto IsType(T **pout = nullptr)
            -> typename std::enable_if<!std::is_base_of<DynamicObject, T>::value, bool>::type;

        /**
         * @brief      判断对象是否为指定类型
         * @tparam T   目标类型
         * @param pout 如果不为nullptr，则将转换后的指针赋值给该参数
         * @return     如果对象为指定类型则返回true，否则返回false
         */
        template <typename T>
        auto IsType(const T **pout = nullptr) const
            -> typename std::enable_if<!std::is_base_of<DynamicObject, T>::value, bool>::type;

        /**
         * @brief    将对象动态转换为指定类型的引用
         * @tparam T 目标类型
         * @return   指定类型的引用
         * @throws   std::bad_cast 如果转换失败
         */
        template <typename T>
        auto DynamicCast()
            -> typename std::enable_if<!std::is_base_of<DynamicObject, T>::value && _IsDynamicCastable<DynamicObject *, T *>::value, T &>::type;

        /**
         * @brief    将对象动态转换为指定类型的引用
         * @tparam T 目标类型
         * @return   指定类型的引用
         * @throws   std::bad_cast 如果转换失败
         */
        template <typename T>
        auto DynamicCast()
            -> typename std::enable_if<!std::is_base_of<DynamicObject, T>::value && !_IsDynamicCastable<DynamicObject *, T *>::value, T &>::type;

        /**
         * @brief    将对象动态转换为指定类型的常量引用
         * @tparam T 目标类型
         * @return   指定类型的常量引用
         * @throws   std::bad_cast 如果转换失败
         */
        template <typename T>
        auto DynamicCast() const
            -> typename std::enable_if<!std::is_base_of<DynamicObject, T>::value && _IsDynamicCastable<DynamicObject *, T *>::value, const T &>::type;

        /**
         * @brief    将对象动态转换为指定类型的常量引用
         * @tparam T 目标类型
         * @return   指定类型的常量引用
         * @throws   std::bad_cast 如果转换失败
         */
        template <typename T>
        auto DynamicCast() const
            -> typename std::enable_if<!std::is_base_of<DynamicObject, T>::value && !_IsDynamicCastable<DynamicObject *, T *>::value, const T &>::type;

        /**
         * @brief    将对象不安全地转换为指定类型的引用
         * @tparam T 目标类型
         * @return   指定类型的引用
         * @note     若目标类型与当前类型不兼容，则行为未定义
         */
        template <typename T>
        auto UnsafeCast()
            -> typename std::enable_if<!std::is_base_of<DynamicObject, T>::value && _IsStaticCastable<DynamicObject *, T *>::value, T &>::type;

        /**
         * @brief    将对象不安全地转换为指定类型的引用
         * @tparam T 目标类型
         * @return   指定类型的引用
         * @note     若目标类型与当前类型不兼容，则行为未定义
         */
        template <typename T>
        auto UnsafeCast()
            -> typename std::enable_if<!std::is_base_of<DynamicObject, T>::value && !_IsStaticCastable<DynamicObject *, T *>::value, T &>::type;

        /**
         * @brief    将对象不安全地转换为指定类型的引用
         * @tparam T 目标类型
         * @return   指定类型的引用
         * @note     若目标类型与当前类型不兼容，则行为未定义
         */
        template <typename T>
        auto UnsafeCast() const
            -> typename std::enable_if<!std::is_base_of<DynamicObject, T>::value && _IsStaticCastable<DynamicObject *, T *>::value, const T &>::type;

        /**
         * @brief    将对象不安全地转换为指定类型的引用
         * @tparam T 目标类型
         * @return   指定类型的引用
         * @note     若目标类型与当前类型不兼容，则行为未定义
         */
        template <typename T>
        auto UnsafeCast() const
            -> typename std::enable_if<!std::is_base_of<DynamicObject, T>::value && !_IsStaticCastable<DynamicObject *, T *>::value, const T &>::type;
    };

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
         * @brief  比较字段ID
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
         * @brief         获取字段的唯一标识符
         * @tparam T      字段所属类类型
         * @tparam TField 字段类型
         * @param field   字段的成员指针
         * @return        对应的字段ID
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
         * @brief        获取成员函数的委托
         * @tparam T     成员函数所属类类型
         * @tparam TRet  成员函数返回值类型
         * @tparam Args  成员函数参数类型列表
         * @param method 成员函数指针
         * @return       对应的委托
         */
        template <typename T, typename TRet, typename... Args>
        static auto GetMethod(TRet (T::*method)(Args...)) -> Delegate<TRet(DynamicObject &, Args...)>
        {
            return [method](DynamicObject &obj, Args... args) -> TRet {
                return (obj.UnsafeCast<T>().*method)(std::forward<Args>(args)...);
            };
        }

        /**
         * @brief        获取常量成员函数的委托
         * @tparam T     成员函数所属类类型
         * @tparam TRet  成员函数返回值类型
         * @tparam Args  成员函数参数类型列表
         * @param method 成员函数指针
         * @return       对应的委托
         */
        template <typename T, typename TRet, typename... Args>
        static auto GetMethod(TRet (T::*method)(Args...) const) -> Delegate<TRet(DynamicObject &, Args...)>
        {
            return [method](DynamicObject &obj, Args... args) -> TRet {
                return (obj.UnsafeCast<T>().*method)(std::forward<Args>(args)...);
            };
        }

        /**
         * @brief         获取字段的访问器
         * @tparam T      字段所属类类型
         * @tparam TField 字段类型
         * @param field   字段的成员指针
         * @return        对应的访问器
         */
        template <typename T, typename TField>
        static auto GetFieldAccessor(TField T::*field) -> Delegate<TField &(DynamicObject &)>
        {
            return [field](DynamicObject &obj) -> TField & {
                return obj.UnsafeCast<T>().*field;
            };
        }

        /**
         * @brief            获取属性的Getter委托
         * @tparam T         属性所属类类型
         * @tparam TProperty 属性类型
         * @param prop       属性指针
         * @return           对应的Getter委托
         * @note             若属性不可读则返回空委托
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
         * @brief            获取属性的Getter委托
         * @tparam T         属性所属类类型
         * @tparam TProperty 属性类型
         * @param prop       属性指针
         * @return           对应的Getter委托
         * @note             若属性不可读则返回空委托
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
         * @brief            获取属性的Setter委托
         * @tparam T         属性所属类类型
         * @tparam TProperty 属性类型
         * @param prop       属性指针
         * @return           对应的Setter委托
         * @note             若属性不可写则返回空委托
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
         * @brief            获取属性的Setter委托
         * @tparam T         属性所属类类型
         * @tparam TProperty 属性类型
         * @param prop       属性指针
         * @return           对应的Setter委托
         * @note             若属性不可写则返回空委托
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
         * @brief        调用成员函数
         * @tparam T     成员函数所属类类型
         * @tparam TFunc 成员函数类型
         * @tparam Args  成员函数参数类型列表
         * @param method 成员函数的委托
         * @param obj    对象引用
         * @param args   成员函数参数列表
         * @return       成员函数返回值
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
         * @brief          访问字段
         * @tparam T       字段所属类类型
         * @tparam TField  字段类型
         * @param accessor 字段访问器的委托
         * @param obj      对象引用
         * @return         字段引用
         */
        template <typename T, typename TField>
        static auto AccessField(const Delegate<TField &(DynamicObject &)> &accessor, T &obj)
            -> typename std::enable_if<std::is_base_of<DynamicObject, T>::value, TField &>::type
        {
            assert(accessor != nullptr);
            return accessor(obj);
        }

        /**
         * @brief         获取属性值
         * @tparam T      属性所属类类型
         * @tparam TValue 属性值类型
         * @param getter  属性Getter的委托
         * @param obj     对象引用
         * @return        属性值
         */
        template <typename T, typename TValue>
        static auto GetProperty(const Delegate<TValue(DynamicObject &)> &getter, T &obj)
            -> typename std::enable_if<std::is_base_of<DynamicObject, T>::value, TValue>::type
        {
            assert(getter != nullptr);
            return getter(obj);
        }

        /**
         * @brief         设置属性值
         * @tparam T      属性所属类类型
         * @tparam TParam 属性Setter参数类型
         * @tparam TValue 属性值类型
         * @param setter  属性Setter的委托
         * @param obj     对象引用
         * @param value   属性值
         */
        template <typename T, typename TParam, typename TValue>
        static auto SetProperty(const Delegate<void(DynamicObject &, TParam)> &setter, T &obj, TValue &&value)
            -> typename std::enable_if<std::is_base_of<DynamicObject, T>::value>::type
        {
            assert(setter != nullptr);
            setter(obj, std::forward<TValue>(value));
        }

    public:
        /**
         * @brief        调用成员函数
         * @tparam T     成员函数所属类类型
         * @tparam TFunc 成员函数类型
         * @tparam Args  成员函数参数类型列表
         * @param method 成员函数的委托
         * @param obj    对象引用
         * @param args   成员函数参数列表
         * @return       成员函数返回值
         */
        template <typename T, typename TFunc, typename... Args>
        static auto InvokeMethod(const Delegate<TFunc> &method, T &obj, Args &&...args)
            -> typename std::enable_if<!std::is_base_of<DynamicObject, T>::value,
                                       decltype(method(std::declval<DynamicObject &>(), std::forward<Args>(args)...))>::type;

        /**
         * @brief          访问字段
         * @tparam T       字段所属类类型
         * @tparam TField  字段类型
         * @param accessor 字段访问器的委托
         * @param obj      对象引用
         * @return         字段引用
         */
        template <typename T, typename TField>
        static auto AccessField(const Delegate<TField &(DynamicObject &)> &accessor, T &obj)
            -> typename std::enable_if<!std::is_base_of<DynamicObject, T>::value, TField &>::type;

        /**
         * @brief         获取属性值
         * @tparam T      属性所属类类型
         * @tparam TValue 属性值类型
         * @param getter  属性Getter的委托
         * @param obj     对象引用
         * @return        属性值
         */
        template <typename T, typename TValue>
        static auto GetProperty(const Delegate<TValue(DynamicObject &)> &getter, T &obj)
            -> typename std::enable_if<!std::is_base_of<DynamicObject, T>::value, TValue>::type;

        /**
         * @brief         设置属性值
         * @tparam T      属性所属类类型
         * @tparam TParam 属性Setter参数类型
         * @tparam TValue 属性值类型
         * @param setter  属性Setter的委托
         * @param obj     对象引用
         * @param value   属性值
         */
        template <typename T, typename TParam, typename TValue>
        static auto SetProperty(const Delegate<void(DynamicObject &, TParam)> &setter, T &obj, TValue &&value)
            -> typename std::enable_if<!std::is_base_of<DynamicObject, T>::value>::type;
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
