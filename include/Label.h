#pragma once

#include "StaticControl.h"

namespace sw
{
    /**
     * @brief 文本过长时末尾的处理方式
     */
    enum class TextTrimming {
        None,         // 不处理
        EndEllipsis,  // 按字符截断并显示“...”
        WordEllipsis, // 按单词截断并显示“...”
    };

    /**
     * @brief 标签
     */
    class Label : public StaticControl
    {
    private:
        /**
         * @brief 文本所需的尺寸
         */
        Size _textSize;

        /**
         * @brief 是否根据文本自动调整尺寸
         */
        bool _autoSize = true;

    public:
        /**
         * @brief 文本的水平对齐方式，可设为左对齐、中心对齐、右对齐
         */
        const Property<sw::HorizontalAlignment> HorizontalContentAlignment;

        /**
         * @brief 文本的垂直对齐方式，仅支持顶部对齐和中心对齐，当使用中心对齐时会使自动换行失效
         */
        const Property<sw::VerticalAlignment> VerticalContentAlignment;

        /**
         * @brief 文本过长时末尾的处理方式，当使用截断时水平对齐和自动换行将失效
         */
        const Property<sw::TextTrimming> TextTrimming;

        /**
         * @brief 是否自动换行
         */
        const Property<bool> AutoWrap;

        /**
         * @brief 是否根据文本自动调整尺寸
         */
        const Property<bool> AutoSize;

    public:
        /**
         * @brief 初始化标签
         */
        Label();

    private:
        /**
         * @brief 更新_textSize
         */
        void _UpdateTextSize();

        /**
         * @brief 调整尺寸为_textSize
         */
        void _ResizeToTextSize();

    protected:
        /**
         * @brief               接收到WM_SIZE时调用该函数
         * @param newClientSize 改变后的用户区尺寸
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnSize(Size newClientSize) override;

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
         * @brief               测量控件所需尺寸
         * @param availableSize 可用的尺寸
         */
        virtual void Measure(const Size &availableSize) override;
    };
}
