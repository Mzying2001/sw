#pragma once

#include "Control.h"

namespace sw
{
    class TextBoxBase : public Control
    {
    private:
        /**
         * @brief 在读取Text属性时用于判断是否需要更新储存的文本
         */
        bool _isTextChanged = false;

    public:
        /**
         * @brief 是否只读
         */
        const Property<bool> ReadOnly;

        /**
         * @brief 文本的水平对齐方式，可设为左对齐、中心对齐、右对齐
         */
        const Property<sw::HorizontalAlignment> HorizontalContentAlignment;

        /**
         * @brief 是否可以撤销
         */
        const ReadOnlyProperty<bool> CanUndo;

    protected:
        /**
         * @brief 初始化TextBoxBase
         */
        TextBoxBase();

        /**
         * @brief 初始化控件
         */
        void InitTextBoxBase(DWORD dwStyle, DWORD dwExStyle);

        /**
         * @brief  获取窗口文本
         * @return _text字段
         */
        virtual std::wstring &GetText() override;

        /**
         * @brief      当父窗口接收到控件的WM_COMMAND时调用该函数
         * @param code 通知代码
         */
        virtual void OnCommand(int code) override;

    public:
        /**
         * @brief        选择指定文本内容
         * @param start  起始位置
         * @param length 选择文本的长度
         */
        void Select(int start, int length);

        /**
         * @brief 选中所有文本
         */
        void SelectAll();

        /**
         * @brief 将控件内容滚动到当前插入符号位置
         */
        void ScrollToCaret();

        /**
         * @brief  撤销
         * @return 操作是否成功
         */
        bool Undo();

        /**
         * @brief 清空内容
         */
        void Clear();
    };
}
