#pragma once

#include <type_traits>

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
