#pragma once

#include "CheckableButton.h"

namespace sw
{
    /**
     * @brief 复选框
     */
    class CheckBox : public CheckableButton
    {
    public:
        /**
         * @brief 是否为三态复选框
         */
        const Property<bool> ThreeState;

    public:
        /**
         * @brief 初始化复选框
         */
        CheckBox();
    };
}
