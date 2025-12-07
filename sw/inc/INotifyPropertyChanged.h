#pragma once

#include "Reflection.h"

namespace sw
{
    class INotifyPropertyChanged; // 向前声明

    /**
     * @brief 属性更改事件处理函数类型
     */
    using PropertyChangedEventHandler = Action<INotifyPropertyChanged *, FieldId>;

    /**
     * @brief 属性变更通知接口
     */
    class INotifyPropertyChanged
    {
    public:
        /**
         * @brief 当属性值更改时触发的事件
         */
        PropertyChangedEventHandler PropertyChanged;

        /**
         * @brief 默认析构函数
         */
        virtual ~INotifyPropertyChanged() = default;
    };
}
