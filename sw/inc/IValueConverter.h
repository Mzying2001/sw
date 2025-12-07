#pragma once

#include "Property.h" // For _PropertySetterParamType<T>

namespace sw
{
    /**
     * @brief 最佳参数类型，标量类型使用值传递，复杂类型使用常量引用传递
     */
    template <typename T>
    using _OptimalParamType = _PropertySetterParamType<T>;

    /**
     * @brief 值转换器接口
     * @tparam TSource 源类型
     * @tparam TTarget 目标类型
     */
    template <typename TSource, typename TTarget>
    class IValueConverter
    {
    public:
        /**
         * @brief 源数据传参类型
         */
        using TSourceParam = _OptimalParamType<TSource>;

        /**
         * @brief 目标数据传参类型
         */
        using TTargetParam = _OptimalParamType<TTarget>;

        /**
         * @brief 默认析构函数
         */
        virtual ~IValueConverter() = default;

    public:
        /**
         * @brief 将源类型转换为目标类型
         * @param source 源值
         * @return 转换后的目标值
         */
        virtual TTarget Convert(TSourceParam source) = 0;

        /**
         * @brief 将目标类型转换为源类型
         * @param target 目标值
         * @return 转换后的源值
         */
        virtual TSource ConvertBack(TTargetParam target) = 0;
    };
}
