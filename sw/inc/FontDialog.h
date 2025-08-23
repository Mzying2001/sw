#pragma once

#include "Window.h"

namespace sw
{
    /**
     * @brief https://learn.microsoft.com/en-us/windows/win32/api/commdlg/ns-commdlg-choosefonta
     */
    enum class FontDialogFlags : DWORD {
        // Causes the dialog box to display the Apply button. You should provide a hook procedure to process
        // WM_COMMAND messages for the Apply button. The hook procedure can send the WM_CHOOSEFONT_GETLOGFONT
        // message to the dialog box to retrieve the address of the structure that contains the current selections
        // for the font.
        Apply = 0x00000200,

        // This flag is obsolete. To limit font selections to all scripts except those that use the OEM or Symbol
        // character sets, use CF_SCRIPTSONLY. To get the original CF_ANSIONLY behavior, use CF_SELECTSCRIPT and
        // specify ANSI_CHARSET in the lfCharSet member of the LOGFONT structure pointed to by lpLogFont.
        ANSIOnly = 0x00000400,

        // This flag is ignored for font enumeration.
        // Windows Vista and Windows XP/2000:  Causes the dialog box to list the available printer and screen fonts.
        // The hDC member is a handle to the device context or information context associated with the printer. This
        // flag is a combination of the CF_SCREENFONTS and CF_PRINTERFONTS flags.
        Both = 0x00000003,

        // Causes the dialog box to display the controls that allow the user to specify strikeout, underline, and
        // text color options. If this flag is set, you can use the rgbColors member to specify the initial text color.
        // You can use the lfStrikeOut and lfUnderline members of the structure pointed to by lpLogFont to specify the
        // initial settings of the strikeout and underline check boxes. ChooseFont can use these members to return the
        // user's selections.
        Effects = 0x00000100,

        // Enables the hook procedure specified in the lpfnHook member of this structure.
        EnableHook = 0x00000008,

        // Indicates that the hInstance and lpTemplateName members specify a dialog box template to use in place of
        // the default template.
        EnableTemplate = 0x00000010,

        // Indicates that the hInstance member identifies a data block that contains a preloaded dialog box template.
        // The system ignores the lpTemplateName member if this flag is specified.
        EnableTemplateHandle = 0x00000020,

        // ChooseFont should enumerate and allow selection of only fixed-pitch fonts.
        FixedPitchOnly = 0x00004000,

        // ChooseFont should indicate an error condition if the user attempts to select a font or style that is not
        // listed in the dialog box.
        ForceFontExist = 0x00010000,

        // ChooseFont should additionally display fonts that are set to Hide in Fonts Control Panel.
        // Windows Vista and Windows XP/2000:  This flag is not supported until Windows 7.
        InavtiveFonts = 0x02000000,

        // ChooseFont should use the structure pointed to by the lpLogFont member to initialize the dialog box controls.
        InitToLogFontStruct = 0x00000040,

        // ChooseFont should select only font sizes within the range specified by the nSizeMin and nSizeMax members.
        LimitSize = 0x00002000,

        // Same as the CF_NOVECTORFONTS flag.
        NoOemFonts = 0x00000800,

        // When using a LOGFONT structure to initialize the dialog box controls, use this flag to prevent the dialog
        // box from displaying an initial selection for the font name combo box. This is useful when there is no single
        // font name that applies to the text selection.
        NoFaceSel = 0x00080000,

        // Disables the Script combo box. When this flag is set, the lfCharSet member of the LOGFONT structure is set
        // to DEFAULT_CHARSET when ChooseFont returns. This flag is used only to initialize the dialog box.
        NoScriptSel = 0x00800000,

        // ChooseFont should not display or allow selection of font simulations.
        NoSimulations = 0x00001000,

        // When using a structure to initialize the dialog box controls, use this flag to prevent the dialog box from
        // displaying an initial selection for the Font Size combo box. This is useful when there is no single font size
        // that applies to the text selection.
        NoSizeSel = 0x00200000,

        // When using a LOGFONT structure to initialize the dialog box controls, use this flag to prevent the dialog box
        // from displaying an initial selection for the Font Style combo box. This is useful when there is no single font
        // style that applies to the text selection.
        NoStyleSel = 0x00100000,

        // ChooseFont should not allow vector font selections.
        NoVectorFonts = 0x00000800,

        // Causes the Font dialog box to list only horizontally oriented fonts.
        NoVertFonts = 0x01000000,

        // This flag is ignored for font enumeration.
        // Windows Vista and Windows XP/2000:  Causes the dialog box to list only the fonts supported by the printer
        // associated with the device context or information context identified by the hDC member. It also causes the font
        // type description label to appear at the bottom of the Font dialog box.
        PrinterFonts = 0x00000002,

        // Specifies that ChooseFont should allow only the selection of scalable fonts. Scalable fonts include vector fonts,
        // scalable printer fonts, TrueType fonts, and fonts scaled by other technologies.
        ScalableOnly = 0x00020000,

        // This flag is ignored for font enumeration.
        // Windows Vista and Windows XP/2000:  Causes the dialog box to list only the screen fonts supported by the system.
        ScreenFonts = 0x00000001,

        // ChooseFont should allow selection of fonts for all non-OEM and Symbol character sets, as well as the ANSI character
        // set. This supersedes the CF_ANSIONLY value.
        ScriptsOnly = 0x00000400,

        // When specified on input, only fonts with the character set identified in the lfCharSet member of the LOGFONT structure
        // are displayed. The user will not be allowed to change the character set specified in the Scripts combo box.
        SelectScript = 0x00400000,

        // Causes the dialog box to display the Help button. The hwndOwner member must specify the window to receive the
        // HELPMSGSTRING registered messages that the dialog box sends when the user clicks the Help button.
        ShowHelp = 0x00000004,

        // ChooseFont should only enumerate and allow the selection of TrueType fonts.
        TrueTypeOnly = 0x00040000,

        // The lpszStyle member is a pointer to a buffer that contains style data that ChooseFont should use to initialize the
        // Font Style combo box. When the user closes the dialog box, ChooseFont copies style data for the user's selection to
        // this buffer.
        // [Note] To globalize your application, you should specify the style by using the lfWeight and lfItalic members of the
        // LOGFONT structure pointed to by lpLogFont. The style name may change depending on the system user interface language.
        UseStyle = 0x00000080,

        // Obsolete. ChooseFont ignores this flag.
        // Windows Vista and Windows XP/2000: ChooseFont should allow only the selection of fonts available on both the printer
        // and the display. If this flag is specified, the CF_SCREENSHOTS and CF_PRINTERFONTS, or CF_BOTH flags should also be
        // specified.
        WYSIWYG = 0x00008000,
    };

    /**
     * @brief 标记FontDialogFlags枚举支持位运算
     */
    _SW_ENUM_ENABLE_BIT_OPERATIONS(FontDialogFlags);

    /**
     * @brief 字体选择对话框
     */
    class FontDialog : public IDialog
    {
    private:
        /**
         * @brief 选择的字体
         */
        sw::Font _font;

        /**
         * @brief 字体选择对话框的配置结构体
         */
        CHOOSEFONTW _cf{};

    public:
        /**
         * @brief 对话框的配置标志
         */
        const Property<FontDialogFlags> Flags;

        /**
         * @brief 选择的字体
         */
        const Property<sw::Font> Font;

        /**
         * @brief 选择的字体名称
         */
        const Property<std::wstring> FontName;

        /**
         * @brief 选择的字体大小
         */
        const Property<double> FontSize;

        /**
         * @brief 选择的字体粗细
         */
        const Property<sw::FontWeight> FontWeight;

        /**
         * @brief 是否显示效果选项（下划线、删除线、颜色）
         */
        const Property<bool> ShowEffects;

        /**
         * @brief 选择的颜色
         */
        const Property<Color> SelectedColor;

    public:
        /**
         * @brief 初始化FontDialog
         */
        FontDialog();

        /**
         * @brief FontDialog默认不支持该函数，调用该函数不会执行任何操作
         */
        virtual void Close() override;

        /**
         * @brief FontDialog默认不支持该函数，调用该函数不会执行任何操作
         */
        virtual void Show() override;

        /**
         * @brief  显示对话框，并指定所有者窗口
         * @return 若用户选择了字体则返回true，否则返回false
         */
        virtual int ShowDialog(Window *owner = nullptr) override;

        /**
         * @brief  显示对话框，并指定所有者窗口
         * @return 若用户选择了字体则返回true，否则返回false
         */
        virtual int ShowDialog(Window &owner);

    protected:
        /**
         * @brief 获取选择字体对话框的配置结构体
         */
        CHOOSEFONTW *GetChooseFontStruct();
    };
}
