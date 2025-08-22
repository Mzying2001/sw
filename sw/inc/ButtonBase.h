#pragma once

#include "Control.h"

namespace sw
{
    /**
     * @brief 窗口类名为BUTTON的控件类型的基类
     */
    class ButtonBase : public Control
    {
    private:
        /**
         * @brief 是否自动调整大小以适应内容
         */
        bool _autoSize = false;

    public:
        /**
         * @brief 是否自动调整大小以适应内容
         */
        const Property<bool> AutoSize;

        /**
         * @brief 文本过长时是否自动换行
         */
        const Property<bool> MultiLine;

        /**
         * @brief 文本边距
         */
        const Property<Thickness> TextMargin;

    public:
        /**
         * @brief 初始化ButtonBase
         */
        ButtonBase();

        /**
         * @brief 析构函数，这里用纯虚函数使该类成为抽象类
         */
        virtual ~ButtonBase() = 0;

    protected:
        /**
         * @brief 初始化控件
         */
        void InitButtonBase(LPCWSTR lpWindowName, DWORD dwStyle, DWORD dwExStyle);

        /**
         * @brief 被单击时调用该函数
         */
        virtual void OnClicked();

        /**
         * @brief 被双击时调用该函数
         */
        virtual void OnDoubleClicked();

        /**
         * @brief      当父窗口接收到控件的WM_COMMAND时调用该函数
         * @param code 通知代码
         */
        virtual void OnCommand(int code) override;

        /**
         * @brief               测量元素所需尺寸，无需考虑边框和边距
         * @param availableSize 可用的尺寸
         * @return              返回元素需要占用的尺寸
         */
        virtual Size MeasureOverride(const Size &availableSize) override;

    private:
        /**
         * @brief 更新布局标记
         */
        void _UpdateLayoutFlags();

        /**
         * @brief 获取理想尺寸
         * @note  该函数发送BCM_GETIDEALSIZE消息
         */
        bool _GetIdealSize(SIZE &size);

        /**
         * @brief 获取按钮控件中绘制文本的边距
         * @note  该函数发送BCM_GETTEXTMARGIN消息
         */
        bool _GetTextMargin(RECT &rect);

        /**
         * @brief 设置按钮控件中绘制文本的边距
         * @note  该函数发送BCM_SETTEXTMARGIN消息
         */
        bool _SetTextMargin(RECT &rect);
    };
}
