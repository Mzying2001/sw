#pragma once

#include "Window.h"

namespace sw
{
    /**
     * @brief https://learn.microsoft.com/en-us/windows/win32/api/commdlg/ns-commdlg-choosecolora-r1
     */
    enum class ColorDialogFlags : DWORD {
        // Causes the dialog box to display all available colors in the set of basic colors.
        AnyColor = 0x00000100,

        // Enables the hook procedure specified in the lpfnHook member of this structure.
        // This flag is used only to initialize the dialog box.
        EnableHook = 0x00000010,

        // The hInstance and lpTemplateName members specify a dialog box template to use in
        // place of the default template. This flag is used only to initialize the dialog box.
        EnableTemplate = 0x00000020,

        // The hInstance member identifies a data block that contains a preloaded dialog box
        // template. The system ignores the lpTemplateName member if this flag is specified.
        // This flag is used only to initialize the dialog box.
        EnableTemplateHandle = 0x00000040,

        // Causes the dialog box to display the additional controls that allow the user to
        // create custom colors. If this flag is not set, the user must click the Define Custom
        // Color button to display the custom color controls.
        FullOpen = 0x00000002,

        // Disables the Define Custom Color button.
        PreventFullOpen = 0x00000004,

        // Causes the dialog box to use the color specified in the rgbResult member as the
        // initial color selection.
        RgbInit = 0x00000001,

        // Causes the dialog box to display the Help button. The hwndOwner member must specify
        // the window to receive the HELPMSGSTRING registered messages that the dialog box sends
        // when the user clicks the Help button.
        ShowHelp = 0x00000008,

        // Causes the dialog box to display only solid colors in the set of basic colors.
        SolidColor = 0x00000080,
    };

    /**
     * @brief 标记ColorDialogFlags枚举支持位运算
     */
    _SW_ENUM_ENABLE_BIT_OPERATIONS(ColorDialogFlags);

    /**
     * @brief 颜色选择对话框
     */
    class ColorDialog : public IDialog
    {
    private:
        /**
         * @brief 颜色选择对话框的配置结构体
         */
        CHOOSECOLORW _cc{};

    public:
        /**
         * @brief 对话框标志
         */
        const Property<ColorDialogFlags> Flags;

        /**
         * @brief 选择的颜色，默认值为黑色
         */
        const Property<Color> SelectedColor;

        /**
         * @brief 是否显示完整的颜色选择界面
         */
        const Property<bool> FullOpen;

        /**
         * @brief 自定义颜色数组，包含16个COLORREF元素
         * @note  默认使用一个全局的自定义颜色数组，若需要自定义请在ShowDialog前修改该数组内容
         */
        const Property<COLORREF *> CustomColors;

    public:
        /**
         * @brief 初始化ColorDialog
         */
        ColorDialog();

        /**
         * @brief ColorDialog默认不支持该函数，调用该函数不会执行任何操作
         */
        virtual void Close() override;

        /**
         * @brief ColorDialog默认不支持该函数，调用该函数不会执行任何操作
         */
        virtual void Show() override;

        /**
         * @brief  显示对话框，并指定所有者窗口
         * @return 若用户选择了颜色则返回true，否则返回false
         */
        virtual int ShowDialog(Window *owner = nullptr) override;

        /**
         * @brief  显示对话框，并指定所有者窗口
         * @return 若用户选择了颜色则返回true，否则返回false
         */
        virtual int ShowDialog(Window &owner);

    protected:
        /**
         * @brief 获取颜色选择对话框的配置结构体指针
         */
        CHOOSECOLORW *GetChooseColorStruct();
    };
}
