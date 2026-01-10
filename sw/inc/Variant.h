#pragma once

#include "BoxedObject.h"

namespace sw
{
    /**
     * @brief 通用变体类，用于存储任意类型的对象
     */
    class Variant final : public IEqualityComparable<Variant>
    {
    private:
        /**
         * @brief 内部动态对象指针
         */
        std::unique_ptr<DynamicObject> _obj;

        /**
         * @brief 内部克隆函数指针类型
         */
        using TCloner = DynamicObject *(*)(const DynamicObject &);

        /**
         * @brief 内部克隆函数指针
         */
        TCloner _cloner = nullptr;

    public:
        /**
         * @brief 默认构造函数，创建一个空的Variant对象
         */
        Variant() = default;

        /**
         * @brief 通用构造函数，接受任意类型的对象
         */
        template <
            typename T,
            typename std::enable_if<!std::is_same<Variant, typename std::decay<T>::type>::value, int>::type = 0>
        Variant(T &&obj)
        {
            Reset(std::forward<T>(obj));
        }

        /**
         * @brief 拷贝构造函数
         */
        Variant(const Variant &other)
        {
            if (other._obj != nullptr) {
                _obj.reset(other._cloner(*other._obj));
                _cloner = other._cloner;
            }
        }

        /**
         * @brief 移动构造函数
         */
        Variant(Variant &&other) noexcept
            : _obj(std::move(other._obj)), _cloner(other._cloner)
        {
            other._cloner = nullptr;
        }

        /**
         * @brief 拷贝构造函数
         * @throws std::runtime_error 如果对象不可拷贝
         */
        Variant &operator=(const Variant &other)
        {
            Reset(other);
            return *this;
        }

        /**
         * @brief 移动构造函数
         */
        Variant &operator=(Variant &&other) noexcept
        {
            Reset(std::move(other));
            return *this;
        }

        /**
         * @brief 布尔转换运算符，判断Variant对象是否为空
         * @return 若Variant对象不为空则返回true，否则返回false
         */
        operator bool() const noexcept
        {
            return _obj != nullptr;
        }

        /**
         * @brief 判断两Variant是否为同一对象
         */
        bool Equals(const Variant &other) const
        {
            return _obj == other._obj;
        }

        /**
         * @brief 重置Variant对象为空
         */
        void Reset()
        {
            _obj.reset();
            _cloner = nullptr;
        }

        /**
         * @brief 重置Variant对象为另一个Variant对象的值
         * @throws std::runtime_error 如果对象不可拷贝
         */
        void Reset(const Variant &other)
        {
            if (this == &other) {
                return;
            }

            if (other._obj == nullptr) {
                Reset();
            } else {
                _obj.reset(other._cloner(*other._obj));
                _cloner = other._cloner;
            }
        }

        /**
         * @brief 重置Variant对象为另一个Variant对象的值
         */
        void Reset(Variant &&other) noexcept
        {
            if (this != &other) {
                _obj          = std::move(other._obj);
                _cloner       = other._cloner;
                other._cloner = nullptr;
            }
        }

        /**
         * @brief 重置Variant对象为指定类型的对象
         * @tparam T 对象类型
         * @param obj 对象
         */
        template <typename T>
        auto Reset(T &&obj)
            -> typename std::enable_if<
                !std::is_same<Variant, typename std::decay<T>::type>::value &&
                std::is_base_of<DynamicObject, typename std::decay<T>::type>::value>::type
        {
            using U = typename std::decay<T>::type;
            _obj.reset(new U(std::forward<T>(obj)));
            ResetCloner<U>();
        }

        /**
         * @brief 重置Variant对象为指定类型的对象
         * @tparam T 对象类型
         * @param obj 对象
         */
        template <typename T>
        auto Reset(T &&obj)
            -> typename std::enable_if<
                !std::is_same<Variant, typename std::decay<T>::type>::value &&
                !std::is_base_of<DynamicObject, typename std::decay<T>::type>::value>::type
        {
            using U = typename std::decay<T>::type;
            _obj.reset(new BoxedObject<U>(std::forward<T>(obj)));
            ResetCloner<U>();
        }

        /**
         * @brief 获取内部动态对象指针
         * @return 内部动态对象指针
         */
        DynamicObject *Object() const noexcept
        {
            return _obj.get();
        }

    private:
        /**
         * @brief 初始化克隆函数指针
         * @tparam T 对象类型
         */
        template <typename T>
        auto ResetCloner()
            -> typename std::enable_if<std::is_copy_constructible<T>::value && std::is_base_of<DynamicObject, T>::value>::type
        {
            _cloner = [](const DynamicObject &other) -> DynamicObject * {
                return new T(other.UnsafeCast<T>());
            };
        }

        /**
         * @brief 初始化克隆函数指针
         * @tparam T 对象类型
         */
        template <typename T>
        auto ResetCloner()
            -> typename std::enable_if<std::is_copy_constructible<T>::value && !std::is_base_of<DynamicObject, T>::value>::type
        {
            _cloner = [](const DynamicObject &other) -> DynamicObject * {
                return new BoxedObject<T>(other.UnsafeCast<BoxedObject<T>>());
            };
        }

        /**
         * @brief 初始化克隆函数指针
         * @tparam T 对象类型
         */
        template <typename T>
        auto ResetCloner()
            -> typename std::enable_if<!std::is_copy_constructible<T>::value>::type
        {
            _cloner = [](const DynamicObject &other) -> DynamicObject * {
                throw std::runtime_error("Object is not copy constructible.");
            };
        }
    };
}
