#pragma once

#include "Control.h"

namespace sw
{
    /**
     * @brief 静态控件
     */
    class StaticControl : public Control
    {
    public:
        /**
         * @brief 获取或设置控件的SS_NOTIFY样式
         */
        const Property<bool> Notify;

    public:
        /**
         * @brief 初始化静态控件
         */
        StaticControl();
    };
}
