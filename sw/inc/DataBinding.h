#pragma once

#include "UIElement.h"

namespace sw
{
    /**
     * @brief 数据绑定，用于UI元素与DataContext之间的属性绑定
     */
    class DataBinding final : public BindingBase
    {
    private:
        /**
         * @brief 目标元素
         */
        UIElement *_targetElement;

        /**
         * @brief 内部绑定对象
         */
        std::unique_ptr<Binding> _innerBinding;

    private:
        /**
         * @brief 构造函数
         * @param targetElement 目标元素
         * @param binding 内部绑定对象
         */
        DataBinding(UIElement *targetElement, Binding *binding)
            : _targetElement(targetElement), _innerBinding(binding)
        {
            UpdateDataContextBinding();
            RegisterNotifications();
        }

    public:
        /**
         * @brief 析构函数
         */
        virtual ~DataBinding()
        {
            UnregisterNotifications();
        }

        /**
         * @brief 更新目标属性的值
         * @return 如果更新成功则返回true，否则返回false
         */
        virtual bool UpdateTarget() override
        {
            return _innerBinding->UpdateTarget();
        }

        /**
         * @brief 更新源属性的值
         * @return 如果更新成功则返回true，否则返回false
         */
        virtual bool UpdateSource() override
        {
            return _innerBinding->UpdateSource();
        }

        /**
         * @brief 获取目标属性ID
         */
        virtual FieldId GetTargetPropertyId() const override
        {
            return _innerBinding->GetTargetPropertyId();
        }

        /**
         * @brief 获取源属性ID
         */
        virtual FieldId GetSourcePropertyId() const override
        {
            return _innerBinding->GetSourcePropertyId();
        }

        /**
         * @brief 获取目标元素
         * @return 目标元素指针
         */
        UIElement *GetTargetElement() const
        {
            return _targetElement;
        }

        /**
         * @brief 设置目标元素
         * @param element 目标元素指针
         */
        void SetTargetElement(UIElement *element)
        {
            if (_targetElement != element) {
                UnregisterNotifications();
                _targetElement = element;
                UpdateDataContextBinding();
                RegisterNotifications();
            }
        }

    private:
        /**
         * @brief 注册事件通知
         */
        void RegisterNotifications()
        {
            if (_targetElement != nullptr) {
                _targetElement->ObjectDead +=
                    ObjectDeadEventHandler(*this, &DataBinding::OnTargetElementDead);
                _targetElement->DataContextChanged +=
                    DataContextChangedEventHandler(*this, &DataBinding::OnTargetElementDataContextChanged);
            }
        }

        /**
         * @brief 注销事件通知
         */
        void UnregisterNotifications()
        {
            if (_targetElement != nullptr) {
                _targetElement->ObjectDead -=
                    ObjectDeadEventHandler(*this, &DataBinding::OnTargetElementDead);
                _targetElement->DataContextChanged -=
                    DataContextChangedEventHandler(*this, &DataBinding::OnTargetElementDataContextChanged);
            }
        }

        /**
         * @brief 更新数据上下文绑定
         */
        void UpdateDataContextBinding()
        {
            if (_targetElement == nullptr) {
                _innerBinding->SetBindingObjects(nullptr, nullptr);
            } else {
                _innerBinding->SetBindingObjects(_targetElement, _targetElement->CurrentDataContext);
            }
        }

        /**
         * @brief 目标元素销毁事件处理函数
         */
        void OnTargetElementDead(INotifyObjectDead &sender)
        {
            SetTargetElement(nullptr);
        }

        /**
         * @brief 目标元素数据上下文更改事件处理函数
         */
        void OnTargetElementDataContextChanged(UIElement &sender, DynamicObject *oldval)
        {
            UpdateDataContextBinding();
        }

    public:
        /**
         * @brief 创建数据绑定对象
         * @param targetElement 目标元素
         * @param binding 内部绑定对象
         * @return 绑定对象指针
         * @note 绑定对象不能为nullptr，其生命周期将由DataBinding管理，请勿与其他对象共享
         */
        static DataBinding *Create(UIElement *targetElement, Binding *binding)
        {
            assert(binding != nullptr);
            return new DataBinding(targetElement, binding);
        }

        /**
         * @brief 创建数据绑定对象
         * @param targetProperty 目标属性成员指针
         * @param sourceProperty 源属性成员指针
         * @param mode 绑定模式
         * @param converter 值转换器指针
         * @return 绑定对象指针
         * @note 转换器的生命周期将由绑定对象管理，请勿与其他对象共享
         */
        template <
            typename TTargetObject,
            typename TTargetProperty,
            typename TSourceObject,
            typename TSourceProperty>
        static auto Create(TTargetProperty TTargetObject::*targetProperty,
                           TSourceProperty TSourceObject::*sourceProperty,
                           BindingMode mode,
                           IValueConverter<typename TSourceProperty::TValue, typename TTargetProperty::TValue> *converter = nullptr)
            -> typename std::enable_if<
                _IsProperty<TTargetProperty>::value &&
                    _IsProperty<TSourceProperty>::value &&
                    std::is_base_of<DynamicObject, TTargetObject>::value &&
                    std::is_base_of<DynamicObject, TSourceObject>::value &&
                    std::is_same<typename TTargetProperty::TValue, typename TSourceProperty::TValue>::value,
                DataBinding *>::type
        {
            return new DataBinding(nullptr, Binding::Create(targetProperty, sourceProperty, mode, converter));
        }

        /**
         * @brief 创建数据绑定对象
         * @param targetProperty 目标属性成员指针
         * @param sourceProperty 源属性成员指针
         * @param mode 绑定模式
         * @param converter 值转换器指针
         * @return 绑定对象指针
         * @note 转换器的生命周期将由绑定对象管理，请勿与其他对象共享
         */
        template <
            typename TTargetObject,
            typename TTargetProperty,
            typename TSourceObject,
            typename TSourceProperty>
        static auto Create(TTargetProperty TTargetObject::*targetProperty,
                           TSourceProperty TSourceObject::*sourceProperty,
                           BindingMode mode,
                           IValueConverter<typename TSourceProperty::TValue, typename TTargetProperty::TValue> *converter)
            -> typename std::enable_if<
                _IsProperty<TTargetProperty>::value &&
                    _IsProperty<TSourceProperty>::value &&
                    std::is_base_of<DynamicObject, TTargetObject>::value &&
                    std::is_base_of<DynamicObject, TSourceObject>::value &&
                    !std::is_same<typename TTargetProperty::TValue, typename TSourceProperty::TValue>::value,
                DataBinding *>::type
        {
            return new DataBinding(nullptr, Binding::Create(targetProperty, sourceProperty, mode, converter));
        }
    };
}
