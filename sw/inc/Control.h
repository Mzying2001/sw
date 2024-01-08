#pragma once

#include "UIElement.h"

namespace sw
{
    /**
     * @brief 控件
     */
    class Control : virtual public UIElement
    {
    protected:
        /**
         * @brief 初始化控件
         */
        Control();

    protected:
        /**
         * @brief 销毁控件句柄并重新初始化，该操作会创建新的句柄并设置样式、文本、字体等
         */
        void ResetHandle();

        /**
         * @brief 控件句柄发生改变时调用该函数
         */
        virtual void HandleChenged();
    };
}
