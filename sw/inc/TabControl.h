#pragma once

#include "Control.h"
#include <CommCtrl.h>

namespace sw
{
    /**
     * @brief TabControl标签的位置
     */
    enum class TabAlignment {
        Top,    // 顶部
        Bottom, // 底部
        Left,   // 左边
        Right   // 右边
    };

    /**
     * @brief 标签页控件
     */
    class TabControl : public Control
    {
    public:
        /**
         * @brief 内容区域位置与尺寸
         */
        const ReadOnlyProperty<sw::Rect> ContentRect;

        /**
         * @brief 当前页面的索引
         */
        const Property<int> SelectedIndex;

        /**
         * @brief 标签的位置
         */
        const Property<TabAlignment> Alignment;

        /**
         * @brief 是否开启多行标签
         */
        const Property<bool> MultiLine;

    public:
        /**
         * @brief 初始化标签页控件
         */
        TabControl();

        /**
         * @brief 获取标签项的数量
         */
        int GetTabCount();

        /**
         * @brief 更新标签项信息
         */
        void UpdateTab();

        /**
         * @brief       更新指定索引处页面项的文本
         * @param index 要更新的索引
         */
        void UpdateTabText(int index);

        /**
         * @brief               测量控件所需尺寸
         * @param availableSize 可用的尺寸
         */
        virtual void Measure(const Size &availableSize) override;

        /**
         * @brief               安排控件位置
         * @param finalPosition 最终控件所安排的位置
         */
        virtual void Arrange(const sw::Rect &finalPosition) override;

    protected:
        /**
         * @brief         添加子元素后调用该函数
         * @param element 添加的子元素
         */
        virtual void OnAddedChild(UIElement &element) override;

        /**
         * @brief         移除子元素后调用该函数
         * @param element 移除的子元素
         */
        virtual void OnRemovedChild(UIElement &element) override;

        /**
         * @brief 父窗口接收到WM_NOTIFY后调用发出通知控件的该函数
         */
        virtual void OnNotified(NMHDR *pNMHDR) override;

        /**
         * @brief SelectedIndex属性更改时调用该函数
         */
        virtual void OnSelectedIndexChanged();

    private:
        /**
         * @brief 根据选中的tab更新子元素的Visible属性
         */
        void UpdateChildVisible();

        /**
         * @brief 发送TCM_INSERTITEMW消息
         */
        int InsertItem(int index, TCITEMW &item);

        /**
         * @brief 发送TCM_SETITEMW消息
         */
        bool SetItem(int index, TCITEMW &item);

        /**
         * @brief 发送TCM_DELETEITEM消息
         */
        bool DeleteItem(int index);

        /**
         * @brief 发送TCM_DELETEALLITEMS消息
         */
        bool DeleteAllItems();
    };
}
