#pragma once

#include "Reflection.h"

namespace sw
{
    /**
     * @brief 属性变更通知接口
     */
    class INotifyPropertyChanged
    {
    public:
        /**
         * @brief 当属性值更改时触发的事件
         */
        Action<FieldId> PropertyChanged;

        /**
         * @brief 默认析构函数
         */
        virtual ~INotifyPropertyChanged() = default;
    };
}
