#pragma once

#include "Variant.h"

namespace sw
{
    /**
     * @brief 非模板列表接口，提供基于Variant的元素访问
     */
    class IList
    {
    public:
        /**
         * @brief 虚析构函数
         */
        virtual ~IList() = default;

    public:
        /**
         * @brief 获取列表元素的类型信息
         * @return 元素类型的std::type_index
         */
        virtual std::type_index GetElementType() const noexcept = 0;

        /**
         * @brief 返回列表中的元素数量
         * @return 元素数量
         */
        virtual int Count() const noexcept = 0;

        /**
         * @brief 获取指定索引处元素的Variant引用
         * @param index 元素索引
         * @return 元素的Variant引用
         * @throws std::out_of_range 索引超出范围
         */
        virtual Variant GetVariantAt(int index) = 0;

        /**
         * @brief 获取指定索引处元素的const Variant引用
         * @param index 元素索引
         * @return 元素的const Variant引用
         * @throws std::out_of_range 索引超出范围
         */
        virtual const Variant GetVariantAt(int index) const = 0;

        /**
         * @brief 设置指定索引处的元素值
         * @param index 元素索引
         * @param value 要设置的值
         * @throws std::out_of_range 索引超出范围
         */
        virtual void SetVariantAt(int index, const Variant &value) = 0;

        /**
         * @brief 设置指定索引处的元素值（移动语义）
         * @param index 元素索引
         * @param value 要设置的值
         * @throws std::out_of_range 索引超出范围
         */
        virtual void MoveVariantAt(int index, Variant &value) = 0;
    };

    /**
     * @brief 类型安全的列表接口，继承IList并提供类型化的元素访问
     * @tparam T 列表元素类型
     */
    template <typename T>
    class IListT : public IList
    {
    public:
        /**
         * @brief 虚析构函数
         */
        virtual ~IListT() = default;

        /**
         * @brief 获取列表元素的类型信息
         * @return 元素类型的std::type_index
         */
        virtual std::type_index GetElementType() const noexcept override final
        {
            return typeid(T);
        }

        /**
         * @brief 获取指定索引处元素的Variant引用
         * @param index 元素索引
         * @return 元素的Variant引用
         * @throws std::out_of_range 索引超出范围
         */
        virtual Variant GetVariantAt(int index) override final
        {
            return Variant::MakeRef(GetAt(index));
        }

        /**
         * @brief 获取指定索引处元素的const Variant引用
         * @param index 元素索引
         * @return 元素的const Variant引用
         * @throws std::out_of_range 索引超出范围
         */
        virtual const Variant GetVariantAt(int index) const override final
        {
            return Variant::MakeRef(const_cast<T &>(GetAt(index)));
        }

        /**
         * @brief 设置指定索引处的元素值
         * @param index 元素索引
         * @param value 要设置的值
         * @throws std::out_of_range 索引超出范围
         * @throws std::logic_error T不可拷贝赋值时
         * @throws std::invalid_argument value为null（T不为Variant时）
         * @throws std::bad_cast Variant类型与T不匹配（T不为Variant时）
         */
        virtual void SetVariantAt(int index, const Variant &value) override final
        {
            SetVariantAtImpl(index, value);
        }

        /**
         * @brief 设置指定索引处的元素值（移动语义）
         * @param index 元素索引
         * @param value 要设置的值
         * @throws std::out_of_range 索引超出范围
         * @throws std::logic_error T不可移动赋值时
         * @throws std::invalid_argument value为null（T不为Variant时）
         * @throws std::bad_cast Variant类型与T不匹配（T不为Variant时）
         */
        virtual void MoveVariantAt(int index, Variant &value) override final
        {
            MoveVariantAtImpl(index, value);
        }

    private:
        /**
         * @brief 设置Variant值的实现（T为Variant时，直接赋值）
         * @param index 元素索引
         * @param value 要设置的Variant值
         * @throws std::out_of_range 索引超出范围
         */
        template <typename U = T>
        auto SetVariantAtImpl(int index, const Variant &value)
            -> typename std::enable_if<std::is_same<U, Variant>::value>::type
        {
            SetAt(index, value);
        }

        /**
         * @brief 设置Variant值的实现（T不为Variant时，提取类型并赋值）
         * @param index 元素索引
         * @param value 要设置的Variant值
         * @throws std::out_of_range 索引超出范围
         * @throws std::invalid_argument value为null
         * @throws std::bad_cast Variant类型与T不匹配
         */
        template <typename U = T>
        auto SetVariantAtImpl(int index, const Variant &value)
            -> typename std::enable_if<!std::is_same<U, Variant>::value>::type
        {
            if (value.IsNull()) {
                throw std::invalid_argument(
                    "Cannot set a null Variant to a non-Variant IListT.");
            }
            SetAt(index, value.DynamicCast<T>());
        }

        /**
         * @brief 设置指定索引处的元素值（移动语义）
         * @param index 元素索引
         * @param value 要设置的值
         * @throws std::out_of_range 索引超出范围
         */
        template <typename U = T>
        auto MoveVariantAtImpl(int index, Variant &value)
            -> typename std::enable_if<std::is_same<U, Variant>::value>::type
        {
            SetAt(index, std::move(value));
        }

        /**
         * @brief 设置指定索引处的元素值（移动语义）
         * @param index 元素索引
         * @param value 要设置的值
         * @throws std::out_of_range 索引超出范围
         * @throws std::invalid_argument value为null
         * @throws std::bad_cast Variant类型与T不匹配
         */
        template <typename U = T>
        auto MoveVariantAtImpl(int index, Variant &value)
            -> typename std::enable_if<!std::is_same<U, Variant>::value>::type
        {
            if (value.IsNull()) {
                throw std::invalid_argument(
                    "Cannot set a null Variant to a non-Variant IListT.");
            }
            SetAt(index, std::move(value.DynamicCast<T>()));
        }

    public:
        /**
         * @brief 返回列表中的元素数量
         * @return 元素数量
         */
        virtual int Count() const noexcept = 0;

        /**
         * @brief 获取指定索引处的元素引用
         * @param index 元素索引
         * @return 元素引用
         * @throws std::out_of_range 索引超出范围
         */
        virtual T &GetAt(int index) = 0;

        /**
         * @brief 获取指定索引处的const元素引用
         * @param index 元素索引
         * @return const元素引用
         * @throws std::out_of_range 索引超出范围
         */
        virtual const T &GetAt(int index) const = 0;

        /**
         * @brief 设置指定索引处的元素值
         * @param index 元素索引
         * @param value 要设置的值
         * @throws std::out_of_range 索引超出范围
         * @throws std::logic_error T不可拷贝赋值时
         */
        virtual void SetAt(int index, const T &value) = 0;

        /**
         * @brief 设置指定索引处的元素值（移动语义）
         * @param index 元素索引
         * @param value 要设置的值
         * @throws std::out_of_range 索引超出范围
         * @throws std::logic_error T不可移动赋值时
         */
        virtual void SetAt(int index, T &&value) = 0;
    };
}
