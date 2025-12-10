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
}
