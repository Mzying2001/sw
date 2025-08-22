#pragma once

#include "ButtonBase.h"

namespace sw
{
    /**
     * @brief 命令链接按钮
     */
    class CommandLink : public ButtonBase
    {
    public:
        /**
         * @brief 描述文字
         */
        const Property<std::wstring> NoteText;

        /**
         * @brief 初始化按钮
         */
        CommandLink();

    protected:
        /**
         * @brief         更新按钮样式
         * @param focused 是否处于焦点状态
         */
        virtual void UpdateButtonStyle(bool focused);

        /**
         * @brief           接收到WM_SETFOCUS时调用该函数
         * @param hPreFocus 丢失焦点的hwnd，可能为NULL
         * @return          若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnSetFocus(HWND hPreFocus) override;

        /**
         * @brief            接收到WM_KILLFOCUS时调用该函数
         * @param hNextFocus 接收到焦点的hwnd，可能为NULL
         * @return           若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnKillFocus(HWND hNextFocus) override;

        /**
         * @brief       接收到WM_KEYDOWN时调用该函数
         * @param key   虚拟按键
         * @param flags 附加信息
         * @return      若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnKeyDown(VirtualKey key, KeyFlags flags) override;
    };
}
