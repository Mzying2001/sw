#pragma once

#include "Delegate.h"
#include "Internal.h"
#include <cassert>
#include <cstddef>
#include <limits>

namespace sw
{
    // 向前声明
    template <typename>
    class Event;

    /*================================================================================*/

    /**
     * @brief 成员事件初始化器
     */
    template <typename TOwner, typename TDelegate>
    class MemberEventInitializer
    {
        template <typename>
        friend class Event;

    private:
        /**
         * @brief 事件所有者
         */
        TOwner *_owner;

        /**
         * @brief 委托访问器函数指针
         */
        TDelegate &(*_accessor)(TOwner *);

    public:
        /**
         * @brief 构造成员事件初始化器
         */
        explicit MemberEventInitializer(TOwner *owner)
            : _owner(owner), _accessor(nullptr)
        {
        }

        /**
         * @brief 设置委托访问器
         */
        MemberEventInitializer &Delegate(TDelegate &(*accessor)(TOwner *))
        {
            _accessor = accessor;
            return *this;
        }

        /**
         * @brief 使用成员函数获取委托
         */
        template <TDelegate (TOwner::*accessor)()>
        MemberEventInitializer &Delegate()
        {
            return Delegate([](TOwner *owner) -> TDelegate & {
                return (owner->*accessor)();
            });
        }

        /**
         * @brief 使用成员函数获取委托
         */
        template <TDelegate (TOwner::*accessor)() const>
        MemberEventInitializer &Delegate()
        {
            return Delegate([](TOwner *owner) -> TDelegate & {
                return (owner->*accessor)();
            });
        }

        /**
         * @brief 使用指定成员字段委托
         */
        template <TDelegate TOwner::*field>
        MemberEventInitializer &Delegate()
        {
            return Delegate([](TOwner *owner) -> TDelegate & {
                return owner->*field;
            });
        }
    };

    /**
     * @brief 静态事件初始化器
     */
    template <typename TDelegate>
    class StaticEventInitializer
    {
        template <typename>
        friend class Event;

    private:
        /**
         * @brief 委托访问器函数指针
         */
        TDelegate &(*_accessor)();

    public:
        /**
         * @brief 构造静态事件初始化器
         */
        StaticEventInitializer()
            : _accessor(nullptr)
        {
        }

        /**
         * @brief 设置委托访问器
         */
        StaticEventInitializer &Delegate(TDelegate &(*accessor)())
        {
            _accessor = accessor;
            return *this;
        }
    };

    /*================================================================================*/

    /**
     * @brief 事件类
     */
    template <typename TRet, typename... Args>
    class Event<Delegate<TRet(Args...)>> final
    {
    public:
        /**
         * @brief 事件的委托类型
         */
        using TDelegate = Delegate<TRet(Args...)>;

        /**
         * @brief 当前事件类型的类型别名
         */
        using TEvent = Event<TDelegate>;

    public:
        /**
         * @brief 构造成员事件
         * @param initializer 成员事件初始化器
         */
        template <typename TOwner>
        explicit Event(const MemberEventInitializer<TOwner, TDelegate> &initializer)
        {
            assert(initializer._owner != nullptr);
            assert(initializer._accessor != nullptr);

            SetOwner(initializer._owner);
            _accessor = reinterpret_cast<void *>(initializer._accessor);

            _extractor = [](void *owner, void *accessor) -> TDelegate & {
                return reinterpret_cast<TDelegate &(*)(TOwner *)>(accessor)(reinterpret_cast<TOwner *>(owner));
            };
        }

        /**
         * @brief 构造静态事件
         * @param initializer 静态事件初始化器
         */
        explicit Event(const StaticEventInitializer<TDelegate> &initializer)
        {
            assert(initializer._accessor != nullptr);

            SetOwner(nullptr);
            _accessor = reinterpret_cast<void *>(initializer._accessor);

            _extractor = [](void * /*owner*/, void *accessor) -> TDelegate & {
                return reinterpret_cast<TDelegate &(*)()>(accessor)();
            };
        }

        /**
         * @brief 添加事件处理程序
         * @param handler 事件处理程序，可以是任意可调用对象
         */
        template <typename T>
        auto operator+=(T &&handler) const
            -> typename std::enable_if<_DelegateCanAddSubtract<TDelegate, T>::value>::type
        {
            this->GetDelegate() += std::forward<T>(handler);
        }

        /**
         * @brief 移除事件处理程序
         * @param handler 事件处理程序，可以是任意可调用对象
         */
        template <typename T>
        auto operator-=(T &&handler) const
            -> typename std::enable_if<_DelegateCanAddSubtract<TDelegate, T>::value>::type
        {
            this->GetDelegate() -= std::forward<T>(handler);
        }

    public:
        /**
         * @brief 初始化成员事件
         */
        template <typename TOwner>
        static MemberEventInitializer<TOwner, TDelegate> Init(TOwner *owner)
        {
            return MemberEventInitializer<TOwner, TDelegate>(owner);
        }

        /**
         * @brief 初始化静态事件
         */
        static StaticEventInitializer<TDelegate> Init()
        {
            return StaticEventInitializer<TDelegate>();
        }

    private:
        /**
         * @brief 静态偏移量，表示静态事件
         */
        static constexpr std::ptrdiff_t _STATICOFFSET =
            (std::numeric_limits<std::ptrdiff_t>::max)();

        /**
         * @brief 事件所有者对象相对于当前事件对象的偏移量
         */
        std::ptrdiff_t _offset;

        /**
         * @brief 委托访问器指针
         */
        void *_accessor;

        /**
         * @brief 类型擦除的委托访问器，用于获取事件的委托引用
         */
        TDelegate &(*_extractor)(void *owner, void *accessor);

        /**
         * @brief 判断事件是否为静态事件
         */
        bool IsStatic() const noexcept
        {
            return _offset == _STATICOFFSET;
        }

        /**
         * @brief 设置事件所有者对象
         * @param owner 事件所有者对象指针，nullptr表示静态事件
         */
        void SetOwner(void *owner) noexcept
        {
            if (owner == nullptr) {
                _offset = _STATICOFFSET;
            } else {
                _offset = reinterpret_cast<uint8_t *>(owner) - reinterpret_cast<uint8_t *>(this);
            }
        }

        /**
         * @brief 获取事件所有者对象，当事件为静态事件时返回nullptr
         */
        void *GetOwner() const noexcept
        {
            if (IsStatic()) {
                return nullptr;
            } else {
                return const_cast<uint8_t *>(reinterpret_cast<const uint8_t *>(this)) + _offset;
            }
        }

        /**
         * @brief 获取事件的委托引用
         */
        TDelegate &GetDelegate() const
        {
            return _extractor(GetOwner(), _accessor);
        }
    };

    /*================================================================================*/

    /**
     * @brief 事件参数结构体
     */
    struct EventArgs {};

    /**
     * @brief 事件处理函数类型别名
     */
    template <typename TSender, typename TEventArgs = EventArgs>
    using EventHandler = Delegate<void(TSender &, TEventArgs &)>;
}
