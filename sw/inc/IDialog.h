#pragma once

namespace sw
{
    class Window; // Window.h

    /**
     * @brief 对话框接口
     */
    class IDialog
    {
    public:
        /**
         * @brief 默认虚析构函数
         */
        virtual ~IDialog() = default;

        /**
         * @brief 关闭对话框
         */
        virtual void Close() = 0;

        /**
         * @brief 显示对话框
         */
        virtual void Show() = 0;

        /**
         * @brief       显示模态对话框
         * @param owner 所有者窗口，若为nullptr则使用当前活动窗口
         * @return      对话框结果，不同对话框有不同的返回值含义
         */
        virtual int ShowDialog(Window *owner = nullptr) = 0;
    };
}
