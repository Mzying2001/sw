#pragma once

#include "UIElement.h"

namespace sw
{
    /**
     * @brief 控件
     */
    class Control : virtual public UIElement
    {
    public:
        /**
         * @brief 控件的标识符
         */
        const ReadOnlyProperty<int> ControlId;

    protected:
        /**
         * @brief 初始化控件
         */
        Control();

    public:
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

        /**
         * @brief 控件句柄发生改变时调用该函数
         */
        virtual void HandleChenged();
    };
}
