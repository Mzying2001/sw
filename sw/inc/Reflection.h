#pragma once

// 定义_SW_DISABLE_REFLECTION可以禁用反射相关功能
// 若该宏被定义，则相关功能会抛出runtime_error异常
// #define _SW_DISABLE_REFLECTION

#include "Delegate.h"
#include "Property.h"
#include <type_traits>
#include <typeindex>

namespace sw
{
    /**
     * @brief 动态对象基类
     */
    class DynamicObject
    {
    public:
        /**
         * @brief 析构函数
         */
        virtual ~DynamicObject() = default;

        /**
         * @brief  获取对象的类型索引
         * @return 对象的类型索引
         */
        inline std::type_index GetTypeIndex() const
        {
#if defined(_SW_DISABLE_REFLECTION)
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
        bool IsType(T **pout = nullptr)
        {
#if defined(_SW_DISABLE_REFLECTION)
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
        bool IsType(const T **pout = nullptr) const
        {
#if defined(_SW_DISABLE_REFLECTION)
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
        T &DynamicCast()
        {
#if defined(_SW_DISABLE_REFLECTION)
            throw std::runtime_error("Reflection is disabled, cannot perform dynamic cast.");
#else
            return dynamic_cast<T &>(*this);
#endif
        }

        /**
         * @brief    将对象动态转换为指定类型的常量引用
         * @tparam T 目标类型
         * @return   指定类型的常量引用
         * @throws   std::bad_cast 如果转换失败
         */
        template <typename T>
        const T &DynamicCast() const
        {
#if defined(_SW_DISABLE_REFLECTION)
            throw std::runtime_error("Reflection is disabled, cannot perform dynamic cast.");
#else
            return dynamic_cast<const T &>(*this);
#endif
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

        /**
         * @brief        获取成员函数的委托
         * @tparam T     成员函数所属类类型
         * @tparam TRet  成员函数返回值类型
         * @tparam Args  成员函数参数类型列表
         * @param method 成员函数指针
         * @return       对应的委托
         */
        template <typename T, typename TRet, typename... Args>
        static auto GetMethod(TRet (T::*method)(Args...))
            -> typename std::enable_if<
                std::is_base_of<DynamicObject, T>::value, Delegate<TRet(DynamicObject &, Args...)>>::type
        {
            return [method](DynamicObject &obj, Args... args) -> TRet {
                return (obj.DynamicCast<T>().*method)(std::forward<Args>(args)...);
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
        static auto GetMethod(TRet (T::*method)(Args...) const)
            -> typename std::enable_if<
                std::is_base_of<DynamicObject, T>::value, Delegate<TRet(DynamicObject &, Args...)>>::type
        {
            return [method](DynamicObject &obj, Args... args) -> TRet {
                return (obj.DynamicCast<T>().*method)(std::forward<Args>(args)...);
            };
        }

        /**
         * @brief         获取成员字段的访问器
         * @tparam T      成员字段所属类类型
         * @tparam TField 成员字段类型
         * @param field   成员字段指针
         * @return        对应的访问器
         */
        template <typename T, typename TField>
        static auto GetFieldAccessor(TField T::*field)
            -> typename std::enable_if<
                std::is_base_of<DynamicObject, T>::value, Delegate<TField &(DynamicObject &)>>::type
        {
            return [field](DynamicObject &obj) -> TField & {
                return obj.DynamicCast<T>().*field;
            };
        }

        /**
         * @brief            获取属性的Getter委托
         * @tparam T         属性所属类类型
         * @tparam TProperty 属性类型
         * @param prop       属性指针
         * @return           对应的Getter委托
         */
        template <typename T, typename TProperty>
        static auto GetPropertyGetter(TProperty T::*prop)
            -> typename std::enable_if<
                std::is_base_of<DynamicObject, T>::value && _IsReadableProperty<TProperty>::value,
                Delegate<typename TProperty::TValue(DynamicObject &)>>::type
        {
            return [prop](DynamicObject &obj) -> typename TProperty::TValue {
                return (obj.DynamicCast<T>().*prop).Get();
            };
        }

        /**
         * @brief            获取属性的Setter委托
         * @tparam T         属性所属类类型
         * @tparam TProperty 属性类型
         * @param prop       属性指针
         * @return           对应的Setter委托
         */
        template <typename T, typename TProperty>
        static auto GetPropertySetter(TProperty T::*prop)
            -> typename std::enable_if<
                std::is_base_of<DynamicObject, T>::value && _IsWritableProperty<TProperty>::value,
                Delegate<void(DynamicObject &, const typename TProperty::TValue &)>>::type
        {
            return [prop](DynamicObject &obj, const typename TProperty::TValue &value) {
                (obj.DynamicCast<T>().*prop).Set(value);
            };
        }
    };
}
