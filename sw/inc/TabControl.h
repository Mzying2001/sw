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
    private:
        /**
         * @brief 是否自动调整大小
         */
        bool _autoSize = true;

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

        /**
         * @brief 是否自动调整大小以适应当前页面内容，默认为true
         */
        const Property<bool> AutoSize;

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
         * @brief               测量元素所需尺寸，无需考虑边框和边距
         * @param availableSize 可用的尺寸
         * @return              返回元素需要占用的尺寸
         */
        virtual Size MeasureOverride(const Size &availableSize) override;

        /**
         * @brief           安排子元素的位置，可重写该函数以实现自定义布局
         * @param finalSize 可用于排列子元素的最终尺寸
         */
        virtual void ArrangeOverride(const Size &finalSize) override;

        /**
         * @brief        父窗口接收到WM_NOTIFY后且父窗口OnNotify函数返回false时调用发出通知控件的该函数
         * @param pNMHDR 包含有关通知消息的信息
         * @param result 函数返回值为true时将该值作为消息的返回值
         * @return       若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnNotified(NMHDR *pNMHDR, LRESULT &result) override;

        /**
         * @brief SelectedIndex属性更改时调用该函数
         */
        virtual void OnSelectedIndexChanged();

    private:
        /**
         * @brief 根据选中的tab更新子元素的Visible属性
         */
        void _UpdateChildVisible(bool invalidMeasure = true);

        /**
         * @brief 发送TCM_INSERTITEMW消息
         */
        int _InsertItem(int index, TCITEMW &item);

        /**
         * @brief 发送TCM_SETITEMW消息
         */
        bool _SetItem(int index, TCITEMW &item);

        /**
         * @brief 发送TCM_DELETEITEM消息
         */
        bool _DeleteItem(int index);

        /**
         * @brief 发送TCM_DELETEALLITEMS消息
         */
        bool _DeleteAllItems();

        /**
         * @brief 计算内容区域的位置和尺寸
         */
        void _CalcContentRect(RECT &rect);

        /**
         * @brief 计算指定内容大小时控件的理想大小
         */
        void _CalcIdealSize(SIZE &size);

        /**
         * @brief 获取当前选中的子元素，若没有选中任何子元素则返回nullptr
         */
        UIElement *_GetSelectedItem();
    };
}
