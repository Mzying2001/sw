#pragma once

#include "IValueConverter.h"
#include <cstdint>
#include <type_traits>

namespace sw
{
    /**
     * @brief 数值类型转换器，将源数值类型转换为目标数值类型
     * @tparam TSource 源数值类型
     * @tparam TTarget 目标数值类型
     */
    template <
        typename TSource,
        typename TTarget,
        std::enable_if<std::is_arithmetic<TSource>::value && std::is_arithmetic<TTarget>::value, int>::type = 0>
    class NumericConverter : public IValueConverter<TSource, TTarget>
    {
    public:
        virtual TTarget Convert(TSource source) override
        {
            return static_cast<TTarget>(source);
        }
        virtual TSource ConvertBack(TTarget target) override
        {
            return static_cast<TTarget>(target);
        }
    };

    using IntToFloatConverter  = NumericConverter<int, float>;
    using IntToDoubleConverter = NumericConverter<int, double>;
    using FloatToIntConverter  = NumericConverter<float, int>;
    using DoubleToIntConverter = NumericConverter<double, int>;

    /*================================================================================*/

    /**
     * @brief 数值取反转换器
     * @tparam T 数值类型
     */
    template <
        typename T,
        typename std::enable_if<std::is_arithmetic<T>::value, int>::type = 0>
    class NumericNegationConverter : public IValueConverter<T, T>
    {
    public:
        virtual T Convert(T source) override
        {
            return -source;
        }
        virtual T ConvertBack(T target) override
        {
            return -target;
        }
    };

    /*================================================================================*/

    /**
     * @brief 布尔值取反转换器
     */
    class BoolNegationConverter : public IValueConverter<bool, bool>
    {
    public:
        virtual bool Convert(bool source) override
        {
            return !source;
        }
        virtual bool ConvertBack(bool target) override
        {
            return !target;
        }
    };

    /**
     * @brief 数值与布尔值转换器，非零数值转换为true，零数值转换为false
     * @tparam TSource 数值类型
     */
    template <
        typename TSource,
        std::enable_if<std::is_arithmetic<TSource>::value, int>::type = 0>
    class NumericToBoolConverter : public IValueConverter<TSource, bool>
    {
    public:
        virtual bool Convert(TSource source) override
        {
            return source != static_cast<TSource>(0);
        }
        virtual TSource ConvertBack(bool target) override
        {
            return target ? static_cast<TSource>(1) : static_cast<TSource>(0);
        }
    };

    /**
     * @brief 布尔值与数值转换器，true转换为1，false转换为0
     * @tparam TTarget 数值类型
     */
    template <
        typename TTarget,
        std::enable_if<std::is_arithmetic<TTarget>::value, int>::type = 0>
    class BoolToNumericConverter : public IValueConverter<bool, TTarget>
    {
    public:
        virtual TTarget Convert(bool source) override
        {
            return source ? static_cast<TTarget>(1) : static_cast<TTarget>(0);
        }
        virtual bool ConvertBack(TTarget target) override
        {
            return target != static_cast<TTarget>(0);
        }
    };
}
