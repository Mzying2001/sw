#pragma once

#include "ButtonBase.h"

namespace sw
{
    /**
     * @brief 选中状态枚举类型
     */
    enum class CheckState {
        Unchecked     = BST_UNCHECKED,     // 未选中
        Checked       = BST_CHECKED,       // 已选中
        Indeterminate = BST_INDETERMINATE, // 不确定状态
    };

    /**
     * @brief 表示可选中的按钮类型（单选框、复选框等）
     */
    class CheckableButton : public ButtonBase
    {
    public:
        /**
         * @brief 选中状态
         */
        const Property<sw::CheckState> CheckState;

        /**
         * @brief 是否选中
         */
        const Property<bool> IsChecked;

    protected:
        /**
         * @brief 初始化CheckableButton
         */
        CheckableButton();

    public:
        /**
         * @brief 析构函数，这里用纯虚函数使该类成为抽象类
         */
        virtual ~CheckableButton() = 0;
    };
}
