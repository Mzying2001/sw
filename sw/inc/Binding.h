#pragma once

#include "INotifyPropertyChanged.h"
#include "IValueConverter.h"

namespace sw
{
    /**
     * @brief 绑定模式枚举
     */
    enum class BindingMode {
        /**
         * @brief 一次性绑定，在绑定创建时更新目标属性值
         */
        OneTime,

        /**
         * @brief 单向，从源到目标
         */
        OneWay,

        /**
         * @brief 单向，从目标到源
         */
        OneWayToSource,

        /**
         * @brief 双向，源和目标相互更新
         */
        TwoWay,
    };

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

    public:
        /**
         * @brief 更新目标属性的值
         * @return 如果更新成功则返回true，否则返回false
         */
        virtual bool UpdateTarget() = 0;

        /**
         * @brief 更新源属性的值
         * @return 如果更新成功则返回true，否则返回false
         */
        virtual bool UpdateSource() = 0;
    };

    /**
     * @brief 数据绑定类
     */
    class Binding : public BindingBase
    {
    private:
        /**
         * @brief 绑定模式
         */
        BindingMode _mode;

        /**
         * @brief 目标对象
         */
        DynamicObject *_targetObject;

        /**
         * @brief 源对象
         */
        DynamicObject *_sourceObject;

        /**
         * @brief 目标属性ID
         */
        FieldId _targetPropertyId;

        /**
         * @brief 源属性ID
         */
        FieldId _sourcePropertyId;

        /**
         * @brief 值转换器
         */
        void *_converter;

        /**
         * @brief 值转换器删除函数
         */
        void (*_converterDeleter)(void *);

        /**
         * @brief 更新目标属性值委托
         */
        Func<Binding *, bool> _updateTargetFunc;

        /**
         * @brief 更新源属性值委托
         */
        Func<Binding *, bool> _updateSourceFunc;

    private:
        /**
         * @brief 默认构造函数
         */
        Binding() = default;

        Binding(const Binding &)            = delete; // 删除拷贝构造函数
        Binding(Binding &&)                 = delete; // 删除移动构造函数
        Binding &operator=(const Binding &) = delete; // 删除拷贝赋值运算符
        Binding &operator=(Binding &&)      = delete; // 删除移动赋值运算符

    public:
        /**
         * @brief 析构函数
         */
        virtual ~Binding()
        {
            UnregisterNotifications();

            if (_converter && _converterDeleter) {
                _converterDeleter(_converter);
            }
        }

        /**
         * @brief 更新目标属性的值
         * @return 如果更新成功则返回true，否则返回false
         */
        virtual bool UpdateTarget() override
        {
            bool result = false;

            if (_updateTargetFunc) {
                result = _updateTargetFunc(this);
            }
            return result;
        }

        /**
         * @brief 更新源属性的值
         * @return 如果更新成功则返回true，否则返回false
         */
        virtual bool UpdateSource() override
        {
            bool result = false;

            if (_updateSourceFunc) {
                result = _updateSourceFunc(this);
            }
            return result;
        }

        /**
         * @brief 获取绑定模式
         */
        BindingMode GetBindingMode() const
        {
            return _mode;
        }

        /**
         * @brief 获取目标对象
         */
        DynamicObject *GetTargetObject() const
        {
            return _targetObject;
        }

        /**
         * @brief 获取源对象
         */
        DynamicObject *GetSourceObject() const
        {
            return _sourceObject;
        }

        /**
         * @brief 修改绑定模式
         */
        Binding &SetBindingMode(BindingMode mode)
        {
            if (_mode != mode) {
                _mode = mode;
                OnBindingChanged();
            }
            return *this;
        }

        /**
         * @brief 修改目标对象
         */
        Binding &SetTargetObject(DynamicObject *target)
        {
            assert(target != nullptr);

            if (_targetObject != target) {
                UnregisterNotifications();
                _targetObject = target;
                RegisterNotifications();
                OnBindingChanged();
            }
            return *this;
        }

        /**
         * @brief 修改源对象
         */
        Binding &SetSourceObject(DynamicObject *source)
        {
            assert(source != nullptr);

            if (_sourceObject != source) {
                UnregisterNotifications();
                _sourceObject = source;
                RegisterNotifications();
                OnBindingChanged();
            }
            return *this;
        }

        /**
         * @brief 修改目标对象和源对象
         */
        Binding &SetBindingObjects(DynamicObject *target, DynamicObject *source)
        {
            assert(target != nullptr);
            assert(source != nullptr);

            if (_targetObject != target || _sourceObject != source) {
                UnregisterNotifications();
                _targetObject = target;
                _sourceObject = source;
                RegisterNotifications();
                OnBindingChanged();
            }
            return *this;
        }

    private:
        /**
         * @brief 注册属性更改通知
         */
        void RegisterNotifications()
        {
            INotifyPropertyChanged *targetNotifObj = nullptr;
            INotifyPropertyChanged *sourceNotifObj = nullptr;

            if (_targetObject->IsType<INotifyPropertyChanged>(&targetNotifObj)) {
                targetNotifObj->PropertyChanged +=
                    Action<FieldId>(*this, &Binding::OnTargetPropertyChanged);
            }

            if (_sourceObject->IsType<INotifyPropertyChanged>(&sourceNotifObj)) {
                sourceNotifObj->PropertyChanged +=
                    Action<FieldId>(*this, &Binding::OnSourcePropertyChanged);
            }
        }

        /**
         * @brief 注销属性更改通知
         */
        void UnregisterNotifications()
        {
            INotifyPropertyChanged *targetNotifObj = nullptr;
            INotifyPropertyChanged *sourceNotifObj = nullptr;

            if (_targetObject->IsType<INotifyPropertyChanged>(&targetNotifObj)) {
                targetNotifObj->PropertyChanged -=
                    Action<FieldId>(*this, &Binding::OnTargetPropertyChanged);
            }

            if (_sourceObject->IsType<INotifyPropertyChanged>(&sourceNotifObj)) {
                sourceNotifObj->PropertyChanged -=
                    Action<FieldId>(*this, &Binding::OnSourcePropertyChanged);
            }
        }

        /**
         * @brief 目标属性更改处理函数
         */
        void OnTargetPropertyChanged(FieldId propertyId)
        {
            if (propertyId != _targetPropertyId) {
                return;
            }

            if (_mode == BindingMode::TwoWay ||
                _mode == BindingMode::OneWayToSource) {
                UpdateSource();
            }
        }

        /**
         * @brief 源属性更改处理函数
         */
        void OnSourcePropertyChanged(FieldId propertyId)
        {
            if (propertyId != _sourcePropertyId) {
                return;
            }

            if (_mode == BindingMode::TwoWay ||
                _mode == BindingMode::OneWay) {
                UpdateTarget();
            }
        }

        /**
         * @brief 绑定创建和发生更改时调用
         */
        void OnBindingChanged()
        {
            switch (_mode) {
                case BindingMode::OneTime:
                case BindingMode::OneWay:
                case BindingMode::TwoWay: {
                    UpdateTarget();
                    break;
                }
                case BindingMode::OneWayToSource: {
                    UpdateSource();
                    break;
                }
            }
        }

    public:
        /**
         * @brief 创建绑定对象
         * @param target 目标对象指针
         * @param targetProperty 目标属性成员指针
         * @param source 源对象指针
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
        static auto Create(DynamicObject *target, TTargetProperty TTargetObject::*targetProperty,
                           DynamicObject *source, TSourceProperty TSourceObject::*sourceProperty,
                           BindingMode mode,
                           IValueConverter<typename TSourceProperty::TValue, typename TTargetProperty::TValue> *converter = nullptr)
            -> typename std::enable_if<
                _IsProperty<TTargetProperty>::value &&
                    _IsProperty<TSourceProperty>::value &&
                    std::is_base_of<DynamicObject, TTargetObject>::value &&
                    std::is_base_of<DynamicObject, TSourceObject>::value &&
                    std::is_same<typename TTargetProperty::TValue, typename TSourceProperty::TValue>::value,
                Binding *>::type
        {
            using TTargetValue = typename TTargetProperty::TValue;
            using TSourceValue = typename TSourceProperty::TValue;

            assert(target != nullptr);
            assert(source != nullptr);

            auto binding   = new Binding;
            binding->_mode = mode;

            binding->_targetObject     = target;
            binding->_sourceObject     = source;
            binding->_targetPropertyId = Reflection::GetFieldId(targetProperty);
            binding->_sourcePropertyId = Reflection::GetFieldId(sourceProperty);

            binding->_converter        = converter;
            binding->_converterDeleter = [](void *ptr) {
                delete reinterpret_cast<IValueConverter<TSourceValue, TTargetValue> *>(ptr);
            };

            // update target action
            binding->_updateTargetFunc = [targetSetter = Reflection::GetPropertySetter(targetProperty),
                                          sourceGetter = Reflection::GetPropertyGetter(sourceProperty)](Binding *binding) -> bool //
            {
                IValueConverter<TSourceValue, TTargetValue> *converter =
                    reinterpret_cast<IValueConverter<TSourceValue, TTargetValue> *>(binding->_converter);

                if (targetSetter == nullptr ||
                    sourceGetter == nullptr ||
                    binding->_targetObject == nullptr ||
                    binding->_sourceObject == nullptr) {
                    return false;
                }

                if (converter) {
                    targetSetter(
                        *binding->_targetObject,
                        converter->Convert(sourceGetter(*binding->_sourceObject)));
                } else {
                    targetSetter(
                        *binding->_targetObject,
                        sourceGetter(*binding->_sourceObject));
                }
                return true;
            };

            // update source action
            binding->_updateSourceFunc = [targetGetter = Reflection::GetPropertyGetter(targetProperty),
                                          sourceSetter = Reflection::GetPropertySetter(sourceProperty)](Binding *binding) -> bool //
            {
                IValueConverter<TSourceValue, TTargetValue> *converter =
                    reinterpret_cast<IValueConverter<TSourceValue, TTargetValue> *>(binding->_converter);

                if (targetGetter == nullptr ||
                    sourceSetter == nullptr ||
                    binding->_targetObject == nullptr ||
                    binding->_sourceObject == nullptr) {
                    return false;
                }

                if (converter) {
                    sourceSetter(
                        *binding->_sourceObject,
                        converter->ConvertBack(targetGetter(*binding->_targetObject)));
                } else {
                    sourceSetter(
                        *binding->_sourceObject,
                        targetGetter(*binding->_targetObject));
                }
                return true;
            };

            binding->RegisterNotifications();
            binding->OnBindingChanged();
            return binding;
        }

        /**
         * @brief 创建绑定对象
         * @param target 目标对象指针
         * @param targetProperty 目标属性成员指针
         * @param source 源对象指针
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
        static auto Create(DynamicObject *target, TTargetProperty TTargetObject::*targetProperty,
                           DynamicObject *source, TSourceProperty TSourceObject::*sourceProperty,
                           BindingMode mode,
                           IValueConverter<typename TSourceProperty::TValue, typename TTargetProperty::TValue> *converter)
            -> typename std::enable_if<
                _IsProperty<TTargetProperty>::value &&
                    _IsProperty<TSourceProperty>::value &&
                    std::is_base_of<DynamicObject, TTargetObject>::value &&
                    std::is_base_of<DynamicObject, TSourceObject>::value &&
                    !std::is_same<typename TTargetProperty::TValue, typename TSourceProperty::TValue>::value,
                Binding *>::type
        {
            using TTargetValue = typename TTargetProperty::TValue;
            using TSourceValue = typename TSourceProperty::TValue;

            assert(target != nullptr);
            assert(source != nullptr);
            assert(converter != nullptr);

            auto binding   = new Binding;
            binding->_mode = mode;

            binding->_targetObject     = target;
            binding->_sourceObject     = source;
            binding->_targetPropertyId = Reflection::GetFieldId(targetProperty);
            binding->_sourcePropertyId = Reflection::GetFieldId(sourceProperty);

            binding->_converter        = converter;
            binding->_converterDeleter = [](void *ptr) {
                delete reinterpret_cast<IValueConverter<TSourceValue, TTargetValue> *>(ptr);
            };

            // update target action
            binding->_updateTargetFunc = [targetSetter = Reflection::GetPropertySetter(targetProperty),
                                          sourceGetter = Reflection::GetPropertyGetter(sourceProperty)](Binding *binding) -> bool //
            {
                IValueConverter<TSourceValue, TTargetValue> *converter =
                    reinterpret_cast<IValueConverter<TSourceValue, TTargetValue> *>(binding->_converter);

                if (targetSetter == nullptr ||
                    sourceGetter == nullptr ||
                    converter == nullptr ||
                    binding->_targetObject == nullptr ||
                    binding->_sourceObject == nullptr) {
                    return false;
                }

                targetSetter(
                    *binding->_targetObject,
                    converter->Convert(sourceGetter(*binding->_sourceObject)));
                return true;
            };

            // update source action
            binding->_updateSourceFunc = [targetGetter = Reflection::GetPropertyGetter(targetProperty),
                                          sourceSetter = Reflection::GetPropertySetter(sourceProperty)](Binding *binding) -> bool //
            {
                IValueConverter<TSourceValue, TTargetValue> *converter =
                    reinterpret_cast<IValueConverter<TSourceValue, TTargetValue> *>(binding->_converter);

                if (targetGetter == nullptr ||
                    sourceSetter == nullptr ||
                    converter == nullptr ||
                    binding->_targetObject == nullptr ||
                    binding->_sourceObject == nullptr) {
                    return false;
                }

                sourceSetter(
                    *binding->_sourceObject,
                    converter->ConvertBack(targetGetter(*binding->_targetObject)));
                return true;
            };

            binding->RegisterNotifications();
            binding->OnBindingChanged();
            return binding;
        }
    };
}
