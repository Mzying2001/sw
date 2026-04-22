#pragma once

#include <windows.h>
#include <cstdint>
#include <string>

namespace sw
{
    /**
     * @brief 字体的粗细
     */
    enum class FontWeight : int {
        DontCare   = 0,   ///< 不指定（由字体映射器选择默认粗细）
        Thin       = 100, ///< 极细
        ExtraLight = 200, ///< 特细，同 UltraLight
        UltraLight = 200, ///< 超细，同 ExtraLight
        Light      = 300, ///< 细
        Normal     = 400, ///< 正常（常规粗细），同 Regular
        Regular    = 400, ///< 常规，同 Normal
        Medium     = 500, ///< 中等
        SemiBold   = 600, ///< 半粗，同 DemiBold
        DemiBold   = 600, ///< 半粗，同 SemiBold
        Bold       = 700, ///< 粗体
        ExtraBold  = 800, ///< 特粗，同 UltraBold
        UltraBold  = 800, ///< 超粗，同 ExtraBold
        Heavy      = 900, ///< 重磅，同 Black
        Black      = 900, ///< 黑体，同 Heavy
    };

    /**
     * @brief 字符集
     */
    enum class FontCharSet : uint8_t {
        ANSI        = ANSI_CHARSET,        ///< ANSI 字符集（Windows-1252，西欧）
        Baltic      = BALTIC_CHARSET,      ///< 波罗的海字符集
        CheneseBig5 = CHINESEBIG5_CHARSET, ///< 繁体中文（Big5）字符集
        Default     = DEFAULT_CHARSET,     ///< 默认字符集（随系统区域设置而定）
        EastEurope  = EASTEUROPE_CHARSET,  ///< 东欧字符集
        GB2312      = GB2312_CHARSET,      ///< 简体中文（GB2312）字符集
        Greek       = GREEK_CHARSET,       ///< 希腊语字符集
        Hangul      = HANGUL_CHARSET,      ///< 韩语字符集
        Mac         = MAC_CHARSET,         ///< Mac 字符集
        Oem         = OEM_CHARSET,         ///< OEM 字符集（随系统 OEM 代码页）
        Russian     = RUSSIAN_CHARSET,     ///< 俄语字符集
        ShiftJIS    = SHIFTJIS_CHARSET,    ///< 日语 Shift-JIS 字符集
        Symbol      = SYMBOL_CHARSET,      ///< Symbol 符号字符集
        Turkish     = TURKISH_CHARSET,     ///< 土耳其语字符集
        Vietnamese  = VIETNAMESE_CHARSET,  ///< 越南语字符集

        // Windows 的朝鲜语版本：
        Johab = JOHAB_CHARSET, ///< 韩语 Johab 字符集

        // Windows 中东语言版本：
        Arabic = ARABIC_CHARSET, ///< 阿拉伯语字符集
        Hebrew = HEBREW_CHARSET, ///< 希伯来语字符集

        // Windows 的泰语版本：
        Thai = THAI_CHARSET, ///< 泰语字符集
    };

    /**
     * @brief 输出质量
     */
    enum class FontOutPrecision : uint8_t {
        Character      = OUT_CHARACTER_PRECIS, ///< 未使用。
        Default        = OUT_DEFAULT_PRECIS,   ///< 指定默认字体映射器行为。
        Device         = OUT_DEVICE_PRECIS,    ///< 当系统包含多个同名字体时，指示字体映射器选择设备字体。
        Outline        = OUT_OUTLINE_PRECIS,   ///< 此值指示字体映射器从 TrueType 和其他基于大纲的字体中进行选择。
        PostScriptOnly = OUT_PS_ONLY_PRECIS,   ///< 指示字体映射器仅从 PostScript 字体中进行选择。 如果系统中没有安装 PostScript 字体，字体映射器将返回到默认行为。
        Raster         = OUT_RASTER_PRECIS,    ///< 当系统包含多个同名字体时，指示字体映射器选择光栅字体。
        String         = OUT_STRING_PRECIS,    ///< 字体映射器不使用此值，但在枚举光栅字体时会返回此值。
        Stroke         = OUT_STROKE_PRECIS,    ///< 字体映射器不使用此值，但在枚举 TrueType、其他基于轮廓的字体和矢量字体时返回此值。
        TrueTypeOnly   = OUT_TT_ONLY_PRECIS,   ///< 指示字体映射器仅从 TrueType 字体中进行选择。 如果系统中没有安装 TrueType 字体，字体映射器将返回到默认行为。
        TrueType       = OUT_TT_PRECIS,        ///< 当系统包含多个同名字体时，指示字体映射器选择 TrueType 字体。
    };

    /**
     * @brief 剪裁精度
     */
    enum class FontClipPrecision : uint8_t {
        CharacterPrecis = CLIP_CHARACTER_PRECIS, ///< 未使用。
        DefaultPrecis   = CLIP_DEFAULT_PRECIS,   ///< 指定默认剪辑行为。

#if defined(CLIP_DFA_DISABLE)
        DFA_Disable = CLIP_DFA_DISABLE, ///< Windows XP SP1： 关闭字体的字体关联。 请注意，此标志不保证在 Windows Server 2003 之后对任何平台产生任何影响。
#endif

        Embedded  = CLIP_EMBEDDED,  ///< 必须指定此标志才能使用嵌入的只读字体。
        LH_Angles = CLIP_LH_ANGLES, ///< 使用此值时，所有字体的旋转取决于坐标系的方向是左手还是右手。如果未使用，设备字体始终逆时针旋转，但其他字体的旋转取决于坐标系的方向。
        Mask      = CLIP_MASK,      ///< 未使用。

#if defined(CLIP_DFA_OVERRIDE)
        DFA_Override = CLIP_DFA_OVERRIDE, ///< 关闭字体的字体关联。 这与CLIP_DFA_DISABLE相同，但在某些情况下可能会有问题：建议使用的标志是CLIP_DFA_DISABLE。
#endif

        StrokePrecis = CLIP_STROKE_PRECIS, ///< 字体映射器不使用，但在枚举光栅、矢量或 TrueType 字体时返回。 为了兼容，枚举字体时始终返回此值。
        TT_Always    = CLIP_TT_ALWAYS,     ///< 未使用。
    };

    /**
     * @brief 输出质量
     */
    enum class FontQuality : uint8_t {
        AntiAliased   = ANTIALIASED_QUALITY,    ///< 如果字体支持该字体，并且字体大小不是太小或太大，则字体始终为抗锯齿。
        ClearType     = CLEARTYPE_QUALITY,      ///< 如果设置，则尽可能使用 ClearType 抗锯齿方法呈现文本。
        Default       = DEFAULT_QUALITY,        ///< 字体的外观并不重要。
        Draft         = DRAFT_QUALITY,          ///< 字体的外观不如使用PROOF_QUALITY时重要。 对于 GDI 光栅字体，会启用缩放，这意味着可以使用更多字号，但质量可能较低。 如有必要，将合成粗体、斜体、下划线和删除线字体。
        NoAntiAliased = NONANTIALIASED_QUALITY, ///< 字体永远不会抗锯齿。
        Proof         = PROOF_QUALITY,          ///< 字体的字符质量比逻辑字体属性的精确匹配更重要。 对于 GDI 光栅字体，将禁用缩放，并选择大小最接近的字体。 虽然在使用PROOF_QUALITY时可能无法精确映射所选字号，但字体质量较高，外观不会失真。 如有必要，将合成粗体、斜体、下划线和删除线字体。
    };

    /**
     * @brief 字体的间距
     */
    enum class FontPitch : uint8_t {
        Default  = DEFAULT_PITCH,  ///< 默认间距（由字体映射器选择）
        Fixed    = FIXED_PITCH,    ///< 等宽间距（所有字符宽度相同，如 Courier New）
        Variable = VARIABLE_PITCH, ///< 可变间距（字符宽度随字形变化，如 Arial）
    };

    /**
     * @brief 字体系列
     */
    enum class FontFamily : uint8_t {
        Decorative = FF_DECORATIVE, ///< 新奇字体。 例如 Old English。
        DontCare   = FF_DONTCARE,   ///< 使用默认字体。
        Modern     = FF_MODERN,     ///< 具有固定笔划宽度的字体 (带衬线或无衬线) 的空白字体。 Monospace 字体通常是新式字体。 例如，Pica、Elite 和 CourierNew。
        Roman      = FF_ROMAN,      ///< 笔划宽度可变的字体 (比例) 和衬线。 例如 MS Serif。
        Script     = FF_SCRIPT,     ///< 设计为类似于手写的字体。 例如 Script 和 Cursive。
        Swiss      = FF_SWISS,      ///< 笔划宽度可变的字体 (成比例) 且不带衬线。 例如 MS Sans Serif。
    };

    /**
     * @brief 字体类
     */
    class Font
    {
    public:
        /**
         * @brief 字体的字体名称，此字符串的长度不能超过 32
         */
        std::wstring name = L"";

        /**
         * @brief 字体大小，以dip为单位
         */
        double size = 12;

        /**
         * @brief 转义向量与设备的 x 轴之间的角度（以十分之一度为单位）
         */
        int escapement = 0;

        /**
         * @brief 每个字符的基线和设备 x 轴之间的角度（以十分之一度为单位）
         */
        int orientation = 0;

        /**
         * @brief 字体的粗细，范围为 0 到 1000
         */
        FontWeight weight = FontWeight::DontCare;

        /**
         * @brief 是否为斜体
         */
        bool italic = false;

        /**
         * @brief 是否有下划线
         */
        bool underline = false;

        /**
         * @brief 是否有删除线
         */
        bool strikeOut = false;

        /**
         * @brief 字符集
         */
        FontCharSet charSet = FontCharSet::Default;

        /**
         * @brief 输出精度
         */
        FontOutPrecision outPrecision = FontOutPrecision::Default;

        /**
         * @brief 裁剪精度
         */
        FontClipPrecision clipPrecision = FontClipPrecision::DefaultPrecis;

        /**
         * @brief 输出质量
         */
        FontQuality quality = FontQuality::Default;

        /**
         * @brief 字体的间距和系列
         */
        /*uint8_t pitchAndFamily;*/

        /**
         * @brief 字体的间距
         */
        FontPitch pitch = FontPitch::Default;

        /**
         * @brief 字体系列
         */
        FontFamily family = FontFamily::DontCare;

    public:
        /**
         * @brief 构造字体对象
         */
        Font();

        /**
         * @brief 构造指定字体名称和字符集的字体对象
         */
        Font(const std::wstring &name, FontCharSet charSet);

        /**
         * @brief 构造指定字体名称、大小与粗细的字体对象
         */
        Font(const std::wstring &name, double size = 12, FontWeight weight = FontWeight::DontCare);

        /**
         * @brief 从LOGFONTW构造字体对象
         */
        Font(const LOGFONTW &logFont);

        /**
         * @brief 隐式转换LOGFONTW
         */
        operator LOGFONTW() const;

        /**
         * @brief 创建HFONT句柄
         */
        HFONT CreateHandle() const;

        /**
         * @brief 通过HFONT获取字体信息
         */
        static Font GetFont(HFONT hFont);

        /**
         * @brief 获取默认字体，线程中首次调用或参数update为true时会获取系统默认字体
         * @param update 是否重新获取
         * @return 当前线程控件的默认字体
         * @note 可修改返回的引用来更改默认字体（已创建的控件字体不会因此改变）
         */
        static Font &GetDefaultFont(bool update = false);
    };
}
