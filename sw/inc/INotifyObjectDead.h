#pragma once

#include "Delegate.h"
#include "Event.h"

namespace sw
{
    class INotifyObjectDead; // 向前声明

    /**
     * @brief 对象销毁事件处理程序类型
     */
    using ObjectDeadEventHandler =
        EventHandler<INotifyObjectDead>;

    /**
     * @brief 对象销毁通知接口
     */
    class INotifyObjectDead
    {
    public:
        /**
         * @brief 对象销毁时触发该事件
         */
        const Event<ObjectDeadEventHandler> ObjectDead{
            Event<ObjectDeadEventHandler>::Init(this)
                .Delegate([](INotifyObjectDead *self) -> ObjectDeadEventHandler & {
                    return self->GetObjectDeadEventDelegate();
                }) //
        };

        /**
         * @brief 默认析构函数
         */
        virtual ~INotifyObjectDead() = default;

    protected:
        /**
         * @brief 获取对象销毁事件委托的引用
         * @note 子类应实现该纯虚函数，并在对象销毁时调用该委托
         * @note ObjectDead事件使用该函数返回的委托来保存事件处理程序
         */
        virtual ObjectDeadEventHandler &GetObjectDeadEventDelegate() = 0;
    };
}
