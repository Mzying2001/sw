#pragma once

#include "Event.h"
#include "Reflection.h"

namespace sw
{
    class INotifyPropertyChanged; // 向前声明

    /**
     * @brief 属性更改事件参数
     */
    struct PropertyChangedEventArgs : EventArgs {
        /**
         * @brief 发生更改的属性ID
         */
        FieldId propertyId;
    };

    /**
     * @brief 属性更改事件处理函数类型
     */
    using PropertyChangedEventHandler =
        EventHandler<INotifyPropertyChanged, PropertyChangedEventArgs>;

    /**
     * @brief 属性变更通知接口
     */
    class INotifyPropertyChanged
    {
    public:
        /**
         * @brief 当属性值更改时触发的事件
         */
        const Event<PropertyChangedEventHandler> PropertyChanged{
            Event<PropertyChangedEventHandler>::Init(this)
                .Delegate([](INotifyPropertyChanged *self) -> PropertyChangedEventHandler & {
                    return self->GetPropertyChangedEventDelegate();
                }) //
        };

        /**
         * @brief 默认析构函数
         */
        virtual ~INotifyPropertyChanged() = default;

    protected:
        /**
         * @brief 获取属性更改事件委托的引用
         * @note PropertyChanged事件使用该函数返回的委托来保存事件处理程序
         */
        virtual PropertyChangedEventHandler &GetPropertyChangedEventDelegate() = 0;
    };
}
