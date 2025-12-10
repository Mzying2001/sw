#pragma once

#include "INotifyObjectDead.h"
#include "INotifyPropertyChanged.h"

namespace sw
{
    /**
     * @brief 可观察对象基类，实现属性更改通知功能
     */
    class ObservableObject : public DynamicObject,
                             public INotifyObjectDead,
                             public INotifyPropertyChanged
    {
    protected:
        /**
         * @brief 触发属性更改通知事件
         * @param propertyId 更改的属性ID
         */
        void RaisePropertyChanged(FieldId propertyId)
        {
            if (PropertyChanged) {
                PropertyChanged(*this, propertyId);
            }
        }

        /**
         * @brief 触发属性更改通知事件
         * @tparam T 属性所属类类型
         * @tparam TProperty 属性类型
         * @param property 更改的属性成员指针
         */
        template <typename T, typename TProperty>
        void RaisePropertyChanged(TProperty T::*property)
        {
            FieldId id = Reflection::GetFieldId(property);
            RaisePropertyChanged(id);
        }
    };
}
