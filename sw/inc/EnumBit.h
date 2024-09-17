#pragma once

#include <type_traits>

namespace sw
{
    /**
     * @brief 用于标记枚举是否支持位运算
     */
    template <typename T>
    struct _EnumSupportBitOperations : std::false_type {
    };

    /**
     * @brief 为标记_EnumSupportBitOperations的枚举类型提供按位或运算
     */
    template <typename T>
    inline constexpr typename std::enable_if<std::is_enum<T>::value && _EnumSupportBitOperations<T>::value, T>::type
    operator|(T a, T b)
    {
        using TUnderlying = typename std::underlying_type<T>::type;
        return static_cast<T>(static_cast<TUnderlying>(a) | static_cast<TUnderlying>(b));
    }

    /**
     * @brief 为标记_EnumSupportBitOperations的枚举类型提供按位或赋值运算
     */
    template <typename T>
    inline constexpr typename std::enable_if<std::is_enum<T>::value && _EnumSupportBitOperations<T>::value, T &>::type
    operator|=(T &a, T b)
    {
        return a = a | b;
    }

    /**
     * @brief 为标记_EnumSupportBitOperations的枚举类型提供按位与运算
     */
    template <typename T>
    inline constexpr typename std::enable_if<std::is_enum<T>::value && _EnumSupportBitOperations<T>::value, T>::type
    operator&(T a, T b)
    {
        using TUnderlying = typename std::underlying_type<T>::type;
        return static_cast<T>(static_cast<TUnderlying>(a) & static_cast<TUnderlying>(b));
    }

    /**
     * @brief 为标记_EnumSupportBitOperations的枚举类型提供按位与赋值运算
     */
    template <typename T>
    inline constexpr typename std::enable_if<std::is_enum<T>::value && _EnumSupportBitOperations<T>::value, T &>::type
    operator&=(T &a, T b)
    {
        return a = a & b;
    }

    /**
     * @brief 为标记_EnumSupportBitOperations的枚举类型提供按位异或运算
     */
    template <typename T>
    inline constexpr typename std::enable_if<std::is_enum<T>::value && _EnumSupportBitOperations<T>::value, T>::type
    operator^(T a, T b)
    {
        using TUnderlying = typename std::underlying_type<T>::type;
        return static_cast<T>(static_cast<TUnderlying>(a) ^ static_cast<TUnderlying>(b));
    }

    /**
     * @brief 为标记_EnumSupportBitOperations的枚举类型提供按位异或赋值运算
     */
    template <typename T>
    inline constexpr typename std::enable_if<std::is_enum<T>::value && _EnumSupportBitOperations<T>::value, T &>::type
    operator^=(T &a, T b)
    {
        return a = a ^ b;
    }

    /**
     * @brief 为标记_EnumSupportBitOperations的枚举类型提供按位取反运算
     */
    template <typename T>
    inline constexpr typename std::enable_if<std::is_enum<T>::value && _EnumSupportBitOperations<T>::value, T>::type
    operator~(T a)
    {
        using TUnderlying = typename std::underlying_type<T>::type;
        return static_cast<T>(~static_cast<TUnderlying>(a));
    }
}
