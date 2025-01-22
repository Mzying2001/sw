#pragma once

#include "Control.h"
#include "EnumBit.h"
#include <CommCtrl.h>

namespace sw
{
    /**
     * @brief 表示一个热键
     */
    struct HotKey {
        VirtualKey key;          // 按键
        HotKeyModifier modifier; // 辅助按键
    };

    /**
     * @brief 热键组合
     */
    enum class HotKeyCombination {
        Alt          = HKCOMB_A,    // ALT
        Ctrl         = HKCOMB_C,    // CTRL
        CtrlAlt      = HKCOMB_CA,   // CTRL+ALT
        None         = HKCOMB_NONE, // Unmodified keys
        Shift        = HKCOMB_S,    // SHIFT
        ShiftAlt     = HKCOMB_SA,   // SHIFT+ALT
        ShiftCtrl    = HKCOMB_SC,   // SHIFT+CTRL
        ShiftCtrlAlt = HKCOMB_SCA,  // SHIFT+CTRL+ALT
    };

    /**
     * @brief 标记HotKeyCombination枚举类型支持位运算
     */
    template <>
    struct _EnumSupportBitOperations<HotKeyCombination> : std::true_type {
    };

    /**
     * @brief 热键框
     */
    class HotKeyControl : public Control
    {
    private:
        /**
         * @brief 热键的值
         */
        HotKey _value;

    public:
        /**
         * @brief 当前控件所表示的热键值
         */
        const Property<HotKey> Value;

    public:
        /**
         * @brief 初始化热键框
         */
        HotKeyControl();

        /**
         * @brief                 设置无效组合与默认值
         * @param invalidComb     无效的组合键
         * @param defaultModifier 用户输入无效组合时使用该组合键替换
         */
        void SetRules(HotKeyCombination invalidComb, HotKeyModifier defaultModifier);

    protected:
        /**
         * @brief      当父窗口接收到控件的WM_COMMAND时调用该函数
         * @param code 通知代码
         */
        virtual void OnCommand(int code) override;

        /**
         * @brief 控件表示的热键值发生改变时调用该函数
         */
        virtual void OnValueChanged(HotKey value);

    private:
        /**
         * @brief 更新_value
         */
        void _UpdateValue();
    };
}
