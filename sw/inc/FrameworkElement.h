#pragma once

#include "Binding.h"
#include "ObservableObject.h"
#include <unordered_map>

namespace sw
{
    // 前向声明
    class DataBinding;
    class FrameworkElement;

    /**
     * @brief 数据上下文更改事件参数
     */
    struct DataContextChangedEventArgs : EventArgs {
        /**
         * @brief 旧的数据上下文值
         */
        DynamicObject *oldDataContext;
    };

    /**
     * @brief 数据上下文更改事件处理函数类型
     */
    using DataContextChangedEventHandler =
        EventHandler<FrameworkElement, DataContextChangedEventArgs>;

    /**
     * @brief 框架元素类，提供数据上下文和绑定功能
     */
    class FrameworkElement : public ObservableObject
    {
    private:
        /**
         * @brief 属性的绑定信息
         */
        std::unordered_map<FieldId, std::unique_ptr<BindingBase>> _bindings{};

        /**
         * @brief 数据上下文
         */
        DynamicObject *_dataContext = nullptr;

        /**
         * @brief 数据上下文改变事件委托
         */
        DataContextChangedEventHandler _dataContextChanged;

    public:
        /**
         * @brief 数据上下文改变时触发该事件
         */
        const Event<DataContextChangedEventHandler> DataContextChanged;

        /**
         * @brief 数据上下文
         */
        const Property<DynamicObject *> DataContext;

        /**
         * @brief 当前元素的有效数据上下文
         * @note 若当前元素的DataContext不为nullptr则返回该值，否则递归获取父元素的DataContext
         */
        const ReadOnlyProperty<DynamicObject *> CurrentDataContext;

    protected:
        /**
         * @brief 初始化FrameworkElement
         */
        FrameworkElement();

        // 删除拷贝构造函数
        FrameworkElement(const FrameworkElement &) = delete;

        // 删除移动构造函数
        FrameworkElement(FrameworkElement &&) = delete;

        // 删除拷贝赋值运算符
        FrameworkElement &operator=(const FrameworkElement &) = delete;

        // 删除移动赋值运算符
        FrameworkElement &operator=(FrameworkElement &&) = delete;

    public:
        /**
         * @brief 添加绑定对象
         * @return 若函数成功则返回true，否则返回false
         * @note 绑定对象的生命周期将由当前元素管理，请勿与其他对象共享
         * @note 请确保绑定对象的目标属性为当前元素的属性，该函数内部不会对此进行检查
         * @note 同一个属性只能设置一个绑定，若该属性已存在绑定则会被新的绑定覆盖
         */
        bool AddBinding(BindingBase *binding);

        /**
         * @brief 添加绑定对象
         * @return 若函数成功则返回true，否则返回false
         * @note 绑定对象的生命周期将由当前元素管理，请勿与其他对象共享
         * @note 该函数会将绑定的目标对象设置为当前元素，若未指定源对象则会将DataContext作为源对象
         * @note 同一个属性只能设置一个绑定，若该属性已存在绑定则会被新的绑定覆盖
         */
        bool AddBinding(Binding *binding);

        /**
         * @brief 添加绑定到DataContext的绑定对象
         * @return 若函数成功则返回true，否则返回false
         * @note 绑定对象的生命周期将由当前元素管理，请勿与其他对象共享
         * @note 同一个属性只能设置一个绑定，若该属性已存在绑定则会被新的绑定覆盖
         */
        bool AddBinding(DataBinding *binding);

        /**
         * @brief 移除指定属性的绑定对象
         * @return 若函数成功则返回true，否则返回false
         */
        bool RemoveBinding(FieldId propertyId);

        /**
         * @brief  移除指定属性的绑定对象
         * @return 若函数成功则返回true，否则返回false
         */
        template <typename T, typename TProperty>
        bool RemoveBinding(TProperty T::*prop)
        { return RemoveBinding(Reflection::GetFieldId(prop)); }

    protected:
        /**
         * @brief 当CurrentDataContext更改时调用此函数
         * @param oldDataContext 旧的数据上下文值
         */
        virtual void OnCurrentDataContextChanged(DynamicObject *oldDataContext);

    public:
        /**
         * @brief 获取父元素
         * @return 父元素指针，如果没有父元素则返回nullptr
         */
        virtual FrameworkElement *GetParent() const = 0;

        /**
         * @brief 获取子元素数量
         * @return 子元素数量
         */
        virtual int GetChildCount() const = 0;

        /**
         * @brief 获取指定索引处的子元素
         * @param index 子元素索引
         * @throw std::out_of_range 如果索引超出范围
         */
        virtual FrameworkElement &GetChildAt(int index) const = 0;
    };
}
