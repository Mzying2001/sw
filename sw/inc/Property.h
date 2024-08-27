#pragma once

#include <functional>
#include <iostream>
#include <type_traits>

namespace sw
{
    /**
     * @brief 伪指针，用于实现使用operator->取属性字段
     */
    template <typename T>
    struct FakePtr {
        /**
         * @brief 伪指针所维护的值
         */
        T value;

        /**
         * @brief 构造伪指针
         */
        template <typename... Args>
        FakePtr(Args &&...args)
            : value(std::forward<Args>(args)...)
        {
        }

        /**
         * @brief 取字段
         */
        T *operator->()
        {
            return &value;
        }

        /**
         * @brief 取字段
         */
        const T *operator->() const
        {
            return &value;
        }
    };

    /*================================================================================*/

    /**
     * @brief 属性基类模板
     */
    template <typename T, typename TDerived>
    class PropertyBase
    {
    public:
        // 使用默认构造函数
        PropertyBase() = default;

        // 删除移动构造
        PropertyBase(PropertyBase &&) = delete;

        // 删除拷贝构造
        PropertyBase(const PropertyBase &) = delete;

        // 删除移动赋值
        PropertyBase &operator=(PropertyBase &&) = delete;

        /**
         * @brief 获取属性值，由子类实现
         */
        T GetterImpl() const;

        /**
         * @brief 设置属性值，由子类实现
         */
        void SetterImpl(const T &value) const;

        // /**
        //  * @brief 获取字段，可由子类重写
        //  */
        // FakePtr<T> ListFieldsImpl() const
        // {
        //     return FakePtr<T>(this->Get());
        // }

        /**
         * @brief 获取字段，可由子类重写
         */
        template <typename U = T>
        typename std::enable_if<!std::is_pointer<U>::value, FakePtr<T>>::type ListFieldsImpl() const
        {
            return FakePtr<T>(this->Get());
        }

        /**
         * @brief 获取字段，可由子类重写
         */
        template <typename U = T>
        typename std::enable_if<std::is_pointer<U>::value, T>::type ListFieldsImpl() const
        {
            return this->Get();
        }

        /**
         * @brief 获取属性值
         */
        T Get() const
        {
            return static_cast<const TDerived *>(this)->GetterImpl();
        }

        /**
         * @brief 设置属性值
         */
        void Set(const T &value) const
        {
            static_cast<const TDerived *>(this)->SetterImpl(value);
        }

        /**
         * @brief 取属性字段
         */
        auto operator->() const
        {
            return static_cast<const TDerived *>(this)->ListFieldsImpl();
        }

        /**
         * @brief 获取属性值
         */
        operator T() const
        {
            return this->Get();
        }

        /**
         * @brief 设置属性值
         */
        PropertyBase &operator=(const T &value)
        {
            this->Set(value);
            return *this;
        }

        /**
         * @brief 设置属性值
         */
        const PropertyBase &operator=(const T &value) const
        {
            this->Set(value);
            return *this;
        }

        /**
         * @brief 设置属性值
         */
        PropertyBase &operator=(const PropertyBase &prop)
        {
            this->Set(prop.Get());
            return *this;
        }

        /**
         * @brief 设置属性值
         */
        const PropertyBase &operator=(const PropertyBase &prop) const
        {
            this->Set(prop.Get());
            return *this;
        }

        /**
         * @brief 加赋值运算
         */
        template <typename U = T>
        typename std::enable_if<std::is_arithmetic<U>::value, PropertyBase &>::type operator+=(T value)
        {
            this->Set(this->Get() + value);
            return *this;
        }

        /**
         * @brief 加赋值运算
         */
        template <typename U = T>
        typename std::enable_if<std::is_arithmetic<U>::value, const PropertyBase &>::type operator+=(T value) const
        {
            this->Set(this->Get() + value);
            return *this;
        }

        /**
         * @brief 减赋值运算
         */
        template <typename U = T>
        typename std::enable_if<std::is_arithmetic<U>::value, PropertyBase &>::type operator-=(T value)
        {
            this->Set(this->Get() - value);
            return *this;
        }

        /**
         * @brief 减赋值运算
         */
        template <typename U = T>
        typename std::enable_if<std::is_arithmetic<U>::value, const PropertyBase &>::type operator-=(T value) const
        {
            this->Set(this->Get() - value);
            return *this;
        }

        /**
         * @brief 乘赋值运算
         */
        template <typename U = T>
        typename std::enable_if<std::is_arithmetic<U>::value, PropertyBase &>::type operator*=(T value)
        {
            this->Set(this->Get() * value);
            return *this;
        }

        /**
         * @brief 乘赋值运算
         */
        template <typename U = T>
        typename std::enable_if<std::is_arithmetic<U>::value, const PropertyBase &>::type operator*=(T value) const
        {
            this->Set(this->Get() * value);
            return *this;
        }

        /**
         * @brief 除赋值运算
         */
        template <typename U = T>
        typename std::enable_if<std::is_arithmetic<U>::value, PropertyBase &>::type operator/=(T value)
        {
            this->Set(this->Get() / value);
            return *this;
        }

        /**
         * @brief 除赋值运算
         */
        template <typename U = T>
        typename std::enable_if<std::is_arithmetic<U>::value, const PropertyBase &>::type operator/=(T value) const
        {
            this->Set(this->Get() / value);
            return *this;
        }

        /**
         * @brief 前置自增运算
         */
        template <typename U = T>
        typename std::enable_if<std::is_arithmetic<U>::value, PropertyBase &>::type operator++()
        {
            this->Set(this->Get() + 1);
            return *this;
        }

        /**
         * @brief 前置自增运算
         */
        template <typename U = T>
        typename std::enable_if<std::is_arithmetic<U>::value, const PropertyBase &>::type operator++() const
        {
            this->Set(this->Get() + 1);
            return *this;
        }

        /**
         * @brief 后置自增运算
         */
        template <typename U = T>
        typename std::enable_if<std::is_arithmetic<U>::value, T>::type operator++(int)
        {
            T oldval = this->Get();
            this->Set(oldval + 1);
            return oldval;
        }

        /**
         * @brief 支持Utils::BuildStr
         */
        friend std::wostream &operator<<(std::wostream &wos, const PropertyBase &prop)
        {
            return wos << prop.Get();
        }
    };

    /*================================================================================*/

    /**
     * @brief 属性
     */
    template <typename T>
    class Property : public PropertyBase<T, Property<T>>
    {
    public:
        using TBase = PropertyBase<T, Property<T>>;
        using FnGet = std::function<T()>;
        using FnSet = std::function<void(const T &)>;
        using TBase::operator=;

    private:
        FnGet _getter;
        FnSet _setter;

    public:
        /**
         * @brief 构造属性
         */
        Property(const FnGet &getter, const FnSet &setter)
            : _getter(getter), _setter(setter)
        {
        }

        /**
         * @brief 获取属性值
         */
        T GetterImpl() const
        {
            return this->_getter();
        }

        /**
         * @brief 设置属性值
         */
        void SetterImpl(const T &value) const
        {
            this->_setter(value);
        }

        /**
         * @brief 重设Getter
         */
        void ResetGetter(const FnGet &getter)
        {
            this->_getter = getter;
        }

        /**
         * @brief 重设Setter
         */
        void ResetSetter(const FnSet &setter)
        {
            this->_setter = setter;
        }
    };

    /**
     * @brief 只读属性
     */
    template <typename T>
    class ReadOnlyProperty : public PropertyBase<T, ReadOnlyProperty<T>>
    {
    public:
        using TBase = PropertyBase<T, ReadOnlyProperty<T>>;
        using FnGet = std::function<T()>;

    private:
        FnGet _getter;

    public:
        /**
         * @brief 构造只读属性
         */
        ReadOnlyProperty(const FnGet &getter)
            : _getter(getter)
        {
        }

        /**
         * @brief 获取属性值
         */
        T GetterImpl() const
        {
            return this->_getter();
        }

        /**
         * @brief 重设Getter
         */
        void ResetGetter(const FnGet &getter)
        {
            this->_getter = getter;
        }
    };

    /**
     * @brief 只写属性
     */
    template <typename T>
    class WriteOnlyProperty : public PropertyBase<T, WriteOnlyProperty<T>>
    {
    public:
        using TBase = PropertyBase<T, WriteOnlyProperty<T>>;
        using FnSet = std::function<void(const T &)>;
        using TBase::operator=;

    private:
        FnSet _setter;

    public:
        /**
         * @brief 构造属性
         */
        WriteOnlyProperty(const FnSet &setter)
            : _setter(setter)
        {
        }

        /**
         * @brief 设置属性值
         */
        void SetterImpl(const T &value) const
        {
            this->_setter(value);
        }

        /**
         * @brief 重设Setter
         */
        void ResetSetter(const FnSet &setter)
        {
            this->_setter = setter;
        }
    };

    /*================================================================================*/

    /**
     * @brief 指针属性基类模板
     */
    template <typename T, typename TDerived>
    class PtrPropertyBase : public PropertyBase<T, TDerived>
    {
        static_assert(std::is_pointer<T>::value, "T should be pointer type");

    public:
        using TBase = PropertyBase<T, TDerived>;
        using TBase::PropertyBase;
        using TBase::operator=;

        /**
         * @brief 获取字段
         */
        T ListFieldsImpl() const
        {
            return this->Get();
        }

        /**
         * @brief 属性值是否为nullptr
         */
        bool IsNull() const
        {
            return this->Get() == nullptr;
        }

        /**
         * @brief 解引用
         */
        auto &operator*() const
        {
            return *this->Get();
        }

        /**
         * @brief 解引用
         */
        auto &operator[](int index) const
        {
            return this->Get()[index];
        }
    };

    /**
     * @brief 指针属性
     */
    template <typename T>
    class PtrProperty : public PtrPropertyBase<T, PtrProperty<T>>
    {
    public:
        using TBase = PtrPropertyBase<T, PtrProperty<T>>;
        using FnGet = std::function<T()>;
        using FnSet = std::function<void(T)>;
        using TBase::operator=;

    private:
        FnGet _getter;
        FnSet _setter;

    public:
        /**
         * @brief 构造属性
         */
        PtrProperty(const FnGet &getter, const FnSet &setter)
            : _getter(getter), _setter(setter)
        {
        }

        /**
         * @brief 获取属性值
         */
        T GetterImpl() const
        {
            return this->_getter();
        }

        /**
         * @brief 设置属性值
         */
        void SetterImpl(T value) const
        {
            this->_setter(value);
        }

        /**
         * @brief 重设Getter
         */
        void ResetGetter(const FnGet &getter)
        {
            this->_getter = getter;
        }

        /**
         * @brief 重设Setter
         */
        void ResetSetter(const FnSet &setter)
        {
            this->_setter = setter;
        }
    };

    /**
     * @brief 指针只读属性
     */
    template <typename T>
    class ReadOnlyPtrProperty : public PtrPropertyBase<T, ReadOnlyPtrProperty<T>>
    {
    public:
        using TBase = PtrPropertyBase<T, ReadOnlyPtrProperty<T>>;
        using FnGet = std::function<T()>;

    private:
        FnGet _getter;

    public:
        /**
         * @brief 构造属性
         */
        ReadOnlyPtrProperty(const FnGet &getter)
            : _getter(getter)
        {
        }

        /**
         * @brief 获取属性值
         */
        T GetterImpl() const
        {
            return this->_getter();
        }

        /**
         * @brief 重设Getter
         */
        void ResetGetter(const FnGet &getter)
        {
            this->_getter = getter;
        }
    };

    /**
     * @brief 指针只写属性
     */
    template <typename T>
    class WriteOnlyPtrProperty : public PtrPropertyBase<T, WriteOnlyPtrProperty<T>>
    {
    public:
        using TBase = PtrPropertyBase<T, WriteOnlyPtrProperty<T>>;
        using FnSet = std::function<void(T)>;
        using TBase::operator=;

    private:
        FnSet _setter;

    public:
        /**
         * @brief 构造属性
         */
        WriteOnlyPtrProperty(const FnSet &setter)
            : _setter(setter)
        {
        }

        /**
         * @brief 设置属性值
         */
        void SetterImpl(T value) const
        {
            this->_setter(value);
        }

        /**
         * @brief 重设Setter
         */
        void ResetSetter(const FnSet &setter)
        {
            this->_setter = setter;
        }
    };
}
