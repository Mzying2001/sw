#pragma once

#include "Control.h"
#include <CommCtrl.h>

namespace sw
{
    /**
     * @brief 包含链接的文本区域
     */
    class SysLink : public Control
    {
    private:
        /**
         * @brief 文本所需的尺寸
         */
        sw::Size _textSize;

        /**
         * @brief 是否根据文本自动调整尺寸
         */
        bool _autoSize = true;

    public:
        /**
         * @brief 是否忽略回车按键，当该属性为true时按下回车键不会触发单击事件
         */
        const Property<bool> IgnoreReturn;

        /**
         * @brief 是否根据内容自动调整尺寸
         */
        const Property<bool> AutoSize;

    public:
        /**
         * @brief 初始化SysLink
         */
        SysLink();

    protected:
        /**
         * @brief Text属性更改时调用此函数
         */
        virtual void OnTextChanged() override;

        /**
         * @brief       字体改变时调用该函数
         * @param hfont 字体句柄
         */
        virtual void FontChanged(HFONT hfont) override;

        /**
         * @brief               测量元素所需尺寸，无需考虑边框和边距
         * @param availableSize 可用的尺寸
         * @return              返回元素需要占用的尺寸
         */
        virtual Size MeasureOverride(const Size &availableSize) override;

        /**
         * @brief        父窗口接收到WM_NOTIFY后且父窗口OnNotify函数返回false时调用发出通知控件的该函数
         * @param pNMHDR 包含有关通知消息的信息
         * @param result 函数返回值为true时将该值作为消息的返回值
         * @return       若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnNotified(NMHDR *pNMHDR, LRESULT &result) override;

        /**
         * @brief         链接被单击时调用该函数
         * @param pNMLink 包含链接相关信息
         */
        virtual void OnClicked(NMLINK *pNMLink);

    private:
        /**
         * @brief 更新_textSize
         */
        void _UpdateTextSize();

        /**
         * @brief 调整尺寸为_textSize
         */
        void _ResizeToTextSize();

        /**
         * @brief 更新LayoutUpdateCondition属性
         */
        void _UpdateLayoutFlags();
    };
}
