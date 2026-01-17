#pragma once

#include <type_traits>

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
}
