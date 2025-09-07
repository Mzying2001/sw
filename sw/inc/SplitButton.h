#pragma once

#include "Button.h"

namespace sw
{
    /**
     * @brief 分割按钮
     */
    class SplitButton : public Button
    {
    public:
        /**
         * @brief 初始化分割按钮
         */
        SplitButton();

    protected:
        /**
         * @brief         更新按钮样式
         * @param focused 是否处于焦点状态
         */
        virtual void UpdateButtonStyle(bool focused) override;
    };
}
