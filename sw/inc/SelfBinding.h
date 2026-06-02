#pragma once

#include "Binding.h"

namespace sw
{
    /**
     * @brief 自绑定类，用于同一对象内部属性之间的绑定
     */
    class SelfBinding final : public BindingBase
    {
    private:
        /**
         * @brief 内部绑定对象
         */
        std::unique_ptr<Binding> _innerBinding;

    private:
        /**
         * @brief 构造函数
         * @param binding 内部绑定对象
         */
        SelfBinding(Binding *binding)
            : _innerBinding(binding)
        {
        }

    public:
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
         * @brief 获取绑定模式
         */
        BindingMode GetBindingMode() const
        {
            return _innerBinding->GetBindingMode();
        }

        /**
         * @brief 设置绑定模式
         */
        void SetBindingMode(BindingMode mode)
        {
            _innerBinding->SetBindingMode(mode);
        }

        /**
         * @brief 获取目标对象
         */
        DynamicObject *GetTargetObject() const
        {
            return _innerBinding->GetTargetObject();
        }

        /**
         * @brief 修改目标对象
         */
        void SetTargetObject(DynamicObject *target)
        {
            _innerBinding->SetBindingObjects(target, target);
        }

    public:
        /**
         * @brief 创建自绑定对象
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
                    BindingCastHelper<typename TSourceProperty::TValue, typename TTargetProperty::TValue>::value,
                SelfBinding *>::type
        {
            return new SelfBinding(Binding::Create(targetProperty, sourceProperty, mode, converter));
        }

        /**
         * @brief 创建自绑定对象
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
                    !BindingCastHelper<typename TSourceProperty::TValue, typename TTargetProperty::TValue>::value,
                SelfBinding *>::type
        {
            return new SelfBinding(Binding::Create(targetProperty, sourceProperty, mode, converter));
        }
    };
}
