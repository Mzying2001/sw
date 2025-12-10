#pragma once

#include "Delegate.h"

namespace sw
{
    class INotifyObjectDead; // 向前声明

    /**
     * @brief 对象销毁事件处理程序类型
     */
    using ObjectDeadEventHandler = Action<INotifyObjectDead &>;

    /**
     * @brief 对象销毁通知接口
     */
    class INotifyObjectDead
    {
    public:
        /**
         * @brief 对象销毁时触发该事件
         */
        ObjectDeadEventHandler ObjectDead;

        /**
         * @brief 析构时触发对象销毁事件
         */
        virtual ~INotifyObjectDead()
        {
            if (ObjectDead) {
                ObjectDead(*this);
            }
        }
    };
}
