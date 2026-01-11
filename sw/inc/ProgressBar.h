#pragma once

#include "Control.h"
#include <commctrl.h>

namespace sw
{
    /**
     * @brief 进度条状态
     */
    enum class ProgressBarState {
        Normal = 0x0001, // 正常（PBST_NORMAL）
        Error  = 0x0002, // 错误（PBST_ERROR）
        Paused = 0x0003, // 暂停（PBST_PAUSED）
    };

    /**
     * @brief 进度条控件
     */
    class ProgressBar : public Control
    {
    public:
        /**
         * @brief 进度范围的下限
         */
        const Property<uint16_t> Minimum;

        /**
         * @brief 进度范围的上限
         */
        const Property<uint16_t> Maximum;

        /**
         * @brief 当前进度值
         */
        const Property<uint16_t> Value;

        /**
         * @brief 进度条状态
         */
        const Property<ProgressBarState> State;

        /**
         * @brief 是否显示为垂直进度条
         */
        const Property<bool> Vertical;

    public:
        /**
         * @brief 初始化进度条
         */
        ProgressBar();
    };
}
