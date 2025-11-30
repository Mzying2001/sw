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

        /**
         * @brief 获取目标属性的字段ID
         */
        virtual FieldId GetTargetPropertyId() const = 0;

        /**
         * @brief 获取源属性的字段ID
         */
        virtual FieldId GetSourcePropertyId() const = 0;
    };
}
