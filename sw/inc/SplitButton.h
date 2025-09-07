#pragma once

#include "Button.h"

namespace sw
{
    /**
     * @brief 分割按钮
     * @note  下拉箭头部分单击时默认显示ContextMenu
     */
    class SplitButton : public Button
    {
    public:
        /**
         * @brief 初始化分割按钮
         */
        SplitButton();

        /**
         * @brief  显示下拉菜单
         * @return 若成功显示菜单则返回true，否则返回false
         */
        bool ShowDropDownMenu();

    protected:
        /**
         * @brief       当下拉箭头被单击时调用该函数
         * @param pInfo 包含有关下拉事件的信息
         */
        virtual void OnDropDown(NMBCDROPDOWN *pInfo);

        /**
         * @brief         更新按钮样式
         * @param focused 是否处于焦点状态
         */
        virtual void UpdateButtonStyle(bool focused) override;

        /**
         * @brief        父窗口接收到WM_NOTIFY后且父窗口OnNotify函数返回false时调用发出通知控件的该函数
         * @param pNMHDR 包含有关通知消息的信息
         * @param result 函数返回值为true时将该值作为消息的返回值
         * @return       若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnNotified(NMHDR *pNMHDR, LRESULT &result) override;

        /**
         * @brief               接收到WM_CONTEXTMENU后调用目标控件的该函数
         * @param isKeyboardMsg 消息是否由按下快捷键（Shift+F10、VK_APPS）产生
         * @param mousePosition 鼠标在屏幕中的位置
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnContextMenu(bool isKeyboardMsg, Point mousePosition) override;
    };
}
