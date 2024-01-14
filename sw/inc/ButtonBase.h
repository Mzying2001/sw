#pragma once

#include "Control.h"

namespace sw
{
    class ButtonBase : public Control
    {
    protected:
        /**
         * @brief 初始化ButtonBase
         */
        ButtonBase();

    public:
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
         * @brief      当父窗口接收到控件的WM_COMMAND时调用该函数
         * @param code 通知代码
         */
        virtual void OnCommand(int code) override;

        /**
         * @brief 被单击时调用该函数
         */
        virtual void OnClicked();

        /**
         * @brief 被双击时调用该函数
         */
        virtual void OnDoubleClicked();
    };
}
