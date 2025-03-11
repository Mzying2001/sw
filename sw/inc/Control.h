#pragma once

#include "UIElement.h"
#include <CommCtrl.h>

namespace sw
{
    /**
     * @brief 控件
     */
    class Control : virtual public UIElement
    {
    private:
        /**
         * @brief 是否绘制虚线框
         */
        bool _drawFocusRect = false;

    public:
        /**
         * @brief 控件的标识符
         */
        const ReadOnlyProperty<int> ControlId;

    public:
        /**
         * @brief 初始化控件
         */
        Control();

        /**
         * @brief 析构函数，这里用纯虚函数使该类成为抽象类
         */
        virtual ~Control() = 0;

    protected:
        /**
         * @brief         销毁控件句柄并重新初始化，该操作会创建新的句柄并设置样式、文本、字体等
         * @param lpParam 创建控件句柄时传给CreateWindowExW的参数
         */
        void ResetHandle(LPVOID lpParam = NULL);

        /**
         * @brief         销毁控件句柄并重新初始化，并修改样式，该操作会创建新的句柄并设置样式、文本、字体等
         * @param style   新的样式
         * @param exStyle 新的扩展样式
         * @param lpParam 创建控件句柄时传给CreateWindowExW的参数
         */
        void ResetHandle(DWORD style, DWORD exStyle, LPVOID lpParam = NULL);

    protected:
        /**
         * @brief        父窗口接收到WM_NOTIFY后且父窗口OnNotify函数返回false时调用发出通知控件的该函数
         * @param pNMHDR 包含有关通知消息的信息
         * @param result 函数返回值为true时将该值作为消息的返回值
         * @return       若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnNotified(NMHDR *pNMHDR, LRESULT &result) override;

        /**
         * @brief            接收到WM_KILLFOCUS时调用该函数
         * @param hNextFocus 接收到焦点的hwnd，可能为NULL
         * @return           若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnKillFocus(HWND hNextFocus) override;

        /**
         * @brief 通过tab键将焦点移动到当前元素时调用该函数
         */
        virtual void OnTabStop() override;

        /**
         * @brief        接收到NM_CUSTOMDRAW后调用该函数
         * @param pNMCD  包含有关自定义绘制的信息
         * @param result 函数返回值为true时将该值作为消息的返回值
         * @return       若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnCustomDraw(NMCUSTOMDRAW *pNMCD, LRESULT &result);

        /**
         * @brief        绘制控件前调用该函数
         * @param hdc    绘制设备句柄
         * @param result 函数返回值为true时将该值作为NM_CUSTOMDRAW消息的返回值
         * @return       若已完成绘制则返回true，否则返回false以使用默认绘制
         */
        virtual bool OnPrePaint(HDC hdc, LRESULT &result);

        /**
         * @brief     绘制控件后调用该函数
         * @param hdc 绘制设备句柄
         */
        virtual void OnPostPaint(HDC hdc);

        /**
         * @brief     绘制虚线框时调用该函数
         * @param hdc 绘制设备句柄
         */
        virtual void OnDrawFocusRect(HDC hdc);

        /**
         * @brief 控件句柄发生改变时调用该函数
         */
        virtual void HandleChenged();
    };
}
