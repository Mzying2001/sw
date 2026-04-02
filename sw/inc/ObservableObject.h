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
    private:
        /**
         * @brief 属性更改事件委托
         */
        PropertyChangedEventHandler _propertyChanged;

        /**
         * @brief 对象销毁事件委托
         */
        ObjectDeadEventHandler _objectDead;

    public:
        /**
         * @brief 析构时触发对象销毁事件
         */
        virtual ~ObservableObject()
        {
            if (_objectDead) {
                EventArgs args{};
                _objectDead(*this, args);
            }
        }

    protected:
        /**
         * @brief 获取属性更改事件委托的引用
         * @note PropertyChanged事件使用该函数返回的委托来保存事件处理程序
         */
        virtual PropertyChangedEventHandler &GetPropertyChangedEventDelegate() override final
        {
            return _propertyChanged;
        }

        /**
         * @brief 获取对象销毁事件委托的引用
         * @note ObjectDead事件使用该函数返回的委托来保存事件处理程序
         */
        virtual ObjectDeadEventHandler &GetObjectDeadEventDelegate() override final
        {
            return _objectDead;
        }

        /**
         * @brief 触发属性更改通知事件
         * @param propertyId 更改的属性ID
         */
        void RaisePropertyChanged(FieldId propertyId)
        {
            if (_propertyChanged) {
                PropertyChangedEventArgs args{};
                args.propertyId = propertyId;
                _propertyChanged(*this, args);
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
