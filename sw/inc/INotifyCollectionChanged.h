#pragma once

#include "Event.h"
#include "IList.h"

namespace sw
{
    // 前向声明
    class INotifyCollectionChanged;
    class NotifyCollectionChangedEventArgs;

    /**
     * @brief 集合变更事件处理函数类型
     */
    using NotifyCollectionChangedEventHandler =
        EventHandler<INotifyCollectionChanged, NotifyCollectionChangedEventArgs>;

    /**
     * @brief 描述集合变更的原因
     */
    enum class NotifyCollectionChangedAction {
        Add,     ///< 添加项
        Remove,  ///< 移除项
        Replace, ///< 替换项
        Move,    ///< 移动项
        Reset    ///< 重置集合
    };

    /**
     * @brief 集合变更事件参数类型
     */
    class NotifyCollectionChangedEventArgs : public EventArgs
    {
    public:
        /**
         * @brief 集合变更的原因
         */
        NotifyCollectionChangedAction action =
            NotifyCollectionChangedAction::Reset;

        /**
         * @brief 变更发生的集合对象
         */
        IList *list = nullptr;

        /**
         * @brief 变更发生的项索引
         */
        int index = -1;

        /**
         * @brief 记录移动项的原始索引
         */
        int oldIndex = -1;
    };

    /**
     * @brief 支持集合变更通知的接口
     */
    class INotifyCollectionChanged
    {
    public:
        /**
         * @brief 集合变更事件，当集合发生添加、移除、替换、移动或重置等变更时触发
         */
        Event<NotifyCollectionChangedEventHandler> CollectionChanged{
            Event<NotifyCollectionChangedEventHandler>::Init(this)
                .Delegate([](INotifyCollectionChanged *self) -> NotifyCollectionChangedEventHandler & {
                    return self->GetCollectionChangedEventDelegate();
                }) //
        };

        /**
         * @brief 虚析构函数
         */
        virtual ~INotifyCollectionChanged() = default;

    protected:
        /**
         * @brief 获取集合变更事件委托的引用
         * @note CollectionChanged事件使用该函数返回的委托来保存事件处理程序
         */
        virtual NotifyCollectionChangedEventHandler &GetCollectionChangedEventDelegate() = 0;
    };
}
