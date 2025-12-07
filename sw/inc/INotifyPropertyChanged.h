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

    protected:
        /**
         * @brief            触发属性更改通知事件
         * @param propertyId 更改的属性ID
         */
        void RaisePropertyChanged(FieldId propertyId) const
        {
            if (PropertyChanged) {
                PropertyChanged(propertyId);
            }
        }

        /**
         * @brief            触发属性更改通知事件
         * @tparam T         属性所属类类型
         * @tparam TProperty 属性类型
         * @param property   更改的属性成员指针
         */
        template <typename T, typename TProperty>
        void RaisePropertyChanged(TProperty T::*property) const
        {
            FieldId id = Reflection::GetFieldId(property);
            RaisePropertyChanged(id);
        }
    };
}
