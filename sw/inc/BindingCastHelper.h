#pragma once

#include "Variant.h"

namespace sw
{
    /**
     * @brief Binding默认转换辅助模板
     * @tparam TSource 源属性值类型
     * @tparam TTarget 目标属性值类型
     * @note 当Binding未显式指定IValueConverter时，会使用该模板判断是否存在默认转换：
     *       - 若BindingCastHelper<TSource, TTarget>::value为true，则必须提供Convert和ConvertBack函数；
     *       - Convert用于将源属性值转换为目标属性值；
     *       - ConvertBack用于将目标属性值转换回源属性值；
     *       - 若value为false，则对应Binding必须显式提供IValueConverter。
     * @note 可在namespace sw中为自定义类型特化该模板，并继承std::true_type、实现Convert和ConvertBack，
     *       从而扩展Binding的默认转换能力。
     */
    template <typename TSource, typename TTarget, typename = void>
    struct BindingCastHelper : std::false_type {
    };

    /**
     * @brief 相同类型之间的默认转换
     * @tparam TSource 源属性值类型
     * @tparam TTarget 目标属性值类型
     */
    template <typename TSource, typename TTarget>
    struct BindingCastHelper<
        TSource, TTarget,
        typename std::enable_if<
            std::is_same<TSource, TTarget>::value>::type>
        : std::true_type //
    {
        /**
         * @brief 将源属性值转换为目标属性值
         * @param value 源属性值
         * @return 目标属性值
         */
        static TTarget Convert(const TSource &value)
        {
            return value;
        }

        /**
         * @brief 将源属性值移动转换为目标属性值
         * @param value 源属性值
         * @return 目标属性值
         */
        static TTarget Convert(TSource &&value)
        {
            return std::move(value);
        }

        /**
         * @brief 将目标属性值转换回源属性值
         * @param value 目标属性值
         * @return 源属性值
         */
        static TSource ConvertBack(const TTarget &value)
        {
            return value;
        }

        /**
         * @brief 将目标属性值移动转换回源属性值
         * @param value 目标属性值
         * @return 源属性值
         */
        static TSource ConvertBack(TTarget &&value)
        {
            return std::move(value);
        }
    };

    /**
     * @brief 可双向static_cast的类型之间的默认转换
     * @tparam TSource 源属性值类型
     * @tparam TTarget 目标属性值类型
     */
    template <typename TSource, typename TTarget>
    struct BindingCastHelper<
        TSource, TTarget,
        typename std::enable_if<
            !std::is_same<TSource, TTarget>::value &&
            _IsStaticCastable<TSource, TTarget>::value &&
            _IsStaticCastable<TTarget, TSource>::value>::type>
        : std::true_type //
    {
        /**
         * @brief 通过static_cast将源属性值转换为目标属性值
         * @param value 源属性值
         * @return 目标属性值
         */
        static TTarget Convert(const TSource &value)
        {
            return static_cast<TTarget>(value);
        }

        /**
         * @brief 通过static_cast将源属性值移动转换为目标属性值
         * @param value 源属性值
         * @return 目标属性值
         */
        static TTarget Convert(TSource &&value)
        {
            return static_cast<TTarget>(std::move(value));
        }

        /**
         * @brief 通过static_cast将目标属性值转换回源属性值
         * @param value 目标属性值
         * @return 源属性值
         */
        static TSource ConvertBack(const TTarget &value)
        {
            return static_cast<TSource>(value);
        }

        /**
         * @brief 通过static_cast将目标属性值移动转换回源属性值
         * @param value 目标属性值
         * @return 源属性值
         */
        static TSource ConvertBack(TTarget &&value)
        {
            return static_cast<TSource>(std::move(value));
        }
    };

    /**
     * @brief 任意非Variant源类型到Variant目标类型的默认转换
     * @tparam TSource 源属性值类型
     */
    template <typename TSource>
    struct BindingCastHelper<
        TSource, Variant,
        typename std::enable_if<!std::is_same<TSource, Variant>::value>::type> : std::true_type //
    {
        /**
         * @brief 将源属性值装箱为Variant
         * @param value 源属性值
         * @return 保存源属性值副本的Variant
         */
        static Variant Convert(const TSource &value)
        {
            return Variant{value};
        }

        /**
         * @brief 将源属性值移动装箱为Variant
         * @param value 源属性值
         * @return 保存源属性值的Variant
         */
        static Variant Convert(TSource &&value)
        {
            return Variant{std::move(value)};
        }

        /**
         * @brief 从Variant中动态转换回源属性值类型
         * @param value Variant属性值
         * @return 源属性值
         * @throws std::bad_cast 当Variant为空或内部对象无法转换为TSource时抛出
         */
        static TSource ConvertBack(const Variant &value)
        {
            return value.DynamicCast<TSource>();
        }
    };

    /**
     * @brief Variant源类型到任意非Variant目标类型的默认转换
     * @tparam TTarget 目标属性值类型
     */
    template <typename TTarget>
    struct BindingCastHelper<
        Variant, TTarget,
        typename std::enable_if<!std::is_same<TTarget, Variant>::value>::type> : std::true_type //
    {
        /**
         * @brief 从Variant中动态转换为目标属性值类型
         * @param value Variant属性值
         * @return 目标属性值
         * @throws std::bad_cast 当Variant为空或内部对象无法转换为TTarget时抛出
         */
        static TTarget Convert(const Variant &value)
        {
            return value.DynamicCast<TTarget>();
        }

        /**
         * @brief 将目标属性值转换回Variant
         * @param value 目标属性值
         * @return 保存目标属性值副本的Variant
         */
        static Variant ConvertBack(const TTarget &value)
        {
            return Variant{value};
        }

        /**
         * @brief 将目标属性值移动转换回Variant
         * @param value 目标属性值
         * @return 保存目标属性值的Variant
         */
        static Variant ConvertBack(TTarget &&value)
        {
            return Variant{std::move(value)};
        }
    };
}
