#pragma once

#include "Reflection.h"

namespace sw
{
    /**
     * @brief 动态对象引用标记类型
     * @note 当Variant内部以BoxedObject<ObjectRef>形式装箱时表示对DynamicObject的引用语义。
     *       Variant的Object()/IsType/DynamicCast/UnsafeCast会自动透视此标记，
     *       使引用对象的访问与值语义一致。一般通过Variant::MakeRef构造，无需直接使用。
     */
    struct ObjectRef final {
        /**
         * @brief 被引用的DynamicObject指针
         */
        DynamicObject *ptr;
    };

    /**
     * @brief 通用变体类型容器，类型擦除地持有任意类型对象
     * @note Variant自身是原生C++值类型，而非DynamicObject派生类。
     *       它可持有任意类型对象 —— 既包括DynamicObject派生类型，也包括原生C++类型。
     *       内部按持入类型与语义自动选择存储策略：
     *       - 值语义 + DynamicObject派生类型：直接以unique_ptr<DynamicObject>持有；
     *       - 值语义 + 其他原生C++类型：装箱为BoxedObject<T>持有；
     *       - 引用语义 + DynamicObject派生类型：装箱为BoxedObject<ObjectRef>持有；
     *       - 引用语义 + 其他原生C++类型：装箱为引用模式的BoxedObject<T>持有；
     *       这些差异对Object()/IsType/DynamicCast/UnsafeCast等查询接口透明。
     * @note 默认为值语义（拷贝/赋值会通过克隆函数指针深拷贝内部对象）；
     *       通过Variant::MakeRef或显式构造ObjectRef可获得引用语义。
     * @note 持入Variant的类型其析构函数不应抛出异常 —— C++11起析构函数默认即为
     *       noexcept，Variant的多个noexcept移动操作依赖此前提。
     */
    class Variant final
    {
    private:
        /**
         * @brief 内部动态对象指针
         */
        std::unique_ptr<DynamicObject> _obj;

        /**
         * @brief 克隆对象函数指针
         */
        DynamicObject *(*_cloner)(const DynamicObject &) = nullptr;

    public:
        /**
         * @brief 默认构造函数，创建一个空的Variant对象
         */
        Variant() = default;

        /**
         * @brief 从nullptr构造一个空的Variant对象
         */
        Variant(std::nullptr_t) noexcept
        {
        }

        /**
         * @brief 以引用语义构造Variant
         * @param ref 引用标记
         * @note 与Variant::MakeRef等价的低层入口，内部以装箱的ObjectRef表示。
         *       使用方需保证被引用对象在Variant存活期间始终有效。
         * @note 若ref.ptr为nullptr则构造为空Variant，行为与Variant(std::nullptr_t)一致。
         */
        Variant(ObjectRef ref)
        {
            if (ref.ptr != nullptr) {
                _obj.reset(new BoxedObject<ObjectRef>(ref));
                ResetCloner<ObjectRef>();
            }
        }

        /**
         * @brief 通用构造函数，接受任意类型的对象（值语义）
         * @warning 若T为基类引用且实际指向派生对象，会发生对象切片；
         *          如需保留对外部对象的引用语义，请改用Variant::MakeRef<T>。
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
         * @throws std::runtime_error 如果对象不可拷贝
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
         * @brief 拷贝赋值运算符
         * @throws std::runtime_error 如果对象不可拷贝
         */
        Variant &operator=(const Variant &other)
        {
            Reset(other);
            return *this;
        }

        /**
         * @brief 移动赋值运算符
         */
        Variant &operator=(Variant &&other) noexcept
        {
            Reset(std::move(other));
            return *this;
        }

        /**
         * @brief 创建一个对外部对象的引用Variant（DynamicObject派生类型重载）
         * @tparam T 被引用的对象类型，必须为DynamicObject的派生类
         * @param obj 被引用的对象
         * @return 一个引用语义的Variant，与obj共享生命周期
         * @note 内部以装箱的ObjectRef表示，Variant的类型查询接口对此透明。
         *       使用方需保证obj在Variant存活期间始终有效。
         */
        template <typename T>
        static auto MakeRef(T &obj)
            -> typename std::enable_if<std::is_base_of<DynamicObject, T>::value, Variant>::type
        {
            Variant v;
            v._obj.reset(new BoxedObject<ObjectRef>(ObjectRef{&obj}));
            v.ResetCloner<ObjectRef>();
            return v;
        }

        /**
         * @brief 创建一个对外部对象的引用Variant（非DynamicObject类型重载）
         * @tparam T 被引用的对象类型
         * @param obj 被引用的对象
         * @return 一个引用语义的Variant，与obj共享生命周期
         * @note 内部以引用模式的BoxedObject<T>表示。
         *       使用方需保证obj在Variant存活期间始终有效。
         */
        template <typename T>
        static auto MakeRef(T &obj)
            -> typename std::enable_if<!std::is_base_of<DynamicObject, T>::value, Variant>::type
        {
            Variant v;
            v._obj.reset(new BoxedObject<T>(BoxedObject<T>::MakeRef(&obj)));
            v.ResetCloner<T>();
            return v;
        }

        /**
         * @brief 布尔转换运算符，判断Variant对象是否为空
         * @return 若Variant对象不为空则返回true，否则返回false
         */
        explicit operator bool() const noexcept
        {
            return _obj != nullptr;
        }

        /**
         * @brief 判断Variant对象是否为空
         * @return 若Variant对象为空则返回true，否则返回false
         */
        bool IsNull() const noexcept
        {
            return _obj == nullptr;
        }

        /**
         * @brief 判断Variant对象是否包含值
         * @return 若Variant对象不为空则返回true，否则返回false
         */
        bool HasValue() const noexcept
        {
            return _obj != nullptr;
        }

        /**
         * @brief 判断两Variant是否引用同一对象（指针身份比较）
         * @return 若两Variant的有效对象指针相同（含都为空）则返回true，否则返回false
         * @note 这是引用相等而非值相等。比较的是Object()返回的指针，因此引用语义的
         *       Variant与持有同一原始对象的Variant会被视为相等。若需值相等比较，
         *       请通过Object()或DynamicCast<T>()获取内部对象后自行比较。
         */
        bool ReferenceEquals(const Variant &other) const noexcept
        {
            return Object() == other.Object();
        }

        /**
         * @brief 重置Variant对象为空
         */
        void Reset() noexcept
        {
            _obj.reset();
            _cloner = nullptr;
        }

        /**
         * @brief 重置Variant对象为空
         */
        void Reset(std::nullptr_t) noexcept
        {
            Reset();
        }

        /**
         * @brief 重置Variant对象为指定的引用标记
         * @param ref 引用标记
         * @note 内部以装箱的ObjectRef表示，使Variant以引用语义持有DynamicObject。
         *       使用方需保证被引用对象在Variant存活期间始终有效。
         * @note 若ref.ptr为nullptr则置为空Variant，行为与Reset(std::nullptr_t)一致。
         */
        void Reset(ObjectRef ref)
        {
            if (ref.ptr == nullptr) {
                Reset();
            } else {
                _obj.reset(new BoxedObject<ObjectRef>(ref));
                ResetCloner<ObjectRef>();
            }
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
         * @return 内部动态对象指针，若Variant为空则返回nullptr
         * @note 若Variant以引用语义持有DynamicObject（即装箱的ObjectRef），
         *       返回被引用对象的指针；否则返回内部对象指针。
         */
        DynamicObject *Object() noexcept
        {
            if (_obj == nullptr) {
                return nullptr;
            }

            ObjectRef *ref = nullptr;

            if (_obj->IsType<ObjectRef>(&ref)) {
                return ref->ptr;
            }
            return _obj.get();
        }

        /**
         * @brief 获取内部动态对象的常量指针
         * @return 内部动态对象的常量指针，若Variant为空则返回nullptr
         * @note 若Variant以引用语义持有DynamicObject（即装箱的ObjectRef），
         *       返回被引用对象的指针；否则返回内部对象指针。
         */
        const DynamicObject *Object() const noexcept
        {
            if (_obj == nullptr) {
                return nullptr;
            }

            const ObjectRef *ref = nullptr;

            if (_obj->IsType<ObjectRef>(&ref)) {
                return ref->ptr;
            }
            return _obj.get();
        }

        /**
         * @brief 判断当前Variant存储的对象是否为指定类型
         * @tparam T 目标类型
         * @param pout 如果不为nullptr，则将转换后的指针赋值给该参数
         * @return 如果Variant对象为指定类型则返回true，否则返回false
         * @note 对引用语义的Variant会自动透视到被引用对象，调用方无需关心存储形式。
         */
        template <typename T>
        bool IsType(T **pout = nullptr)
        {
            DynamicObject *p = Object();

            if (p == nullptr) {
                if (pout != nullptr) {
                    *pout = nullptr;
                }
                return false;
            }
            return p->IsType<T>(pout);
        }

        /**
         * @brief 判断当前Variant存储的对象是否为指定类型
         * @tparam T 目标类型
         * @param pout 如果不为nullptr，则将转换后的指针赋值给该参数
         * @return 如果Variant对象为指定类型则返回true，否则返回false
         * @note 对引用语义的Variant会自动透视到被引用对象，调用方无需关心存储形式。
         */
        template <typename T>
        bool IsType(const T **pout = nullptr) const
        {
            const DynamicObject *p = Object();

            if (p == nullptr) {
                if (pout != nullptr) {
                    *pout = nullptr;
                }
                return false;
            }
            return p->IsType<T>(pout);
        }

        /**
         * @brief 将Variant对象动态转换为指定类型的引用
         * @tparam T 目标类型
         * @return 指定类型的引用
         * @throws std::bad_cast 如果转换失败或Variant为空
         * @note 对引用语义的Variant会自动透视到被引用对象。
         */
        template <typename T>
        T &DynamicCast()
        {
            DynamicObject *p = Object();

            if (p == nullptr) {
                throw std::bad_cast();
            }
            return p->DynamicCast<T>();
        }

        /**
         * @brief 将Variant对象动态转换为指定类型的常量引用
         * @tparam T 目标类型
         * @return 指定类型的常量引用
         * @throws std::bad_cast 如果转换失败或Variant为空
         * @note 对引用语义的Variant会自动透视到被引用对象。
         */
        template <typename T>
        const T &DynamicCast() const
        {
            const DynamicObject *p = Object();

            if (p == nullptr) {
                throw std::bad_cast();
            }
            return p->DynamicCast<T>();
        }

        /**
         * @brief 将Variant对象转换为指定类型的引用
         * @tparam T 目标类型
         * @return 指定类型的引用
         * @throws std::bad_cast 如果Variant为空
         * @note 若目标类型与存储类型不匹配，则行为未定义
         * @note 对引用语义的Variant会自动透视到被引用对象。
         */
        template <typename T>
        T &UnsafeCast()
        {
            DynamicObject *p = Object();

            if (p == nullptr) {
                throw std::bad_cast();
            }
            return p->UnsafeCast<T>();
        }

        /**
         * @brief 将Variant对象转换为指定类型的常量引用
         * @tparam T 目标类型
         * @return 指定类型的常量引用
         * @throws std::bad_cast 如果Variant为空
         * @note 若目标类型与存储类型不匹配，则行为未定义
         * @note 对引用语义的Variant会自动透视到被引用对象。
         */
        template <typename T>
        const T &UnsafeCast() const
        {
            const DynamicObject *p = Object();

            if (p == nullptr) {
                throw std::bad_cast();
            }
            return p->UnsafeCast<T>();
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
         * @note 当T不可拷贝时，克隆函数指针在被调用时抛出std::runtime_error，
         *       而非在编译期失败 —— 以便不可拷贝类型仍可被存放和移动。
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
