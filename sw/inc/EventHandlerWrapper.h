#pragma once

#include "RoutedEvent.h"

namespace sw
{
    /**
     * @brief 路由事件处理函数包装类，用于需要转换RoutedEventArgs为特定事件参数类型的情况
     */
    template <
        typename TEventArgs,
        typename std::enable_if<std::is_base_of<RoutedEventArgs, TEventArgs>::value, int>::type = 0>
    class RoutedEventHandlerWrapper : public ICallable<void(UIElement &, RoutedEventArgs &)>
    {
    private:
        /**
         * @brief 事件处理函数
         */
        Action<UIElement &, TEventArgs &> _handler;

    public:
        /**
         * @brief         构造函数
         * @param handler 事件处理函数
         */
        RoutedEventHandlerWrapper(const Action<UIElement &, TEventArgs &> &handler)
            : _handler(handler)
        {
        }

        /**
         * @brief 调用事件处理函数
         */
        virtual void Invoke(UIElement &sender, RoutedEventArgs &args) const override
        {
            if (_handler) _handler(sender, static_cast<TEventArgs &>(args));
        }

        /**
         * @brief 克隆当前可调用对象
         */
        virtual ICallable<void(UIElement &, RoutedEventArgs &)> *Clone() const override
        {
            return new RoutedEventHandlerWrapper(_handler);
        }

        /**
         * @brief 获取当前可调用对象的类型信息
         */
        virtual std::type_index GetType() const override
        {
            return typeid(RoutedEventHandlerWrapper<TEventArgs>);
        }

        /**
         * @brief       判断当前可调用对象是否与另一个可调用对象相等
         * @param other 另一个可调用对象
         * @return      如果相等则返回true，否则返回false
         */
        virtual bool Equals(const ICallable<void(UIElement &, RoutedEventArgs &)> &other) const override
        {
            if (this == &other) {
                return true;
            }
            if (GetType() != other.GetType()) {
                return false;
            }
            const auto &otherWrapper = static_cast<const RoutedEventHandlerWrapper &>(other);
            return _handler.Equals(otherWrapper._handler);
        }
    };
}
