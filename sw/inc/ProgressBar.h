#pragma once

#include "Control.h"
#include <CommCtrl.h>

#if !defined(PBST_NORMAL) // g++
#define PBST_NORMAL 0x0001
#endif

#if !defined(PBST_ERROR) // g++
#define PBST_ERROR 0x0002
#endif

#if !defined(PBST_PAUSED) // g++
#define PBST_PAUSED 0x0003
#endif

namespace sw
{
    /**
     * @brief 进度条状态
     */
    enum class ProgressBarState {
        Normal = PBST_NORMAL, // 正常
        Error  = PBST_ERROR,  // 错误
        Paused = PBST_PAUSED, // 暂停
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
