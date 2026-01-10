#pragma once

#include "Reflection.h"

namespace sw
{
    /**
     * @brief 装箱对象，用于将任意类型封装为DynamicObject对象
     */
    template <typename T>
    class BoxedObject<T, typename std::enable_if<!std::is_base_of<DynamicObject, T>::value>::type> final
        : public DynamicObject
    {
    private:
        /**
         * @brief 指示当前对象是否为引用对象
         */
        bool _isRef;

        /**
         * @brief 内部数据联合体
         */
        union {
            /**
             * @brief 引用指针
             */
            T *refptr;

            /**
             * @brief 对象缓冲区
             */
            alignas(T) uint8_t objbuf[sizeof(T)];
        } _data;

    private:
        /**
         * @brief 内部结构体，用于标识是否为引用参数
         */
        struct _IsRefParam {
            bool val;
        };

        /**
         * @brief 私有构造函数，根据是否为引用参数初始化对象
         * @param isRef 指示是否为引用参数
         */
        explicit BoxedObject(_IsRefParam isRef) noexcept
            : _isRef(isRef.val), _data()
        {
            _isBoxedObject = true;
        }

        /**
         * @brief 释放对象资源
         */
        void Release() noexcept(std::is_nothrow_destructible<T>::value)
        {
            if (!_isRef) {
                Unbox().~T();
            }
        }

    public:
        /**
         * @brief 构造值类型的装箱对象
         * @param args 构造参数列表
         */
        template <typename... Args>
        BoxedObject(Args &&...args)
            : BoxedObject(_IsRefParam{false})
        {
            new (_data.objbuf) T(std::forward<Args>(args)...);
        }

        /**
         * @brief 析构函数
         */
        virtual ~BoxedObject() noexcept(std::is_nothrow_destructible<T>::value)
        {
            Release();
        }

        /**
         * @brief 创建引用类型的装箱对象
         * @param ptr 指向外部对象的指针
         * @return 引用类型的装箱对象
         */
        static BoxedObject<T> MakeRef(T *ptr) noexcept
        {
            BoxedObject<T> result{_IsRefParam{true}};
            result._data.refptr = ptr;
            return result;
        }

        /**
         * @brief 创建引用类型的装箱对象
         * @tparam U 目标类型
         * @param other 另一个装箱对象
         * @return 引用类型的装箱对象
         */
        template <typename U>
        static auto MakeRef(BoxedObject<U> &other) noexcept
            -> typename std::enable_if<_IsStaticCastable<U, T>::value, BoxedObject<T>>::type
        {
            BoxedObject<T> result{_IsRefParam{true}};
            result._data.refptr = other.IsNullRef() ? nullptr : static_cast<T *>(&other.Unbox());
            return result;
        }

        /**
         * @brief 拷贝构造函数
         * @param other 另一个装箱对象
         */
        BoxedObject(const BoxedObject &other) noexcept(std::is_nothrow_copy_constructible<T>::value)
            : BoxedObject(_IsRefParam{other._isRef})
        {
            if (_isRef) {
                _data.refptr = other._data.refptr;
            } else {
                new (_data.objbuf) T(other.Unbox());
            }
        }

        /**
         * @brief 移动构造函数
         * @param other 另一个装箱对象
         */
        BoxedObject(BoxedObject &&other) noexcept(std::is_nothrow_move_constructible<T>::value)
            : BoxedObject(_IsRefParam{other._isRef})
        {
            if (_isRef) {
                _data.refptr       = other._data.refptr;
                other._data.refptr = nullptr;
            } else {
                new (_data.objbuf) T(std::move(other.Unbox()));
            }
        }

        /**
         * @brief 拷贝赋值运算符
         * @param other 另一个装箱对象
         * @return 当前装箱对象的引用
         */
        BoxedObject &operator=(const BoxedObject &other)
        {
            if (this == &other)
                return *this;

            if (_isRef) {
                if (other._isRef) {
                    _data.refptr = other._data.refptr;
                } else {
                    auto oldPtr = _data.refptr;
                    try {
                        new (_data.objbuf) T(other.Unbox());
                        _isRef = false;
                    } catch (...) {
                        _data.refptr = oldPtr;
                        throw;
                    }
                }
            } else {
                if (other._isRef) {
                    Release();
                    _data.refptr = other._data.refptr;
                    _isRef       = true;
                } else {
                    Unbox() = other.Unbox();
                }
            }
            return *this;
        }

        /**
         * @brief 移动赋值运算符
         * @param other 另一个装箱对象
         * @return 当前装箱对象的引用
         */
        BoxedObject &operator=(BoxedObject &&other)
        {
            if (this == &other)
                return *this;

            if (_isRef) {
                if (other._isRef) {
                    _data.refptr       = other._data.refptr;
                    other._data.refptr = nullptr;
                } else {
                    auto oldPtr = _data.refptr;
                    try {
                        new (_data.objbuf) T(std::move(other.Unbox()));
                        _isRef = false;
                    } catch (...) {
                        _data.refptr = oldPtr;
                        throw;
                    }
                }
            } else {
                if (other._isRef) {
                    Release();
                    _data.refptr       = other._data.refptr;
                    _isRef             = true;
                    other._data.refptr = nullptr;
                } else {
                    Unbox() = std::move(other.Unbox());
                }
            }
            return *this;
        }

        /**
         * @brief 判断当前装箱对象是否为引用类型
         * @return 若当前对象为引用类型则返回true，否则返回false
         */
        bool IsRef() const noexcept
        {
            return _isRef;
        }

        /**
         * @brief 判断当前装箱对象是否为空引用
         * @return 若当前对象为引用类型且引用指针为空则返回true，否则返回false
         */
        bool IsNullRef() const noexcept
        {
            return _isRef && (_data.refptr == nullptr);
        }

        /**
         * @brief 判断当前装箱对象是否包含有效值
         * @return 若当前对象包含有效值则返回true，否则返回false
         */
        bool HasValue() const noexcept
        {
            return !IsNullRef();
        }

        /**
         * @brief 获取装箱对象中的值
         * @return 装箱对象中的值的引用
         * @note 仅当装箱对象包含有效值时调用此方法，否则行为未定义
         */
        T &Unbox() noexcept
        {
            assert(HasValue());
            return _isRef ? *_data.refptr : *reinterpret_cast<T *>(_data.objbuf);
        }

        /**
         * @brief 获取装箱对象中的值
         * @return 装箱对象中的值的常量引用
         * @note 仅当装箱对象包含有效值时调用此方法，否则行为未定义
         */
        const T &Unbox() const noexcept
        {
            assert(HasValue());
            return _isRef ? *_data.refptr : *reinterpret_cast<const T *>(_data.objbuf);
        }
    };

    /**
     * @brief 判断对象是否为指定类型
     * @tparam T 目标类型
     * @param pout 如果不为nullptr，则将转换后的指针赋值给该参数
     * @return 如果对象为指定类型则返回true，否则返回false
     */
    template <typename T>
    auto DynamicObject::IsType(T **pout)
        -> typename std::enable_if<!std::is_base_of<DynamicObject, T>::value, bool>::type
    {
#if defined(SW_DISABLE_REFLECTION)
        throw std::runtime_error("Reflection is disabled, cannot check type.");
#else
        if (!_isBoxedObject) {
            if (pout == nullptr) {
                return dynamic_cast<T *>(this) != nullptr;
            } else {
                *pout = dynamic_cast<T *>(this);
                return *pout != nullptr;
            }
        } else {
            BoxedObject<T> *obj = nullptr;

            if (!IsType(&obj) || obj->IsNullRef()) {
                if (pout != nullptr) {
                    *pout = nullptr;
                }
                return false;
            } else {
                if (pout != nullptr) {
                    *pout = &obj->Unbox();
                }
                return true;
            }
        }
#endif
    }

    /**
     * @brief 判断对象是否为指定类型
     * @tparam T 目标类型
     * @param pout 如果不为nullptr，则将转换后的指针赋值给该参数
     * @return 如果对象为指定类型则返回true，否则返回false
     */
    template <typename T>
    auto DynamicObject::IsType(const T **pout) const
        -> typename std::enable_if<!std::is_base_of<DynamicObject, T>::value, bool>::type
    {
#if defined(SW_DISABLE_REFLECTION)
        throw std::runtime_error("Reflection is disabled, cannot check type.");
#else
        if (!_isBoxedObject) {
            if (pout == nullptr) {
                return dynamic_cast<const T *>(this) != nullptr;
            } else {
                *pout = dynamic_cast<const T *>(this);
                return *pout != nullptr;
            }
        } else {
            const BoxedObject<T> *obj = nullptr;

            if (!IsType(&obj) || obj->IsNullRef()) {
                if (pout != nullptr) {
                    *pout = nullptr;
                }
                return false;
            } else {
                if (pout != nullptr) {
                    *pout = &obj->Unbox();
                }
                return true;
            }
        }
#endif
    }

    /**
     * @brief 将对象动态转换为指定类型的引用
     * @tparam T 目标类型
     * @return 指定类型的引用
     * @throws std::bad_cast 如果转换失败
     */
    template <typename T>
    auto DynamicObject::DynamicCast()
        -> typename std::enable_if<!std::is_base_of<DynamicObject, T>::value, T &>::type
    {
#if defined(SW_DISABLE_REFLECTION)
        throw std::runtime_error("Reflection is disabled, cannot perform dynamic cast.");
#else
        if (!_isBoxedObject) {
            return dynamic_cast<T &>(*this);
        } else {
            return dynamic_cast<BoxedObject<T> &>(*this).Unbox();
        }
#endif
    }

    /**
     * @brief 将对象动态转换为指定类型的常量引用
     * @tparam T 目标类型
     * @return 指定类型的常量引用
     * @throws std::bad_cast 如果转换失败
     */
    template <typename T>
    auto DynamicObject::DynamicCast() const
        -> typename std::enable_if<!std::is_base_of<DynamicObject, T>::value, const T &>::type
    {
#if defined(SW_DISABLE_REFLECTION)
        throw std::runtime_error("Reflection is disabled, cannot perform dynamic cast.");
#else
        if (!_isBoxedObject) {
            return dynamic_cast<const T &>(*this);
        } else {
            return dynamic_cast<const BoxedObject<T> &>(*this).Unbox();
        }
#endif
    }

    /**
     * @brief 将对象不安全地转换为指定类型的引用
     * @tparam T 目标类型
     * @return 指定类型的引用
     * @note 若目标类型与当前类型不兼容，则行为未定义
     */
    template <typename T>
    auto DynamicObject::UnsafeCast()
        -> typename std::enable_if<!std::is_base_of<DynamicObject, T>::value && _IsStaticCastable<DynamicObject *, T *>::value, T &>::type
    {
        if (!_isBoxedObject) {
            return static_cast<T &>(*this);
        } else {
            return static_cast<BoxedObject<T> &>(*this).Unbox();
        }
    }

    /**
     * @brief 将对象不安全地转换为指定类型的引用
     * @tparam T 目标类型
     * @return 指定类型的引用
     * @note 若目标类型与当前类型不兼容，则行为未定义
     */
    template <typename T>
    auto DynamicObject::UnsafeCast()
        -> typename std::enable_if<!std::is_base_of<DynamicObject, T>::value && !_IsStaticCastable<DynamicObject *, T *>::value, T &>::type
    {
        if (!_isBoxedObject) {
            return DynamicCast<T>();
        } else {
            return static_cast<BoxedObject<T> &>(*this).Unbox();
        }
    }

    /**
     * @brief 将对象不安全地转换为指定类型的引用
     * @tparam T 目标类型
     * @return 指定类型的引用
     * @note 若目标类型与当前类型不兼容，则行为未定义
     */
    template <typename T>
    auto DynamicObject::UnsafeCast() const
        -> typename std::enable_if<!std::is_base_of<DynamicObject, T>::value && _IsStaticCastable<DynamicObject *, T *>::value, const T &>::type
    {
        if (!_isBoxedObject) {
            return static_cast<const T &>(*this);
        } else {
            return static_cast<const BoxedObject<T> &>(*this).Unbox();
        }
    }

    /**
     * @brief 将对象不安全地转换为指定类型的引用
     * @tparam T 目标类型
     * @return 指定类型的引用
     * @note 若目标类型与当前类型不兼容，则行为未定义
     */
    template <typename T>
    auto DynamicObject::UnsafeCast() const
        -> typename std::enable_if<!std::is_base_of<DynamicObject, T>::value && !_IsStaticCastable<DynamicObject *, T *>::value, const T &>::type
    {
        if (!_isBoxedObject) {
            return DynamicCast<T>();
        } else {
            return static_cast<const BoxedObject<T> &>(*this).Unbox();
        }
    }

    /**
     * @brief 调用成员函数
     * @tparam T 成员函数所属类类型
     * @tparam TFunc 成员函数类型
     * @tparam Args 成员函数参数类型列表
     * @param method 成员函数的委托
     * @param obj 对象引用
     * @param args 成员函数参数列表
     * @return 成员函数返回值
     */
    template <typename T, typename TFunc, typename... Args>
    static auto Reflection::InvokeMethod(const Delegate<TFunc> &method, T &obj, Args &&...args)
        -> typename std::enable_if<!std::is_base_of<DynamicObject, T>::value,
                                   decltype(method(std::declval<DynamicObject &>(), std::forward<Args>(args)...))>::type
    {
        assert(method != nullptr);
        auto boxed = BoxedObject<T>::MakeRef(&obj);
        return method(boxed, std::forward<Args>(args)...);
    }

    /**
     * @brief 访问字段
     * @tparam T 字段所属类类型
     * @tparam TField 字段类型
     * @param accessor 字段访问器的委托
     * @param obj 对象引用
     * @return 字段引用
     */
    template <typename T, typename TField>
    static auto Reflection::AccessField(const Delegate<TField &(DynamicObject &)> &accessor, T &obj)
        -> typename std::enable_if<!std::is_base_of<DynamicObject, T>::value, TField &>::type
    {
        assert(accessor != nullptr);
        auto boxed = BoxedObject<T>::MakeRef(&obj);
        return accessor(boxed);
    }

    /**
     * @brief 获取属性值
     * @tparam T 属性所属类类型
     * @tparam TValue 属性值类型
     * @param getter 属性Getter的委托
     * @param obj 对象引用
     * @return 属性值
     */
    template <typename T, typename TValue>
    static auto Reflection::GetProperty(const Delegate<TValue(DynamicObject &)> &getter, T &obj)
        -> typename std::enable_if<!std::is_base_of<DynamicObject, T>::value, TValue>::type
    {
        assert(getter != nullptr);
        auto boxed = BoxedObject<T>::MakeRef(&obj);
        return getter(boxed);
    }

    /**
     * @brief 设置属性值
     * @tparam T 属性所属类类型
     * @tparam TParam 属性Setter参数类型
     * @tparam TValue 属性值类型
     * @param setter 属性Setter的委托
     * @param obj 对象引用
     * @param value 属性值
     */
    template <typename T, typename TParam, typename TValue>
    static auto Reflection::SetProperty(const Delegate<void(DynamicObject &, TParam)> &setter, T &obj, TValue &&value)
        -> typename std::enable_if<!std::is_base_of<DynamicObject, T>::value>::type
    {
        assert(setter != nullptr);
        auto boxed = BoxedObject<T>::MakeRef(&obj);
        setter(boxed, std::forward<TValue>(value));
    }
}
