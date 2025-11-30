#pragma once

#include "NotificationObject.h"

namespace sw
{
    /**
     * @brief 数据绑定基类
     */
    class BindingBase
    {
    public:
        /**
         * @brief 默认析构函数
         */
        virtual ~BindingBase() = default;

        /**
         * @brief 更新目标属性的值
         */
        virtual void UpdateTarget() = 0;

        /**
         * @brief 更新源属性的值
         */
        virtual void UpdateSource() = 0;
    };
}
