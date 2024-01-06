// https://github.com/Mzying2001/sw

#pragma once
#include <Windows.h>
#include <memory>
#include <string>
#include <cstdint>
#include <iostream>
#include <map>
#include <algorithm>
#include <vector>
#include <CommCtrl.h>
#include <initializer_list>
#include <tuple>
#include <functional>
#include <type_traits>
#include <sstream>
#include <windowsx.h>

// Alignment.h

namespace sw
{
    /**
     * @brief 水平对齐方式
     */
    enum class HorizontalAlignment {
        Center,  // 中心
        Stretch, // 拉伸
        Left,    // 左对齐
        Right,   // 右对齐
    };

    /**
     * @brief 垂直对齐方式
     */
    enum class VerticalAlignment {
        Center,  // 中心
        Stretch, // 拉伸
        Top,     // 顶部对齐
        Bottom,  // 底部对齐
    };

    /**
     * @brief 排列方式
     */
    enum class Orientation {
        Horizontal, // 水平排列
        Vertical,   // 垂直排列
    };
}

// Color.h


namespace sw
{
    struct Color {
        uint8_t r;
        uint8_t g;
        uint8_t b;

        Color();
        Color(uint8_t r, uint8_t g, uint8_t b);
        Color(COLORREF color);
        operator COLORREF() const;

        friend bool operator==(const Color &left, const Color &right)
        {
            return (left.r == right.r) && (left.g == right.g) && (left.b == right.b);
        }

        friend bool operator!=(const Color &left, const Color &right)
        {
            return (left.r != right.r) || (left.g != right.g) || (left.b != right.b);
        }

        friend std::wostream &operator<<(std::wostream &wos, const Color &color)
        {
            return wos << L"Color{r=" << (int)color.r << L", g=" << (int)color.g << L", b=" << (int)color.b << L"}";
        }

        /*==================================================*/

        enum SolidColors : COLORREF {
            AliceBlue            = RGB(240, 248, 255), // 爱丽丝蓝
            AntiqueWhite         = RGB(250, 235, 215), // 古董白
            Aqua                 = RGB(0, 255, 255),   // 青色
            Aquamarine           = RGB(127, 255, 212), // 碧绿色
            Azure                = RGB(240, 255, 255), // 蔚蓝色
            Beige                = RGB(245, 245, 220), // 米色
            Bisque               = RGB(255, 228, 196), // 橘黄色
            Black                = RGB(0, 0, 0),       // 黑色
            BlanchedAlmond       = RGB(255, 235, 205), // 杏白色
            Blue                 = RGB(0, 0, 255),     // 蓝色
            BlueViolet           = RGB(138, 43, 226),  // 蓝紫色
            Brown                = RGB(165, 42, 42),   // 棕色
            BurlyWood            = RGB(222, 184, 135), // 实木色
            CadetBlue            = RGB(95, 158, 160),  // 军服蓝
            Chartreuse           = RGB(127, 255, 0),   // 查特酒绿
            Chocolate            = RGB(210, 105, 30),  // 巧克力色
            Coral                = RGB(255, 127, 80),  // 珊瑚红
            CornflowerBlue       = RGB(100, 149, 237), // 矢车菊蓝
            Cornsilk             = RGB(255, 248, 220), // 玉米丝色
            Crimson              = RGB(220, 20, 60),   // 深红色
            Cyan                 = RGB(0, 255, 255),   // 青色
            DarkBlue             = RGB(0, 0, 139),     // 深蓝色
            DarkCyan             = RGB(0, 139, 139),   // 深青色
            DarkGoldenrod        = RGB(184, 134, 11),  // 深金黄色
            DarkGray             = RGB(169, 169, 169), // 深灰色
            DarkGreen            = RGB(0, 100, 0),     // 深绿色
            DarkKhaki            = RGB(189, 183, 107), // 深卡其色
            DarkMagenta          = RGB(139, 0, 139),   // 深洋红色
            DarkOliveGreen       = RGB(85, 107, 47),   // 深橄榄绿
            DarkOrange           = RGB(255, 140, 0),   // 深橙色
            DarkOrchid           = RGB(153, 50, 204),  // 深兰花紫
            DarkRed              = RGB(139, 0, 0),     // 深红色
            DarkSalmon           = RGB(233, 150, 122), // 深橙红
            DarkSeaGreen         = RGB(143, 188, 143), // 深海绿色
            DarkSlateBlue        = RGB(72, 61, 139),   // 深石板蓝
            DarkSlateGray        = RGB(47, 79, 79),    // 深石板灰
            DarkTurquoise        = RGB(0, 206, 209),   // 深青绿色
            DarkViolet           = RGB(148, 0, 211),   // 深紫罗兰色
            DeepPink             = RGB(255, 20, 147),  // 深粉红色
            DeepSkyBlue          = RGB(0, 191, 255),   // 深天蓝色
            DimGray              = RGB(105, 105, 105), // 昏灰色
            DodgerBlue           = RGB(30, 144, 255),  // 道奇蓝
            Firebrick            = RGB(178, 34, 34),   // 火砖色
            FloralWhite          = RGB(255, 250, 240), // 花白色
            ForestGreen          = RGB(34, 139, 34),   // 森林绿
            Fuchsia              = RGB(255, 0, 255),   // 紫红色
            Gainsboro            = RGB(220, 220, 220), // 淡灰色
            GhostWhite           = RGB(248, 248, 255), // 幽灵白
            Gold                 = RGB(255, 215, 0),   // 金色
            Goldenrod            = RGB(218, 165, 32),  // 金麒麟色
            Gray                 = RGB(128, 128, 128), // 灰色
            Green                = RGB(0, 128, 0),     // 绿色
            GreenYellow          = RGB(173, 255, 47),  // 绿黄色
            Honeydew             = RGB(240, 255, 240), // 蜜瓜色
            HotPink              = RGB(255, 105, 180), // 热情粉红色
            IndianRed            = RGB(205, 92, 92),   // 印度红
            Indigo               = RGB(75, 0, 130),    // 靛青色
            Ivory                = RGB(255, 255, 240), // 象牙色
            Khaki                = RGB(240, 230, 140), // 卡其色
            Lavender             = RGB(230, 230, 250), // 薰衣草淡紫色
            LavenderBlush        = RGB(255, 240, 245), // 淡紫红色
            LawnGreen            = RGB(124, 252, 0),   // 草坪绿
            LemonChiffon         = RGB(255, 250, 205), // 柠檬绸色
            LightBlue            = RGB(173, 216, 230), // 淡蓝色
            LightCoral           = RGB(240, 128, 128), // 浅珊瑚色
            LightCyan            = RGB(224, 255, 255), // 淡青色
            LightGoldenrodYellow = RGB(250, 250, 210), // 浅金菊黄色
            LightGray            = RGB(211, 211, 211), // 浅灰色
            LightGreen           = RGB(144, 238, 144), // 浅绿色
            LightPink            = RGB(255, 182, 193), // 浅粉红色
            LightSalmon          = RGB(255, 160, 122), // 浅鲑鱼肉色
            LightSeaGreen        = RGB(32, 178, 170),  // 浅海洋绿色
            LightSkyBlue         = RGB(135, 206, 250), // 浅天蓝色
            LightSlateGray       = RGB(119, 136, 153), // 浅石板灰
            LightSteelBlue       = RGB(176, 196, 222), // 浅钢蓝色
            LightYellow          = RGB(255, 255, 224), // 浅黄色
            Lime                 = RGB(0, 255, 0),     // 酸橙色
            LimeGreen            = RGB(50, 205, 50),   // 酸橙绿色
            Linen                = RGB(250, 240, 230), // 亚麻色
            Magenta              = RGB(255, 0, 255),   // 洋红色
            Maroon               = RGB(128, 0, 0),     // 栗色
            MediumAquamarine     = RGB(102, 205, 170), // 中绿宝石色
            MediumBlue           = RGB(0, 0, 205),     // 中蓝色
            MediumOrchid         = RGB(186, 85, 211),  // 中兰花紫
            MediumPurple         = RGB(147, 112, 219), // 中紫色
            MediumSeaGreen       = RGB(60, 179, 113),  // 中海洋绿色
            MediumSlateBlue      = RGB(123, 104, 238), // 中石板蓝色
            MediumSpringGreen    = RGB(0, 250, 154),   // 中春绿色
            MediumTurquoise      = RGB(72, 209, 204),  // 中绿宝石色
            MediumVioletRed      = RGB(199, 21, 133),  // 中紫罗兰色
            MidnightBlue         = RGB(25, 25, 112),   // 午夜蓝
            MintCream            = RGB(245, 255, 250), // 薄荷奶油色
            MistyRose            = RGB(255, 228, 225), // 浅玫瑰色
            Moccasin             = RGB(255, 228, 181), // 鹿皮色
            NavajoWhite          = RGB(255, 222, 173), // 纳瓦白
            Navy                 = RGB(0, 0, 128),     // 海军蓝
            OldLace              = RGB(253, 245, 230), // 老花色
            Olive                = RGB(128, 128, 0),   // 橄榄色
            OliveDrab            = RGB(107, 142, 35),  // 橄榄褐色
            Orange               = RGB(255, 165, 0),   // 橙色
            OrangeRed            = RGB(255, 69, 0),    // 橙红色
            Orchid               = RGB(218, 112, 214), // 兰花紫
            PaleGoldenrod        = RGB(238, 232, 170), // 苍麒麟色
            PaleGreen            = RGB(152, 251, 152), // 苍绿色
            PaleTurquoise        = RGB(175, 238, 238), // 苍宝石绿
            PaleVioletRed        = RGB(219, 112, 147), // 苍紫罗兰色
            PapayaWhip           = RGB(255, 239, 213), // 番木色
            PeachPuff            = RGB(255, 218, 185), // 桃色
            Peru                 = RGB(205, 133, 63),  // 秘鲁色
            Pink                 = RGB(255, 192, 203), // 粉红色
            Plum                 = RGB(221, 160, 221), // 李子色
            PowderBlue           = RGB(176, 224, 230), // 粉蓝色
            Purple               = RGB(128, 0, 128),   // 紫色
            Red                  = RGB(255, 0, 0),     // 红色
            RosyBrown            = RGB(188, 143, 143), // 玫瑰褐色
            RoyalBlue            = RGB(65, 105, 225),  // 皇家蓝
            SaddleBrown          = RGB(139, 69, 19),   // 鞍褐色
            Salmon               = RGB(250, 128, 114), // 鲜肉色
            SandyBrown           = RGB(244, 164, 96),  // 沙褐色
            SeaGreen             = RGB(46, 139, 87),   // 海洋绿色
            SeaShell             = RGB(255, 245, 238), // 海贝壳色
            Sienna               = RGB(160, 82, 45),   // 黄土赭色
            Silver               = RGB(192, 192, 192), // 银色
            SkyBlue              = RGB(135, 206, 235), // 天蓝色
            SlateBlue            = RGB(106, 90, 205),  // 石板蓝色
            SlateGray            = RGB(112, 128, 144), // 石板灰
            Snow                 = RGB(255, 250, 250), // 雪白色
            SpringGreen          = RGB(0, 255, 127),   // 春绿色
            SteelBlue            = RGB(70, 130, 180),  // 钢蓝色
            Tan                  = RGB(210, 180, 140), // 茶色
            Teal                 = RGB(0, 128, 128),   // 水鸭色
            Thistle              = RGB(216, 191, 216), // 蓟色
            Tomato               = RGB(255, 99, 71),   // 番茄色
            Turquoise            = RGB(64, 224, 208),  // 绿宝石色
            Violet               = RGB(238, 130, 238), // 紫罗兰色
            Wheat                = RGB(245, 222, 179), // 浅黄色
            White                = RGB(255, 255, 255), // 白色
            WhiteSmoke           = RGB(245, 245, 245), // 烟白色
            Yellow               = RGB(255, 255, 0),   // 黄色
            YellowGreen          = RGB(154, 205, 50),  // 黄绿色
        };
    };
}

// Cursor.h


namespace sw
{
    /**
     * @brief 系统标准鼠标样式
     * @brief https://learn.microsoft.com/en-us/windows/win32/menurc/about-cursors
     */
    enum class StandardCursor {
        Arrow       = 32512, // Normal select
        IBeam       = 32513, // Text select
        Wait        = 32514, // Busy
        Cross       = 32515, // Precision select
        UpArrow     = 32516, // Alternate select
        Handwriting = 32631, // Handwriting
        SizeNWSE    = 32642, // Diagonal resize 1
        SizeNESW    = 32643, // Diagonal resize 2
        SizeWE      = 32644, // Horizontal resize
        SizeNS      = 32645, // Vertical resize
        SizeAll     = 32646, // Move
        No          = 32648, // Unavailable
        Hand        = 32649, // Link select
        AppStarting = 32650, // Working in background
        Help        = 32651, // Help select
        Pin         = 32671, // Location select
        Person      = 32672, // Person select
    };

    class CursorHelper
    {
    private:
        CursorHelper() = delete;

    public:
        /**
         * @brief        获取系统标准鼠标样式句柄
         * @param cursor 鼠标样式
         * @return       鼠标句柄
         */
        static HCURSOR GetCursorHandle(StandardCursor cursor);

        /**
         * @brief            从指定模块中获取鼠标句柄
         * @param hInstance  DLL或EXE的模块句柄
         * @param resourceId 鼠标的资源序号
         * @return           成功则返回鼠标句柄，否则返回NULL
         */
        static HCURSOR GetCursorHandle(HINSTANCE hInstance, int resourceId);

        /**
         * @brief            从指定模块中获取鼠标句柄
         * @param hInstance  DLL或EXE的模块句柄
         * @param cursorName 鼠标的资源名称
         * @return           成功则返回鼠标句柄，否则返回NULL
         */
        static HCURSOR GetCursorHandle(HINSTANCE hInstance, const std::wstring &cursorName);

        /**
         * @brief          从文件加载鼠标句柄
         * @param fileName 鼠标文件路径
         * @return         成功则返回鼠标句柄，否则返回NULL
         */
        static HCURSOR GetCursorHandle(const std::wstring &fileName);
    };
}

// Font.h


namespace sw
{
    /**
     * @brief 字体的粗细
     */
    enum class FontWeight : int {
        DontCare   = 0,
        Thin       = 100,
        ExtraLight = 200,
        UltraLight = 200,
        Light      = 300,
        Normal     = 400,
        Regular    = 400,
        Medium     = 500,
        SemiBold   = 600,
        DemiBold   = 600,
        Bold       = 700,
        ExtraBold  = 800,
        UltraBold  = 800,
        Heavy      = 900,
        Black      = 900,
    };

    /**
     * @brief 字符集
     */
    enum class FontCharSet : uint8_t {
        ANSI        = ANSI_CHARSET,
        Baltic      = BALTIC_CHARSET,
        CheneseBig5 = CHINESEBIG5_CHARSET,
        Default     = DEFAULT_CHARSET,
        EastEurope  = EASTEUROPE_CHARSET,
        GB2312      = GB2312_CHARSET,
        Greek       = GREEK_CHARSET,
        Hangul      = HANGUL_CHARSET,
        Mac         = MAC_CHARSET,
        Oem         = OEM_CHARSET,
        Russian     = RUSSIAN_CHARSET,
        ShiftJIS    = SHIFTJIS_CHARSET,
        Symbol      = SYMBOL_CHARSET,
        Turkish     = TURKISH_CHARSET,
        Vietnamese  = VIETNAMESE_CHARSET,

        // Windows 的朝鲜语版本：
        Johab = JOHAB_CHARSET,

        // Windows 中东语言版本：
        Arabic = ARABIC_CHARSET,
        Hebrew = HEBREW_CHARSET,

        // Windows 的泰语版本：
        Thai = THAI_CHARSET,
    };

    /**
     * @brief 输出质量
     */
    enum class FontOutPrecision : uint8_t {
        Character      = OUT_CHARACTER_PRECIS, // 未使用。
        Default        = OUT_DEFAULT_PRECIS,   // 指定默认字体映射器行为。
        Device         = OUT_DEVICE_PRECIS,    // 当系统包含多个同名字体时，指示字体映射器选择设备字体。
        Outline        = OUT_OUTLINE_PRECIS,   // 此值指示字体映射器从 TrueType 和其他基于大纲的字体中进行选择。
        PostScriptOnly = OUT_PS_ONLY_PRECIS,   // 指示字体映射器仅从 PostScript 字体中进行选择。 如果系统中没有安装 PostScript 字体，字体映射器将返回到默认行为。
        Raster         = OUT_RASTER_PRECIS,    // 当系统包含多个同名字体时，指示字体映射器选择光栅字体。
        String         = OUT_STRING_PRECIS,    // 字体映射器不使用此值，但在枚举光栅字体时会返回此值。
        Stroke         = OUT_STROKE_PRECIS,    // 字体映射器不使用此值，但在枚举 TrueType、其他基于轮廓的字体和矢量字体时返回此值。
        TrueTypeOnly   = OUT_TT_ONLY_PRECIS,   // 指示字体映射器仅从 TrueType 字体中进行选择。 如果系统中没有安装 TrueType 字体，字体映射器将返回到默认行为。
        TrueType       = OUT_TT_PRECIS,        // 当系统包含多个同名字体时，指示字体映射器选择 TrueType 字体。
    };

    /**
     * @brief 剪裁精度
     */
    enum class FontClipPrecision : uint8_t {
        CharacterPrecis = CLIP_CHARACTER_PRECIS, // 未使用。
        DefaultPrecis   = CLIP_DEFAULT_PRECIS,   // 指定默认剪辑行为。

#if defined(CLIP_DFA_DISABLE)
        DFA_Disable = CLIP_DFA_DISABLE, // Windows XP SP1： 关闭字体的字体关联。 请注意，此标志不保证在 Windows Server 2003 之后对任何平台产生任何影响。
#endif

        Embedded  = CLIP_EMBEDDED,  // 必须指定此标志才能使用嵌入的只读字体。
        LH_Angles = CLIP_LH_ANGLES, // 使用此值时，所有字体的旋转取决于坐标系的方向是左手还是右手。如果未使用，设备字体始终逆时针旋转，但其他字体的旋转取决于坐标系的方向。
        Mask      = CLIP_MASK,      // 未使用。

#if defined(CLIP_DFA_OVERRIDE)
        DFA_Override = CLIP_DFA_OVERRIDE, // 关闭字体的字体关联。 这与CLIP_DFA_DISABLE相同，但在某些情况下可能会有问题：建议使用的标志是CLIP_DFA_DISABLE。
#endif

        StrokePrecis = CLIP_STROKE_PRECIS, // 字体映射器不使用，但在枚举光栅、矢量或 TrueType 字体时返回。 为了兼容，枚举字体时始终返回此值。
        TT_Always    = CLIP_TT_ALWAYS,     // 未使用。
    };

    /**
     * @brief 输出质量
     */
    enum class FontQuality : uint8_t {
        AntiAliased   = ANTIALIASED_QUALITY,    // 如果字体支持该字体，并且字体大小不是太小或太大，则字体始终为抗锯齿。
        ClearType     = CLEARTYPE_QUALITY,      // 如果设置，则尽可能使用 ClearType 抗锯齿方法呈现文本。
        Default       = DEFAULT_QUALITY,        // 字体的外观并不重要。
        Draft         = DRAFT_QUALITY,          // 字体的外观不如使用PROOF_QUALITY时重要。 对于 GDI 光栅字体，会启用缩放，这意味着可以使用更多字号，但质量可能较低。 如有必要，将合成粗体、斜体、下划线和删除线字体。
        NoAntiAliased = NONANTIALIASED_QUALITY, // 字体永远不会抗锯齿。
        Proof         = PROOF_QUALITY,          // 字体的字符质量比逻辑字体属性的精确匹配更重要。 对于 GDI 光栅字体，将禁用缩放，并选择大小最接近的字体。 虽然在使用PROOF_QUALITY时可能无法精确映射所选字号，但字体质量较高，外观不会失真。 如有必要，将合成粗体、斜体、下划线和删除线字体。
    };

    /**
     * @brief 字体的间距
     */
    enum class FontPitch : uint8_t {
        Default  = DEFAULT_PITCH,
        Fixed    = FIXED_PITCH,
        Variable = VARIABLE_PITCH,
    };

    /**
     * @brief 字体系列
     */
    enum class FontFamily : uint8_t {
        Decorative = FF_DECORATIVE, // 新奇字体。 例如 Old English。
        DontCare   = FF_DONTCARE,   // 使用默认字体。
        Modern     = FF_MODERN,     // 具有固定笔划宽度的字体 (带衬线或无衬线) 的空白字体。 Monospace 字体通常是新式字体。 例如，Pica、Elite 和 CourierNew。
        Roman      = FF_ROMAN,      // 笔划宽度可变的字体 (比例) 和衬线。 例如 MS Serif。
        Script     = FF_SCRIPT,     // 设计为类似于手写的字体。 例如 Script 和 Cursive。
        Swiss      = FF_SWISS,      // 笔划宽度可变的字体 (成比例) 且不带衬线。 例如 MS Sans Serif。
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
        Font();
        Font(const std::wstring &name, FontCharSet charSet);
        Font(const std::wstring &name, double size = 12, FontWeight weight = FontWeight::DontCare);
        Font(const LOGFONTW &logFont);
        operator LOGFONTW() const;

        /**
         * @brief 创建HFONT句柄
         */
        HFONT CreateHandle();

        /**
         * @brief 通过HFONT获取字体信息
         */
        static Font GetFont(HFONT hFont);

        /**
         * @brief        获取默认字体，可修改返回的引用来更改控件的默认字体，当首次调用或参数update为true时会获取系统默认字体（已创建的控件字体不会因此改变）
         * @param update 是否重新获取
         * @return       返回默认字体
         */
        static Font &GetDefaultFont(bool update = false);
    };
}

// Icon.h


namespace sw
{
    /**
     * @brief 系统标准图标样式
     * @brief https://learn.microsoft.com/en-us/windows/win32/menurc/about-icons
     */
    enum class StandardIcon {
        Application = 32512, // Default application icon
        Error       = 32513, // Error icon
        Question    = 32514, // Question mark icon
        Warning     = 32515, // Warning icon
        Information = 32516, // Information icon
        WinLogo     = 32517, // Windows logo icon
        Shield      = 32518, // Security shield icon
    };

    class IconHelper
    {
    private:
        IconHelper() = delete;

    public:
        /**
         * @brief      获取系统标准图标句柄
         * @param icon 图标样式
         * @return     图标句柄
         */
        static HICON GetIconHandle(StandardIcon icon);

        /**
         * @brief            从指定模块中获取图标句柄
         * @param hInstance  DLL或EXE的模块句柄
         * @param resourceId 图标的资源序号
         * @return           成功则返回图标句柄，否则返回NULL
         */
        static HICON GetIconHandle(HINSTANCE hInstance, int resourceId);

        /**
         * @brief            从指定模块中获取图标句柄
         * @param hInstance  DLL或EXE的模块句柄
         * @param iconName   图标的资源名称
         * @return           成功则返回图标句柄，否则返回NULL
         */
        static HICON GetIconHandle(HINSTANCE hInstance, const std::wstring &iconName);

        /**
         * @brief          从文件加载图标句柄
         * @param fileName 图标文件的路径
         * @return         成功则返回图标句柄，否则返回NULL
         */
        static HICON GetIconHandle(const std::wstring &fileName);
    };
}

// ITag.h


namespace sw
{
    /**
     * @brief Tag接口
     */
    class ITag
    {
    public:
        /**
         * @brief 获取Tag
         */
        virtual uint64_t GetTag() = 0;

        /**
         * @brief 设置Tag
         */
        virtual void SetTag(uint64_t tag) = 0;
    };
}

// Keys.h


namespace sw
{
    /**
     * @brief https://learn.microsoft.com/en-us/windows/win32/inputdev/about-keyboard-input#keystroke-message-flags
     */
    struct KeyFlags {
        uint16_t repeatCount;  // repeat count, > 0 if several keydown messages was combined into one message
        uint8_t scanCode;      // scan code
        bool isExtendedKey;    // extended-key flag, 1 if scancode has 0xE0 prefix
        bool contextCode;      // indicates whether the ALT key was down
        bool previousKeyState; // indicates whether the key that generated the keystroke message was previously up or down
        bool transitionState;  // transition-state flag, 1 on keyup
        KeyFlags(LPARAM lParam);
    };

    /**
     * @brief 虚拟按键
     */
    enum class VirtualKey {
        MouseLeft   = 0x01, // Left mouse button
        MouseRight  = 0x02, // Right mouse button
        Cancel      = 0x03, // Control-break processing
        MouseMiddle = 0x04, // Middle mouse button (three-button mouse)
        MouseX1     = 0x05, // X1 mouse button
        MouseX2     = 0x06, // X2 mouse button

        //- 0x07 Undefined

        Backspace = 0x08, // BACKSPACE key
        Tab       = 0x09, // TAB key

        //- 0x0A-0B Reserved

        Clear = 0x0C, // CLEAR key
        Enter = 0x0D, // ENTER key

        //- 0x0E-0F Undefined

        Shift      = 0x10, // SHIFT key
        Ctrl       = 0x11, // CTRL key
        Alt        = 0x12, // ALT key
        Pause      = 0x13, // PAUSE key
        CapsLock   = 0x14, // CAPS LOCK key
        Kana       = 0x15, // IME Kana mode
        Hanguel    = 0x15, // IME Hanguel mode (maintained for compatibility; use VK_HANGUL)
        Hangul     = 0x15, // IME Hangul mode
        IME_On     = 0x16, // IME On
        Junja      = 0x17, // IME Junja mode
        Final      = 0x18, // IME final mode
        Hanja      = 0x19, // IME Hanja mode
        KANJI      = 0x19, // IME Kanji mode
        IME_Off    = 0x1A, // IME Off
        Esc        = 0x1B, // ESC key
        Convert    = 0x1C, // IME convert
        Nonconvert = 0x1D, // IME nonconvert
        Accept     = 0x1E, // IME accept
        ModeChange = 0x1F, // IME mode change request
        Space      = 0x20, // SPACEBAR
        PageUp     = 0x21, // PAGE UP key
        PageDown   = 0x22, // PAGE DOWN key
        End        = 0x23, // END key
        Home       = 0x24, // HOME key
        Left       = 0x25, // LEFT ARROW key
        Up         = 0x26, // UP ARROW key
        Right      = 0x27, // RIGHT ARROW key
        Down       = 0x28, // DOWN ARROW key
        Select     = 0x29, // SELECT key
        Print      = 0x2A, // PRINT key
        Execute    = 0x2B, // EXECUTE key
        Snapshot   = 0x2C, // PRINT SCREEN key
        Insert     = 0x2D, // INS key
        Delete     = 0x2E, // DEL key
        Help       = 0x2F, // HELP key

        Zero  = 0x30, // 0 key
        One   = 0x31, // 1 key
        Two   = 0x32, // 2 key
        Three = 0x33, // 3 key
        Four  = 0x34, // 4 key
        Five  = 0x35, // 5 key
        Six   = 0x36, // 6 key
        Seven = 0x37, // 7 key
        Eight = 0x38, // 8 key
        Nine  = 0x39, // 9 key

        //- 0x3A-40 Undefined

        A = 0x41, // A key
        B = 0x42, // B key
        C = 0x43, // C key
        D = 0x44, // D key
        E = 0x45, // E key
        F = 0x46, // F key
        G = 0x47, // G key
        H = 0x48, // H key
        I = 0x49, // I key
        J = 0x4A, // J key
        K = 0x4B, // K key
        L = 0x4C, // L key
        M = 0x4D, // M key
        N = 0x4E, // N key
        O = 0x4F, // O key
        P = 0x50, // P key
        Q = 0x51, // Q key
        R = 0x52, // R key
        S = 0x53, // S key
        T = 0x54, // T key
        U = 0x55, // U key
        V = 0x56, // V key
        W = 0x57, // W key
        X = 0x58, // X key
        Y = 0x59, // Y key
        Z = 0x5A, // Z key

        LeftWindows  = 0x5B, // Left Windows key (Natural keyboard)
        RightWindows = 0x5C, // Right Windows key (Natural keyboard)
        Applications = 0x5D, // Applications key (Natural keyboard)

        //- 0x5E Reserved

        Sleep     = 0x5F, // Computer Sleep key
        NumPad0   = 0x60, // Numeric keypad 0 key
        NumPad1   = 0x61, // Numeric keypad 1 key
        NumPad2   = 0x62, // Numeric keypad 2 key
        NumPad3   = 0x63, // Numeric keypad 3 key
        NumPad4   = 0x64, // Numeric keypad 4 key
        NumPad5   = 0x65, // Numeric keypad 5 key
        NumPad6   = 0x66, // Numeric keypad 6 key
        NumPad7   = 0x67, // Numeric keypad 7 key
        NumPad8   = 0x68, // Numeric keypad 8 key
        NumPad9   = 0x69, // Numeric keypad 9 key
        Multipy   = 0x6A, // Multiply key
        Add       = 0x6B, // Add key
        Separator = 0x6C, // Separator key
        Subtract  = 0x6D, // Subtract key
        Decimal   = 0x6E, // Decimal key
        Divide    = 0x6F, // Divide key
        F1        = 0x70, // F1 key
        F2        = 0x71, // F2 key
        F3        = 0x72, // F3 key
        F4        = 0x73, // F4 key
        F5        = 0x74, // F5 key
        F6        = 0x75, // F6 key
        F7        = 0x76, // F7 key
        F8        = 0x77, // F8 key
        F9        = 0x78, // F9 key
        F10       = 0x79, // F10 key
        F11       = 0x7A, // F11 key
        F12       = 0x7B, // F12 key
        F13       = 0x7C, // F13 key
        F14       = 0x7D, // F14 key
        F15       = 0x7E, // F15 key
        F16       = 0x7F, // F16 key
        F17       = 0x80, // F17 key
        F18       = 0x81, // F18 key
        F19       = 0x82, // F19 key
        F20       = 0x83, // F20 key
        F21       = 0x84, // F21 key
        F22       = 0x85, // F22 key
        F23       = 0x86, // F23 key
        F24       = 0x87, // F24 key

        //- 0x88-8F Unassigned

        NumLock    = 0x90, // NUM LOCK key
        ScrollLock = 0x91, // SCROLL LOCK key

        // 0x92-96 OEM specific
        //- 0x97-9F Unassigned

        LeftShift          = 0xA0, // Left SHIFT key
        RightShift         = 0xA1, // Right SHIFT key
        LeftCtrl           = 0xA2, // Left CONTROL key
        RightCtrl          = 0xA3, // Right CONTROL key
        LeftAlt            = 0xA4, // Left ALT key
        RightAlt           = 0xA5, // Right ALT key
        BrowserBack        = 0xA6, // Browser Back key
        BrowserForward     = 0xA7, // Browser Forward key
        BrowserRefresh     = 0xA8, // Browser Refresh key
        BrowserStop        = 0xA9, // Browser Stop key
        BrowserSearch      = 0xAA, // Browser Search key
        BrowserFavorites   = 0xAB, // Browser Favorites key
        BrowserHome        = 0xAC, // Browser Start and Home key
        VolumeMute         = 0xAD, // Volume Mute key
        VolumeDown         = 0xAE, // Volume Down key
        VolumeUp           = 0xAF, // Volume Up key
        MediaNextTrack     = 0xB0, // Next Track key
        MediaPreviousTrack = 0xB1, // Previous Track key
        MediaStop          = 0xB2, // Stop Media key
        MediaPlayPause     = 0xB3, // Play/Pause Media key
        LaunchMail         = 0xB4, // Start Mail key
        LaunchMediaSelect  = 0xB5, // Select Media key
        LaunchApplication1 = 0xB6, // Start Application 1 key
        LaunchApplication2 = 0xB7, // Start Application 2 key

        //- 0xB8-B9 Reserved

        OEM_1      = 0xBA, // Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the ';:' key
        OEM_Plus   = 0xBB, // For any country/region, the '+' key
        OEM_Comma  = 0xBC, // For any country/region, the ',' key
        OEM_Minus  = 0xBD, // For any country/region, the '-' key
        OEM_Period = 0xBE, // For any country/region, the '.' key
        OEM_2      = 0xBF, // Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '/?' key
        OEM_3      = 0xC0, // Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '`~' key

        //- 0xC1-D7 Reserved
        //- 0xD8-DA Unassigned

        OEM_4 = 0xDB, // Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '[{' key
        OEM_5 = 0xDC, // Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '\|' key
        OEM_6 = 0xDD, // Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the ']}' key
        OEM_7 = 0xDE, // Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the 'single-quote/double-quote' key
        OEM_8 = 0xDF, // Used for miscellaneous characters; it can vary by keyboard.

        //- 0xE0 Reserved
        // 0xE1 OEM specific

        OEM_102 = 0xE2, // The <> keys on the US standard keyboard, or the \\| key on the non-US 102-key keyboard

        // 0xE3-E4 OEM specific

        ProcessKey = 0xE5, // IME PROCESS key

        // 0xE6 OEM specific

        Packet = 0xE7, // Used to pass Unicode characters as if they were keystrokes. The VK_PACKET key is the low word of a 32-bit Virtual Key value used for non-keyboard input methods. For more information, see Remark in KEYBDINPUT, SendInput, WM_KEYDOWN, and WM_KEYUP

        //- 0xE8 Unassigned
        // 0xE9-F5 OEM specific

        Attn      = 0xF6, // Attn key
        CrSel     = 0xF7, // CrSel key
        ExSel     = 0xF8, // ExSel key
        EraseEOF  = 0xF9, // Erase EOF key
        Play      = 0xFA, // Play key
        Zoom      = 0xFB, // Zoom key
        NONAME    = 0xFC, // Reserved
        PA1       = 0xFD, // PA1 key
        OEM_Clear = 0xFE, // Clear key
    };

    /**
     * @brief 鼠标事件时用于判断按键状态
     */
    enum class MouseKey {
        Ctrl        = MK_CONTROL,  // The CTRL key is down.
        MouseLeft   = MK_LBUTTON,  // The left mouse button is down.
        MouseMiddle = MK_MBUTTON,  // The middle mouse button is down.
        MouseRight  = MK_RBUTTON,  // The right mouse button is down.
        Shift       = MK_SHIFT,    // The SHIFT key is down.
        MouseX1     = MK_XBUTTON1, // The first X button is down.
        MouseX2     = MK_XBUTTON2, // The second X button is down.
    };
}

// Path.h


namespace sw
{
    class Path
    {
    private:
        Path() = delete;

    public:
        /**
         * @brief      获取文件名
         * @param path 文件的路径
         * @return     文件名，包含扩展名
         */
        static std::wstring GetFileName(const std::wstring &path);

        /**
         * @brief      获取文件名
         * @param path 文件的路径
         * @return     文件名，不含扩展名
         */
        static std::wstring GetFileNameWithoutExt(const std::wstring &path);

        /**
         * @brief      获取扩展名
         * @param path 文件的路径
         * @return     文件的扩展名，不包含前面的点
         */
        static std::wstring GetExtension(const std::wstring &path);

        /**
         * @brief      获取文件所在路径
         * @param path 文件的路径
         * @return     文件所在路径
         */
        static std::wstring GetDirectory(const std::wstring &path);

        /**
         * @brief       对路径进行拼接
         * @param paths 要拼接的路径
         * @return      完整的路径
         */
        static std::wstring Combine(std::initializer_list<std::wstring> paths);

        /**
         * @brief       获取路径所对应的绝对路径
         * @param paths 要转换的路径
         * @return      若函数成功则返回绝对路径，否则返回空字符串
         */
        static std::wstring GetAbsolutePath(const std::wstring &path);
    };
}

// Point.h


namespace sw
{
    struct Point {
        double x;
        double y;

        Point();
        Point(double x, double y);
        Point(const POINT &point);
        operator POINT() const;

        friend bool operator==(const Point &left, const Point &right)
        {
            return (left.x == right.x) && (left.y == right.y);
        }

        friend bool operator!=(const Point &left, const Point &right)
        {
            return (left.x != right.x) || (left.y != right.y);
        }

        friend std::wostream &operator<<(std::wostream &wos, const Point &point)
        {
            return wos << L"(" << point.x << L", " << point.y << L")";
        }
    };
}

// ProcMsg.h


namespace sw
{
    struct ProcMsg {
        HWND hwnd;
        UINT uMsg;
        WPARAM wParam;
        LPARAM lParam;

        ProcMsg();
        ProcMsg(const HWND &hwnd, const UINT &uMsg, const WPARAM &wParam, const LPARAM &lParam);
    };
}

// Property.h


namespace sw
{
    template <class T>
    class ReadOnlyProperty
    {
    private:
        std::function<const T &()> _funcGet;

    public:
        ReadOnlyProperty(const std::function<const T &()> &funcGet);
        const T &Get() const;
        operator const T &() const;
        const T *operator->() const;
    };

    template <class T>
    class WriteOnlyProperty
    {
    private:
        std::function<void(const T &)> _funcSet;

    public:
        WriteOnlyProperty(const std::function<void(const T &)> &funcSet);
        void Set(const T &value) const;
        const WriteOnlyProperty &operator=(const T &value) const;
    };

    template <class T>
    class Property : public ReadOnlyProperty<T>,
                     public WriteOnlyProperty<T>
    {
    public:
        Property(const std::function<const T &()> &funcGet, const std::function<void(const T &)> &funcSet);
        const Property &operator=(const T &value) const;
        T *operator->() const;
    };

    template <class T>
    std::wostream &operator<<(std::wostream &wos, const ReadOnlyProperty<T> &prop);

    /*================================================================================*/

    template <class T>
    inline ReadOnlyProperty<T>::ReadOnlyProperty(const std::function<const T &()> &funcGet)
        : _funcGet(funcGet)
    {
    }

    template <class T>
    inline const T &ReadOnlyProperty<T>::Get() const
    {
        return this->_funcGet();
    }

    template <class T>
    inline ReadOnlyProperty<T>::operator const T &() const
    {
        return this->_funcGet();
    }

    template <class T>
    inline const T *ReadOnlyProperty<T>::operator->() const
    {
        return &this->_funcGet();
    }

    template <class T>
    inline WriteOnlyProperty<T>::WriteOnlyProperty(const std::function<void(const T &)> &funcSet)
        : _funcSet(funcSet)
    {
    }

    template <class T>
    inline void WriteOnlyProperty<T>::Set(const T &value) const
    {
        this->_funcSet(value);
    }

    template <class T>
    inline const WriteOnlyProperty<T> &WriteOnlyProperty<T>::operator=(const T &value) const
    {
        this->_funcSet(value);
        return *this;
    }

    template <class T>
    inline Property<T>::Property(const std::function<const T &()> &funcGet, const std::function<void(const T &)> &funcSet)
        : ReadOnlyProperty<T>(funcGet), WriteOnlyProperty<T>(funcSet)
    {
    }

    template <class T>
    inline const Property<T> &Property<T>::operator=(const T &value) const
    {
        this->Set(value);
        return *this;
    }

    template <class T>
    inline T *Property<T>::operator->() const
    {
        const T &value = this->Get();
        return const_cast<T *>(&value);
    }

    template <class T>
    inline std::wostream &operator<<(std::wostream &wos, const ReadOnlyProperty<T> &prop)
    {
        return wos << prop.Get();
    }
}

// RoutedEvent.h


namespace sw
{
    /**
     * @brief 路由事件类型枚举
     */
    enum RoutedEventType : uint32_t {
        UserEventBegin = 0,
        UserEventEnd   = 0x80000000,

        /**
         * @brief 尺寸改变，参数类型为sw::SizeChangedEventArgs
         */
        UIElement_SizeChanged,

        /**
         * @brief 位置改变，参数类型为sw::PositionChangedEventArgs
         */
        UIElement_PositionChanged,

        /**
         * @brief Text属性发生变化，参数类型为sw::RoutedEventArgs
         */
        UIElement_TextChanged,

        /**
         * @brief 获取到焦点，参数类型为sw::RoutedEventArgs
         */
        UIElement_GotFocus,

        /**
         * @brief 失去焦点，参数类型为sw::RoutedEventArgs
         */
        UIElement_LostFocus,

        /**
         * @brief 输入字符，参数类型为sw::GotCharEventArgs
         */
        UIElement_GotChar,

        /**
         * @brief 键盘按键按下，参数类型为sw::KeyDownEventArgs
         */
        UIElement_KeyDown,

        /**
         * @brief 键盘按键抬起，参数类型为sw::KeyUpEventArgs
         */
        UIElement_KeyUp,

        /**
         * @brief 鼠标移动，参数类型为sw::MouseMoveEventArgs
         */
        UIElement_MouseMove,

        /**
         * @brief 鼠标离开，参数类型为sw::RoutedEventArgs
         */
        UIElement_MouseLeave,

        /**
         * @brief 鼠标滚轮滚动，参数类型为sw::MouseWheelEventArgs
         */
        UIElement_MouseWheel,

        /**
         * @brief 鼠标按键按下，参数类型为sw::MouseButtonDownEventArgs
         */
        UIElement_MouseButtonDown,

        /**
         * @brief 鼠标按键抬起，参数类型为sw::MouseButtonUpEventArgs
         */
        UIElement_MouseButtonUp,

        /**
         * @brief 要显示用户自定义的上下文菜单前触发该事件，参数类型为sw::ShowContextMenuEventArgs
         */
        UIElement_ShowContextMenu,

        /**
         * @brief 窗口正在关闭，参数类型为sw::WindowClosingEventArgs
         */
        Window_Closing,

        /**
         * @brief 窗口已关闭，参数类型为sw::RoutedEventArgs
         */
        Window_Closed,

        /**
         * @brief 窗口成为前台窗口，参数类型为sw::RoutedEventArgs
         */
        Window_Actived,

        /**
         * @brief 窗口成为后台窗口，参数类型为sw::RoutedEventArgs
         */
        Window_Inactived,

        /**
         * @brief 按钮被单击，参数类型为sw::RoutedEventArgs
         */
        ButtonBase_Clicked,

        /**
         * @brief 按钮被双击，参数类型为sw::RoutedEventArgs
         */
        ButtonBase_DoubleClicked,

        /**
         * @brief 列表视图/列表框/组合框的选中项改变，参数类型为sw::RoutedEventArgs
         */
        ItemsControl_SelectionChanged,

        /**
         * @brief 列表视图某个复选框的选中状态改变，参数类型为sw::ListViewCheckStateChangedEventArgs
         */
        ListView_CheckStateChanged,

        /**
         * @brief 鼠标左键单击列表视图的列标题，参数类型为sw::ListViewHeaderClickedEventArgs
         */
        ListView_HeaderClicked,

        /**
         * @brief 鼠标左键双击列表视图的列标题，参数类型为sw::ListViewHeaderClickedEventArgs
         */
        ListView_HeaderDoubleClicked,

        /**
         * @brief 鼠标左键单击列表视图某个项，参数类型为sw::ListViewItemClickedEventArgs
         */
        ListView_ItemClicked,

        /**
         * @brief 鼠标左键单击列表视图某个项，参数类型为sw::ListViewItemClickedEventArgs
         */
        ListView_ItemDoubleClicked,

        /**
         * @brief 滑块的值被改变，参数类型为sw::RoutedEventArgs
         */
        Slider_ValueChanged,

        /**
         * @brief 滑块被释放，参数类型为sw::RoutedEventArgs
         */
        Slider_EndTrack,

        /**
         * @brief 窗口/面板滚动条滚动，参数类型为sw::ScrollingEventArgs
         */
        Layer_Scrolling,

        /**
         * @brief SelectedIndex属性被改变，参数类型为sw::RoutedEventArgs
         */
        TabControl_SelectedIndexChanged,
    };

    /*================================================================================*/

    /**
     * @brief 路由事件的参数
     */
    struct RoutedEventArgs {
        /**
         * @brief 事件类型
         */
        RoutedEventType eventType;

        /**
         * @brief 事件是否已被处理，若将此字段设为true，则事件不会继续往上传递
         */
        bool handled = false;

        /**
         * @brief 表示是否已处理事件所对应的Windows消息，对于部分消息将字段设为true可取消对DefaultWndProc的调用，若当前事件无对应消息则该字段无意义
         */
        bool handledMsg = false;

        /**
         * @brief RoutedEventArgs构造函数
         */
        RoutedEventArgs(RoutedEventType eventType);
    };

    class UIElement; // UIElement.h
    /**
     * @brief 路由事件类型
     */
    typedef std::function<void(UIElement &, RoutedEventArgs &)> RoutedEvent;
}

// ScrollEnums.h


namespace sw
{
    /**
     * @brief 滚动条方向
     */
    enum class ScrollOrientation {
        Horizontal, // 水平滚动条
        Vertical,   // 垂直滚动条
    };

    /**
     * @brief 滚动条事件
     */
    enum class ScrollEvent {
        LineUp        = SB_LINEUP,        // Scrolls one line up.
        LineLeft      = SB_LINELEFT,      // Scrolls left by one unit.
        LineDown      = SB_LINEDOWN,      // Scrolls one line down.
        LineRight     = SB_LINERIGHT,     // Scrolls right by one unit.
        PageUp        = SB_PAGEUP,        // Scrolls one page up.
        PageLeft      = SB_PAGELEFT,      // Scrolls left by the width of the window.
        PageDown      = SB_PAGEDOWN,      // Scrolls one page down.
        PageRight     = SB_PAGERIGHT,     // Scrolls right by the width of the window.
        ThumbPosition = SB_THUMBPOSITION, // The user has dragged the scroll box (thumb) and released the mouse button. The HIWORD indicates the position of the scroll box at the end of the drag operation.
        ThubmTrack    = SB_THUMBTRACK,    // The user is dragging the scroll box. This message is sent repeatedly until the user releases the mouse button. The HIWORD indicates the position that the scroll box has been dragged to.
        Top           = SB_TOP,           // Scrolls to the upper left.
        Left          = SB_LEFT,          // Scrolls to the upper left.
        Bottom        = SB_BOTTOM,        // Scrolls to the lower right.
        Right         = SB_RIGHT,         // Scrolls to the lower right.
        EndScroll     = SB_ENDSCROLL,     // Ends scroll.
    };
}

// Size.h


namespace sw
{
    struct Size {
        double width;
        double height;

        Size();
        Size(double width, double height);
        Size(const SIZE &size);
        operator SIZE() const;

        friend bool operator==(const Size &left, const Size &right)
        {
            return (left.width == right.width) && (left.height == right.height);
        }

        friend bool operator!=(const Size &left, const Size &right)
        {
            return (left.width != right.width) || (left.height != right.height);
        }

        friend std::wostream &operator<<(std::wostream &wos, const Size &size)
        {
            return wos << L"Size{width=" << size.width << L", height=" << size.height << L"}";
        }
    };
}

// Thickness.h


namespace sw
{
    struct Thickness {
        double left;
        double top;
        double right;
        double bottom;

        Thickness();
        Thickness(double thickness);
        Thickness(double horizontal, double vertical);
        Thickness(double left, double top, double right, double bottom);

        friend bool operator==(const Thickness &left, const Thickness &right)
        {
            return (left.left   == right.left)  &&
                   (left.top    == right.top)   &&
                   (left.right  == right.right) &&
                   (left.bottom == right.bottom);
        }

        friend bool operator!=(const Thickness &left, const Thickness &right)
        {
            return (left.left   != right.left)  ||
                   (left.top    != right.top)   ||
                   (left.right  != right.right) ||
                   (left.bottom != right.bottom);
        }

        friend std::wostream &operator<<(std::wostream &wos, const Thickness &thickness)
        {
            return wos << L"Thickness{left=" << thickness.left << L", top=" << thickness.top << L", right=" << thickness.right << L", bottom=" << thickness.bottom << L"}";
        }
    };
}

// Utils.h


namespace sw
{
    /**
     * @brief 工具类
     */
    class Utils
    {
    private:
        Utils() = delete;

        template <typename T>
        static void _BuildStr(std::wostream &wos, const T &arg)
        {
            wos << arg;
        }

        static void _BuildStr(std::wostream &wos, const char *str)
        {
            wos << Utils::ToWideStr(str);
        }

        static void _BuildStr(std::wostream &wos, const std::string &str)
        {
            wos << Utils::ToWideStr(str);
        }

        template <typename T>
        static void _BuildStr(std::wostream &wos, const std::vector<T> &vec)
        {
            auto beg = vec.begin();
            auto end = vec.end();
            wos << L"[";
            for (auto it = beg; it != end; ++it) {
                if (it != beg)
                    wos << L", ";
                Utils::_BuildStr(wos, *it);
            }
            wos << L"]";
        }

        template <typename TKey, typename TVal>
        static void _BuildStr(std::wostream &wos, const std::map<TKey, TVal> &map)
        {
            auto beg = map.begin();
            auto end = map.end();
            wos << L"{";
            for (auto it = beg; it != end; ++it) {
                if (it != beg)
                    wos << L", ";
                Utils::_BuildStr(wos, it->first);
                wos << L":";
                Utils::_BuildStr(wos, it->second);
            }
            wos << L"}";
        }

    public:
        /**
         * @brief 拼接字符串，也可使用此函数将其他类型转为wstring
         */
        template <typename... Args>
        static std::wstring BuildStr(const Args &...args)
        {
            std::wstringstream wss;
            int _[]{(Utils::_BuildStr(wss, args), 0)...};
            return wss.str();
        }

        /**
         * @brief      将窄字符串转为宽字符串
         * @param str  要转换的字符串
         * @param utf8 是否使用utf8编码
         * @return     转换后的字符串
         */
        static std::wstring ToWideStr(const std::string &str, bool utf8 = false);

        /**
         * @brief      将宽字符串转为窄字符串
         * @param wstr 要转换的字符串
         * @param utf8 是否使用utf8编码
         * @return     转换后的字符串
         */
        static std::string ToMultiByteStr(const std::wstring &wstr, bool utf8 = false);

        /**
         * @brief     删除首尾空白字符
         * @param str 输入的字符串
         * @return    删除首位空白字符后的字符串
         */
        static std::wstring Trim(const std::wstring &str);

        /**
         * @brief     删除串首空白字符
         * @param str 输入的字符串
         * @return    删除串首空白字符后的字符串
         */
        static std::wstring TrimStart(const std::wstring &str);

        /**
         * @brief     删除串尾空白字符
         * @param str 输入的字符串
         * @return    删除串尾空白字符后的字符串
         */
        static std::wstring TrimEnd(const std::wstring &str);

        /**
         * @brief           对字符串按照指定分隔符进行拆分
         * @param str       输入的字符串
         * @param delimiter 分隔符
         * @result          包含字串的vector
         */
        static std::vector<std::wstring> Split(const std::wstring &str, const std::wstring &delimiter);

        /**
         * @brief 取两值中的较大值
         */
        template <typename T>
        static constexpr inline T Max(const T &a, const T &b)
        {
            return a > b ? a : b;
        }

        /**
         * @brief 取两值中的较小值
         */
        template <typename T>
        static constexpr inline T Min(const T &a, const T &b)
        {
            return a < b ? a : b;
        }
    };
}

// WndMsg.h


namespace sw
{
    /**
     * @brief 包含SimpleWindow用到的一些窗口消息
     */
    enum WndMsg : UINT {

        /**
         * @brief SimpleWindow所用消息的起始位置
         */
        WM_SimpleWindowBegin = WM_USER + 0x3000,

        /**
         * @brief 控件布局发生变化时控件所在顶级窗口将收到该消息，wParam和lParam均未使用
         */
        WM_UpdateLayout,
    };
}

// App.h


namespace sw
{
    /**
     * @brief 程序退出消息循环的方式
     */
    enum class AppQuitMode {
        Auto,   // 当所有窗口都销毁时自动退出消息循环
        Manual, // 需手动调用QuitMsgLoop以退出消息循环
    };

    /**
     * @brief App类
     */
    class App
    {
    private:
        App() = delete;

    public:
        /**
         * @brief 应用程序的当前实例的句柄
         */
        static const ReadOnlyProperty<HINSTANCE> Instance;

        /**
         * @brief 当前exe的文件路径
         */
        static const ReadOnlyProperty<std::wstring> ExePath;

        /**
         * @brief 当前exe所在的文件夹路径
         */
        static const ReadOnlyProperty<std::wstring> ExeDirectory;

        /**
         * @brief 当前工作路径
         */
        static const Property<std::wstring> CurrentDirectory;

        /**
         * @brief 程序退出消息循环的方式
         */
        static const Property<AppQuitMode> QuitMode;

        /**
         * @brief  消息循环
         * @return 退出代码
         */
        static int MsgLoop();

        /**
         * @brief          退出当前消息循环
         * @param exitCode 退出代码
         */
        static void QuitMsgLoop(int exitCode = 0);

    private:
        /**
         * @brief  获取当前exe文件路径
         */
        static std::wstring _GetExePath();

        /**
         * @brief 获取当前工作路径
         */
        static std::wstring _GetCurrentDirectory();
    };
}

// Dictionary.h


namespace sw
{
    template <typename TKey, typename TVal>
    class Dictionary; // 向前声明

    /**
     * @brief 以字符串为键值的字典
     */
    template <typename TVal>
    using StrDictionary = Dictionary<std::wstring, TVal>;

    /**
     * @brief 字典类，内部维护了一个指向std::map的智能指针
     */
    template <typename TKey, typename TVal>
    class Dictionary
    {
    private:
        /**
         * @brief 指向std::map的智能指针
         */
        std::shared_ptr<std::map<TKey, TVal>> _pMap;

    public:
        /**
         * @brief 初始化字典
         */
        Dictionary()
            : _pMap(new std::map<TKey, TVal>)
        {
        }

        /**
         * @brief 使用初始化列表
         */
        Dictionary(std::initializer_list<std::pair<const TKey, TVal>> list)
            : Dictionary()
        {
            this->_pMap->insert(list);
        }

        /**
         * @brief 正向迭代器开始
         */
        auto begin() const
        {
            return this->_pMap->begin();
        }

        /**
         * @brief 正向迭代器结束
         */
        auto end() const
        {
            return this->_pMap->end();
        }

        /**
         * @brief 反向迭代器开始
         */
        auto rbegin() const
        {
            return this->_pMap->rbegin();
        }

        /**
         * @brief 反向迭代器结束
         */
        auto rend() const
        {
            return this->_pMap->rend();
        }

        /**
         * @brief     获取或设置值
         * @param key 键值
         */
        auto &operator[](const TKey &key) const
        {
            return this->_pMap->operator[](key);
        }

        /**
         * @brief 判断是否为同一个字典
         */
        friend bool operator==(const Dictionary &left, const Dictionary &right)
        {
            return left._pMap == right._pMap;
        }

        /**
         * @brief 判断是否不是同一个字典
         */
        friend bool operator!=(const Dictionary &left, const Dictionary &right)
        {
            return left._pMap != right._pMap;
        }

        /**
         * @brief 支持Utils::BuildStr
         */
        friend std::wostream &operator<<(std::wostream &wos, const Dictionary &dic)
        {
            wos << Utils::BuildStr(*dic._pMap);
            return wos;
        }

        /**
         * @brief 获取键值对个数
         */
        int Count() const
        {
            return (int)this->_pMap->size();
        }

        /**
         * @brief 字典是否为空
         */
        bool IsEmpty() const
        {
            return this->_pMap->empty();
        }

        /**
         * @brief     是否存在某个键值
         * @param key 要查询的键值
         */
        bool ContainsKey(const TKey &key) const
        {
            return this->_pMap->count(key);
        }

        /**
         * @brief     移除指定键值对
         * @param key 要删除的键值
         */
        void Remove(const TKey &key) const
        {
            this->_pMap->erase(key);
        }

        /**
         * @brief 清空字典
         */
        void Clear() const
        {
            this->_pMap->clear();
        }

        /**
         * @brief 复制当前字典
         */
        Dictionary Copy() const
        {
            Dictionary dic;
            dic._pMap->insert(this->_pMap->begin(), this->_pMap->end());
            return dic;
        }
    };
}

// Dip.h


namespace sw
{
    class Dip
    {
    public:
        struct DipScaleInfo {
            double scaleX;
            double scaleY;
        };

    private:
        Dip();

    public:
        static const ReadOnlyProperty<double> ScaleX;
        static const ReadOnlyProperty<double> ScaleY;
        static void Update(int dpiX, int dpiY);
    };
}

// List.h


namespace sw
{
    template <typename T>
    class List; // 向前声明

    /**
     * @brief 字符串列表
     */
    using StrList = List<std::wstring>;

    /**
     * @brief 列表类，内部维护了一个指向std::vector的智能指针
     */
    template <typename T>
    class List
    {
    private:
        /**
         * @brief 指向std::vector的智能指针
         */
        std::shared_ptr<std::vector<T>> _pVec;

    public:
        /**
         * @brief 初始化列表
         */
        List()
            : _pVec(new std::vector<T>)
        {
        }

        /**
         * @brief 使用初始化列表
         */
        List(std::initializer_list<T> list)
            : _pVec(new std::vector<T>(list))
        {
        }

        /**
         * @brief 初始化列表并指定容量
         */
        List(int capacity)
            : List()
        {
            this->_pVec->reserve(capacity);
        }

        /**
         * @brief 正向迭代器开始
         */
        auto begin() const
        {
            return this->_pVec->begin();
        }

        /**
         * @brief 正向迭代器结束
         */
        auto end() const
        {
            return this->_pVec->end();
        }

        /**
         * @brief 反向迭代器开始
         */
        auto rbegin() const
        {
            return this->_pVec->rbegin();
        }

        /**
         * @brief 反向迭代器结束
         */
        auto rend() const
        {
            return this->_pVec->rend();
        }

        /**
         * @brief 获取或设置列表中指定位置的值
         */
        auto &operator[](int index) const
        {
            return this->_pVec->operator[](index);
        }

        /**
         * @brief 判断是否为同一个列表
         */
        friend bool operator==(const List &left, const List &right)
        {
            return left._pVec == right._pVec;
        }

        /**
         * @brief 判断是否不是同一个列表
         */
        friend bool operator!=(const List &left, const List &right)
        {
            return left._pVec != right._pVec;
        }

        /**
         * @brief 支持Utils::BuildStr
         */
        friend std::wostream &operator<<(std::wostream &wos, const List &list)
        {
            wos << Utils::BuildStr(*list._pVec);
            return wos;
        }

        /**
         * @brief 列表当前的容量
         */
        int Capacity() const
        {
            return (int)this->_pVec->capacity();
        }

        /**
         * @brief 获取元素个数
         */
        int Count() const
        {
            return (int)this->_pVec->size();
        }

        /**
         * @brief 列表是否为空
         */
        bool IsEmpty() const
        {
            return this->_pVec->empty();
        }

        /**
         * @brief 添加一个值到列表末尾
         */
        auto &Append(const T &value) const
        {
            this->_pVec->push_back(value);
            return *this;
        }

        /**
         * @brief 在指定位置插入值
         */
        void Insert(int index, const T &value) const
        {
            this->_pVec->insert(this->_pVec->begin() + index, value);
        }

        /**
         * @brief       列表是否包含某个值
         * @param value 要查找的值
         */
        bool Contains(const T &value) const
        {
            return std::find(this->_pVec->begin(), this->_pVec->end(), value) != this->_pVec->end();
        }

        /**
         * @brief       查找值在列表中的索引
         * @param value 要查找的值
         * @return      若列表中包含该值则返回其索引，否则返回-1
         */
        int IndexOf(const T &value) const
        {
            auto it = std::find(this->_pVec->begin(), this->_pVec->end(), value);
            return it == this->_pVec->end() ? -1 : int(it - this->_pVec->begin());
        }

        /**
         * @brief       移除列表中第一个指定的值
         * @param value 要移除的值
         * @return      是否成功移除
         */
        bool Remove(const T &value) const
        {
            auto it = std::find(this->_pVec->begin(), this->_pVec->end(), value);
            if (it == this->_pVec->end())
                return false;
            this->_pVec->erase(it);
            return true;
        }

        /**
         * @brief       移除指定索引处的值
         * @param index 要移除元素的索引
         */
        void RemoveAt(int index) const
        {
            this->_pVec->erase(this->_pVec->begin() + index);
        }

        /**
         * @brief 清空列表
         */
        void Clear() const
        {
            this->_pVec->clear();
        }

        /**
         * @brief 复制当前列表
         */
        List Copy() const
        {
            List list((int)this->_pVec->capacity());
            list._pVec->assign(this->_pVec->begin(), this->_pVec->end());
            return list;
        }
    };
}

// MenuItem.h


namespace sw
{
    class MenuItem : public ITag
    {
    public:
        /**
         * @brief 储存用户自定义信息
         */
        uint64_t tag = 0;

        /**
         * @brief 菜单项的文本，当值为“-”时表示当前项为分隔条
         */
        std::wstring text;

        /**
         * @brief 子项
         */
        std::vector<std::shared_ptr<MenuItem>> subItems;

        /**
         * @brief 菜单项被单击时调用的函数
         */
        std::function<void(MenuItem &)> command;

        /**
         * @brief      构造一个MenuItem，并设置文本
         * @param text 菜单项的文本
         */
        MenuItem(const std::wstring &text);

        /**
         * @brief          构造一个MenuItem，并设置其子项
         * @param text     菜单下的文本
         * @param subItems 子项列表
         */
        MenuItem(const std::wstring &text, std::initializer_list<MenuItem> subItems);

        /**
         * @brief         构造一个MenuItem，并设置其回调函数
         * @param text    菜单项的文本
         * @param command 被单击时调用的函数
         */
        MenuItem(const std::wstring &text, const decltype(command) &command);

        /**
         * @brief 获取一个值，表示当前菜单项是否为分隔条
         */
        bool IsSeparator() const;

        /**
         * @brief 调用command
         */
        void CallCommand();

        /**
         * @brief 获取Tag
         */
        virtual uint64_t GetTag() override;

        /**
         * @brief 设置Tag
         */
        virtual void SetTag(uint64_t tag) override;
    };
}

// Rect.h


namespace sw
{
    struct Rect {
        double left;
        double top;
        double width;
        double height;

        Rect();
        Rect(double left, double top, double width, double height);
        Rect(const RECT &rect);
        operator RECT() const;

        Point GetPos() const;
        Size GetSize() const;

        friend bool operator==(const Rect& left, const Rect& right)
        {
            return (left.left   == right.left)  &&
                   (left.top    == right.top)   &&
                   (left.width  == right.width) &&
                   (left.height == right.height);
        }

        friend bool operator!=(const Rect& left, const Rect& right)
        {
            return (left.left   != right.left)  ||
                   (left.top    != right.top)   ||
                   (left.width  != right.width) ||
                   (left.height != right.height);
        }

        friend std::wostream& operator<<(std::wostream& wos, const Rect& rect)
        {
            return wos << L"Rect{left=" << rect.left << L", top=" << rect.top << L", width=" << rect.width << L", height=" << rect.height << L"}";
        }
    };
}

// RoutedEventArgs.h


namespace sw
{
    struct RoutedEventArgs; // RoutedEvent.h

    /**
     * @brief       表示特定类型路由事件的事件参数类型，继承自该类的事件参数可以用于RegisterRoutedEvent模板函数
     * @tparam TYPE 一个RoutedEventType枚举值，表示路由事件类型
     */
    template <RoutedEventType TYPE>
    struct RoutedEventArgsOfType : RoutedEventArgs {

        /**
         * @brief 路由事件的类型，RegisterRoutedEvent模板函数使用此字段注册事件
         */
        static constexpr RoutedEventType EventType = TYPE;

        RoutedEventArgsOfType()
            : RoutedEventArgs(EventType)
        {
        }
    };

    /**
     * @brief 结构体模板，用于检测类型T是否含有名为EventType的静态字段
     */
    template <typename T, typename = void>
    struct _HasEventType : std::false_type {
    };

    /**
     * @brief 模板特化：当T包含EventType时，将_IsTypedRoutedEventArgs<T>设为std::true_type
     */
    template <typename T>
    struct _HasEventType<T, decltype(void(std::declval<T>().EventType))> : std::true_type {
    };

    /**
     * @brief 结构体模板，用于检测类型T是否包含事件类型信息
     */
    template <typename T>
    struct _IsTypedRoutedEventArgs : _HasEventType<T> {
    };

    /**
     * @brief 尺寸改变事件参数类型
     */
    struct SizeChangedEventArgs : RoutedEventArgsOfType<UIElement_SizeChanged> {

        Size newClientSize; // 用户区的新尺寸

        SizeChangedEventArgs(Size newClientSize)
            : newClientSize(newClientSize)
        {
        }
    };

    /**
     * @brief 位置改变事件参数类型
     */
    struct PositionChangedEventArgs : RoutedEventArgsOfType<UIElement_PositionChanged> {

        Point newClientPosition; // 移动后用户区左上角的位置

        PositionChangedEventArgs(Point newClientPosition)
            : newClientPosition(newClientPosition)
        {
        }
    };

    /**
     * @brief 输入字符事件类型参数
     */
    struct GotCharEventArgs : RoutedEventArgsOfType<UIElement_GotChar> {

        wchar_t ch;     // 输入的字符
        KeyFlags flags; // 附加信息

        GotCharEventArgs(wchar_t ch, KeyFlags flags)
            : ch(ch), flags(flags)
        {
        }
    };

    /**
     * @brief 键盘按键按下事件参数类型
     */
    struct KeyDownEventArgs : RoutedEventArgsOfType<UIElement_KeyDown> {

        VirtualKey key; // 虚拟按键
        KeyFlags flags; // 附加信息

        KeyDownEventArgs(VirtualKey key, KeyFlags flags)
            : key(key), flags(flags)
        {
        }
    };

    /**
     * @brief 键盘按键抬起事件参数类型
     */
    struct KeyUpEventArgs : RoutedEventArgsOfType<UIElement_KeyUp> {

        VirtualKey key; // 虚拟按键
        KeyFlags flags; // 附加信息

        KeyUpEventArgs(VirtualKey key, KeyFlags flags)
            : key(key), flags(flags)
        {
        }
    };

    /**
     * @brief 鼠标移动事件参数类型
     */
    struct MouseMoveEventArgs : RoutedEventArgsOfType<UIElement_MouseMove> {

        Point mousePosition; // 鼠标位置
        MouseKey keyState;   // 按键状态

        MouseMoveEventArgs(Point mousePosition, MouseKey keyState)
            : mousePosition(mousePosition), keyState(keyState)
        {
        }
    };

    /**
     * @brief 鼠标滚轮滚动事件参数类型
     */
    struct MouseWheelEventArgs : RoutedEventArgsOfType<UIElement_MouseWheel> {

        int wheelDelta;      // 滚轮滚动的距离，为120的倍数
        Point mousePosition; // 鼠标位置
        MouseKey keyState;   // 按键状态

        MouseWheelEventArgs(int wheelDelta, Point mousePosition, MouseKey keyState)
            : wheelDelta(wheelDelta), mousePosition(mousePosition), keyState(keyState)
        {
        }
    };

    /**
     * @brief 鼠标按键按下事件参数类型
     */
    struct MouseButtonDownEventArgs : RoutedEventArgsOfType<UIElement_MouseButtonDown> {

        MouseKey key;        // 按下的按键（左键、中间、右键）
        Point mousePosition; // 鼠标位置
        MouseKey keyState;   // 按键状态

        MouseButtonDownEventArgs(MouseKey key, Point mousePosition, MouseKey keyState)
            : key(key), mousePosition(mousePosition), keyState(keyState)
        {
        }
    };

    /**
     * @brief 鼠标按键抬起事件参数类型
     */
    struct MouseButtonUpEventArgs : RoutedEventArgsOfType<UIElement_MouseButtonUp> {

        MouseKey key;        // 抬起的按键（左键、中间、右键）
        Point mousePosition; // 鼠标位置
        MouseKey keyState;   // 按键状态

        MouseButtonUpEventArgs(MouseKey key, Point mousePosition, MouseKey keyState)
            : key(key), mousePosition(mousePosition), keyState(keyState)
        {
        }
    };

    /**
     * @brief 显示用户自定义上下文菜单的事件参数类型
     */
    struct ShowContextMenuEventArgs : RoutedEventArgsOfType<UIElement_ShowContextMenu> {

        bool cancel = false; // 是否取消显示上下文菜单
        bool isKeyboardMsg;  // 消息是否由按下快捷键（Shift+F10、VK_APPS）产生
        Point mousePosition; // 鼠标在屏幕中的位置

        ShowContextMenuEventArgs(bool isKeyboardMsg, Point mousePosition)
            : isKeyboardMsg(isKeyboardMsg), mousePosition(mousePosition)
        {
        }
    };

    /**
     * @brief 窗口正在关闭事件参数类型
     */
    struct WindowClosingEventArgs : RoutedEventArgsOfType<Window_Closing> {
        bool cancel = false; // 是否取消本次关闭
    };

    /**
     * @brief 窗口/面板滚动条滚动事件参数类型
     */
    struct ScrollingEventArgs : RoutedEventArgsOfType<Layer_Scrolling> {

        bool cancel = false;         // 是否取消滚动条默认行为
        ScrollOrientation scrollbar; // 滚动条类型
        ScrollEvent event;           // 滚动条事件
        double pos;                  // 当event为ThumbPosition或ThubmTrack时表示当前滚动条位置，其他情况固定为0

        ScrollingEventArgs(ScrollOrientation scrollbar, ScrollEvent event, double pos)
            : scrollbar(scrollbar), event(event), pos(pos)
        {
        }
    };

    /**
     * @brief 列表视图某个复选框选中状态改变的事件参数类型
     */
    struct ListViewCheckStateChangedEventArgs : RoutedEventArgsOfType<ListView_CheckStateChanged> {

        int index; // 改变项的索引

        ListViewCheckStateChangedEventArgs(int index)
            : index(index)
        {
        }
    };

    /**
     * @brief 列表视图的列标题单击与双击事件参数类型
     */
    struct ListViewHeaderClickedEventArgs : RoutedEventArgs {

        int index; // 被点击列标题的索引

        ListViewHeaderClickedEventArgs(RoutedEventType eventType, int index)
            : RoutedEventArgs(eventType), index(index)
        {
        }
    };

    /**
     * @brief 列表视图项单击与双击事件参数类型
     */
    struct ListViewItemClickedEventArgs : RoutedEventArgs {

        int row; // 被点击的行
        int col; // 被点击的列

        ListViewItemClickedEventArgs(RoutedEventType eventType, int row, int col)
            : RoutedEventArgs(eventType), row(row), col(col)
        {
        }
    };
}

// Screen.h


namespace sw
{
    /**
     * @brief 屏幕相关
     */
    class Screen
    {
    private:
        Screen() = delete;

    public:
        /**
         * @brief 屏幕宽度
         */
        static const ReadOnlyProperty<double> Width;

        /**
         * @brief 屏幕高度
         */
        static const ReadOnlyProperty<double> Height;

        /**
         * @brief 鼠标在屏幕中的位置
         */
        static const ReadOnlyProperty<Point> CursorPosition;
    };
}

// ILayout.h


namespace sw
{
    /**
     * @brief 布局接口
     */
    class ILayout
    {
    public:
        /**
         * @brief 获取布局标记
         */
        virtual uint64_t GetLayoutTag() = 0;

        /**
         * @brief 获取子控件的数量
         */
        virtual int GetChildLayoutCount() = 0;

        /**
         * @brief 获取对应索引处的子控件
         */
        virtual ILayout &GetChildLayoutAt(int index) = 0;

        /**
         * @brief 获取控件所需尺寸
         */
        virtual Size GetDesireSize() = 0;

        /**
         * @brief 设置当前控件所需的尺寸
         */
        virtual void SetDesireSize(const Size &size) = 0;

        /**
         * @brief               测量控件所需尺寸
         * @param availableSize 可用的尺寸
         */
        virtual void Measure(const Size &availableSize) = 0;

        /**
         * @brief               安排控件位置
         * @param finalPosition 最终控件所安排的位置
         */
        virtual void Arrange(const Rect &finalPosition) = 0;
    };
}

// MenuBase.h


namespace sw
{
    class MenuBase
    {
    private:
        /**
         * @brief 记录菜单项的依赖关系
         */
        struct _MenuItemDependencyInfo {
            HMENU hParent; // 所在菜单的句柄
            HMENU hSelf;   // 若本身含有子项，则此项为本身的菜单句柄，否则为NULL
            int index;     // 所在菜单中的索引
        };

    private:
        /**
         * @brief 菜单句柄，使用InitMenuBase函数设置该值
         */
        HMENU _hMenu = NULL;

        /**
         * @brief 储存所有子项菜单句柄
         */
        std::vector<std::tuple<std::shared_ptr<MenuItem>, HMENU>> _popupMenus;

        /**
         * @brief 储存所有叶子节点，即可以被单击的菜单项，索引为其id
         */
        std::vector<std::shared_ptr<MenuItem>> _leaves;

        /**
         * @brief 记录菜单项直接依赖关系的map
         */
        std::map<MenuItem *, _MenuItemDependencyInfo> _dependencyInfoMap;

    public:
        /**
         * @brief 菜单项集合
         */
        std::vector<std::shared_ptr<MenuItem>> items;

        /**
         * @brief 初始化菜单
         */
        MenuBase();

        /**
         * @brief 重载拷贝构造
         */
        MenuBase(const MenuBase &menu);

        /**
         * @brief 释放资源
         */
        ~MenuBase();

        /**
         * @brief 重载拷贝赋值运算
         */
        MenuBase &operator=(const MenuBase &menu);

        /**
         * @brief 获取菜单句柄
         */
        HMENU GetHandle();

        /**
         * @brief 更新菜单
         */
        void Update();

        /**
         * @brief 初始化菜单并添加菜单项
         */
        void SetItems(std::initializer_list<MenuItem> items);

        /**
         * @brief          重新设置当前菜单中某个菜单项的子项
         * @param item     要修改的菜单项，当该项原先不含有子项时将会调用Update更新整个菜单
         * @param subItems 新的子项列表
         * @return         返回一个bool值，表示操作是否成功
         */
        bool SetSubItems(MenuItem &item, std::initializer_list<MenuItem> subItems);

        /**
         * @brief      添加新的菜单项到菜单
         * @param item 新的菜单项
         */
        void AddItem(const MenuItem &item);

        /**
         * @brief         像当前菜单中的某个菜单项添加新的子项
         * @param item    要添加子项的菜单项，当该项原本不含有子项时将会调用Update更新整个菜单
         * @param subItem 要添加的子菜单项
         * @return        返回一个bool值，表示操作是否成功
         */
        bool AddSubItem(MenuItem &item, const MenuItem &subItem);

        /**
         * @brief      移除当前菜单中的某个子项
         * @param item 要移除的菜单项
         * @return     返回一个bool值，表示操作是否成功
         */
        bool RemoveItem(MenuItem &item);

        /**
         * @brief    通过id获取菜单项
         * @param id 要获取菜单项的id
         * @return   若函数成功则返回菜单项的指针，否则返回nullptr
         */
        MenuItem *GetMenuItem(int id);

        /**
         * @brief      通过索引来获取菜单项
         * @param path 要找项所在下索引
         * @return     若函数成功则返回菜单项的指针，否则返回nullptr
         */
        MenuItem *GetMenuItem(std::initializer_list<int> path);

        /**
         * @brief      通过菜单项的text来获取菜单项
         * @param path 每层要找的text
         * @return     若函数成功则返回菜单项的指针，否则返回nullptr
         */
        MenuItem *GetMenuItem(std::initializer_list<std::wstring> path);

        /**
         * @brief      获取当前菜单中指定菜单项的直接父菜单项
         * @param item 要查询的子菜单项
         * @return     若函数成功则返回指向直接父菜单项的指针，否则返回nullptr
         */
        MenuItem *GetParent(MenuItem &item);

        /**
         * @brief      获取一个值，表示菜单项是否可用
         * @param item 要获取的菜单项
         * @param out  输出值
         * @return     函数是否成功
         */
        bool GetEnabled(MenuItem &item, bool &out);

        /**
         * @brief       设置菜单项是否可用
         * @param item  要修改的菜单项
         * @param value 设置的值
         * @return      修改是否成功
         */
        bool SetEnabled(MenuItem &item, bool value);

        /**
         * @brief      获取一个值，表示菜单项是否选中
         * @param item 要获取的菜单项
         * @param out  输出值
         * @return     函数是否成功
         */
        bool GetChecked(MenuItem &item, bool &out);

        /**
         * @brief       设置菜单项是否选中
         * @param item  要修改的菜单项
         * @param value 设置的值
         * @return      修改是否成功
         */
        bool SetChecked(MenuItem &item, bool value);

        /**
         * @brief       设置菜单项文本
         * @param item  要修改的菜单项
         * @param value 设置的值
         * @return      修改是否成功
         */
        bool SetText(MenuItem &item, const std::wstring &value);

    private:
        /**
         * @brief 清除已添加的所有菜单项
         */
        void _ClearAddedItems();

        /**
         * @brief       添加菜单项到指定句柄
         * @param hMenu 要添加子项的菜单句柄
         * @param pItem 要添加的菜单项
         * @param index 菜单项在父菜单中的索引
         */
        void _AppendMenuItem(HMENU hMenu, std::shared_ptr<MenuItem> pItem, int index);

        /**
         * @brief      获取菜单项的依赖信息
         * @param item 要获取信息的菜单项
         * @return     若函数成功则返回指向_MenuItemDependencyInfo的指针，否则返回nullptr
         */
        _MenuItemDependencyInfo *_GetMenuItemDependencyInfo(MenuItem &item);

    protected:
        /**
         * @brief       设置菜单句柄
         * @param hMenu 菜单句柄
         */
        void InitMenuBase(HMENU hMenu);

        /**
         * @brief       根据索引获取ID
         * @param index 索引
         * @return      菜单项的ID
         */
        virtual int IndexToID(int index) = 0;

        /**
         * @brief    根据ID获取索引
         * @param id 菜单项的ID
         * @return   索引
         */
        virtual int IDToIndex(int id) = 0;
    };
}

// WndBase.h


namespace sw
{
    class WndBase
    {
    private:
        /**
         * @brief 窗口过程函数，调用对象的WndProc
         */
        static LRESULT CALLBACK _WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    private:
        /**
         * @brief 窗口句柄
         */
        HWND _hwnd = NULL;

        /**
         * @brief 字体句柄
         */
        HFONT _hfont = NULL;

        /**
         * @brief 字体信息
         */
        sw::Font _font;

        /**
         * @brief 窗口的位置和尺寸
         */
        sw::Rect _rect = sw::Rect();

        /**
         * @brief 窗口标题或文本
         */
        std::wstring _text = L"";

        /**
         * @brief 背景颜色
         */
        Color _backColor = Color::White;

        /**
         * @brief 文本颜色
         */
        Color _textColor = Color::Black;

        /**
         * @brief 控件是否拥有焦点
         */
        bool _focused = false;

        /**
         * @brief 窗口是否已销毁
         */
        bool _isDestroyed = false;

        /**
         * @brief 当前对象是控件时该函数指针指向控件原本的WndProc
         */
        WNDPROC _controlOldWndProc = NULL;

    public:
        /**
         * @brief 窗口句柄
         */
        const ReadOnlyProperty<HWND> Handle;

        /**
         * @brief 字体
         */
        const Property<sw::Font> Font;

        /**
         * @brief 字体名称
         */
        const Property<std::wstring> FontName;

        /**
         * @brief 字体大小
         */
        const Property<double> FontSize;

        /**
         * @brief 字体粗细
         */
        const Property<sw::FontWeight> FontWeight;

        /**
         * @brief 位置和尺寸
         */
        const Property<sw::Rect> Rect;

        /**
         * @brief 左边
         */
        const Property<double> Left;

        /**
         * @brief 顶边
         */
        const Property<double> Top;

        /**
         * @brief 宽度
         */
        const Property<double> Width;

        /**
         * @brief 高度
         */
        const Property<double> Height;

        /**
         * @brief 用户区尺寸
         */
        const ReadOnlyProperty<sw::Rect> ClientRect;

        /**
         * @brief 用户区宽度
         */
        const ReadOnlyProperty<double> ClientWidth;

        /**
         * @brief 用户区高度
         */
        const ReadOnlyProperty<double> ClientHeight;

        /**
         * @brief 窗口或控件是否可用
         */
        const Property<bool> Enabled;

        /**
         * @brief 窗口或控件是否可见
         */
        const Property<bool> Visible;

        /**
         * @brief 窗口标题或控件文本
         */
        const Property<std::wstring> Text;

        /**
         * @brief 背景颜色，对于部分控件该属性可能会失效
         */
        const Property<Color> BackColor;

        /**
         * @brief 文本颜色，对于部分控件该属性可能会失效
         */
        const Property<Color> TextColor;

        /**
         * @brief 窗口是否拥有焦点
         */
        const Property<bool> Focused;

        /**
         * @brief 父窗口
         */
        const ReadOnlyProperty<WndBase *> Parent;

        /**
         * @brief 是否已销毁，当该值为true时不应该继续使用当前对象
         */
        const ReadOnlyProperty<bool> IsDestroyed;

    protected:
        WndBase();
        virtual ~WndBase();

        WndBase(const WndBase &)            = delete; // 删除拷贝构造函数
        WndBase(WndBase &&)                 = delete; // 删除移动构造函数
        WndBase &operator=(const WndBase &) = delete; // 删除拷贝赋值运算符
        WndBase &operator=(WndBase &&)      = delete; // 删除移动赋值运算符

        /**
         * @brief 初始化为窗口，该函数会调用CreateWindowExW
         */
        void InitWindow(LPCWSTR lpWindowName, DWORD dwStyle, DWORD dwExStyle);

        /**
         * @brief 初始化为控件，该函数会调用CreateWindowExW
         */
        void InitControl(LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, DWORD dwExStyle);

        /**
         * @brief 获取窗口样式
         */
        LONG_PTR GetStyle();

        /**
         * @brief 设置窗口样式
         */
        void SetStyle(LONG_PTR style);

        /**
         * @brief 获取窗口的某个样式
         */
        bool GetStyle(LONG_PTR style);

        /**
         * @brief 设置窗口的某个样式
         */
        void SetStyle(LONG_PTR style, bool value);

        /**
         * @brief 获取扩展窗口样式
         */
        LONG_PTR GetExtendedStyle();

        /**
         * @brief 设置扩展窗口样式
         */
        void SetExtendedStyle(LONG_PTR style);

        /**
         * @brief 获取窗口的某个扩展样式
         */
        bool GetExtendedStyle(LONG_PTR style);

        /**
         * @brief 设置窗口的某个扩展样式
         */
        void SetExtendedStyle(LONG_PTR style, bool value);

        /**
         * @brief 获取字体句柄
         */
        HFONT GetFontHandle();

        /**
         * @brief 调用默认的WndProc，对于窗口则调用DefWindowProcW，控件则调用_controlOldWndProc
         */
        LRESULT DefaultWndProc(const ProcMsg &refMsg);

        /**
         * @brief 对WndProc的封装
         */
        virtual LRESULT WndProc(const ProcMsg &refMsg);

        /**
         * @brief 更新_text字段
         */
        void UpdateText();

        /**
         * @brief  获取窗口文本
         * @return _text字段
         */
        virtual std::wstring &GetText();

        /**
         * @brief       调用SetWindowTextW设置窗口文本
         * @param value 要设置的文本
         */
        virtual void SetText(const std::wstring &value);

        /**
         * @brief        设置背景颜色
         * @param color  要设置的颜色
         * @param redraw 是否重绘
         */
        virtual void SetBackColor(Color color, bool redraw);

        /**
         * @brief        设置文本颜色
         * @param color  要设置的颜色
         * @param redraw 是否重绘
         */
        virtual void SetTextColor(Color color, bool redraw);

        /**
         * @brief  接收到WM_CREATE时调用该函数
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnCreate();

        /**
         * @brief  接收到WM_CLOSE时调用该函数
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnClose();

        /**
         * @brief  接收到WM_DESTROY时调用该函数
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnDestroy();

        /**
         * @brief  接收到WM_PAINT时调用该函数
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnPaint();

        /**
         * @brief 在OnPaint函数完成之后调用该函数
         */
        virtual void OnEndPaint();

        /**
         * @brief                   接收到WM_MOVE时调用该函数
         * @param newClientPosition 移动后用户区左上角的位置
         * @return                  若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMove(Point newClientPosition);

        /**
         * @brief               接收到WM_SIZE时调用该函数
         * @param newClientSize 改变后的用户区尺寸
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnSize(Size newClientSize);

        /**
         * @brief Text属性更改时调用此函数
         */
        virtual void OnTextChanged();

        /**
         * @brief            接收到WM_SETFOCUS时调用该函数
         * @param hPrevFocus 丢失焦点的hwnd，可能为NULL
         * @return           若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnSetFocus(HWND hPrevFocus);

        /**
         * @brief            接收到WM_KILLFOCUS时调用该函数
         * @param hNextFocus 接收到焦点的hwnd，可能为NULL
         * @return           若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnKillFocus(HWND hNextFocus);

        /**
         * @brief               接收到WM_MOUSEMOVE时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState      指示某些按键是否按下
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseMove(Point mousePosition, MouseKey keyState);

        /**
         * @brief  接收到WM_MOUSELEAVE时调用该函数
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseLeave();

        /**
         * @brief               接收到WM_MOUSEWHEEL时调用该函数
         * @param wheelDelta    滚轮滚动的距离，为120的倍数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState      指示某些按键是否按下
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseWheel(int wheelDelta, Point mousePosition, MouseKey keyState);

        /**
         * @brief               接收到WM_LBUTTONDOWN时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState      指示某些按键是否按下
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseLeftButtonDown(Point mousePosition, MouseKey keyState);

        /**
         * @brief               接收到WM_LBUTTONUP时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState      指示某些按键是否按下
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseLeftButtonUp(Point mousePosition, MouseKey keyState);

        /**
         * @brief               接收到WM_LBUTTONDBLCLK时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState      指示某些按键是否按下
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseLeftButtonDoubleClick(Point mousePosition, MouseKey keyState);

        /**
         * @brief               接收到WM_RBUTTONDOWN时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState      指示某些按键是否按下
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseRightButtonDown(Point mousePosition, MouseKey keyState);

        /**
         * @brief               接收到WM_RBUTTONUP时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState      指示某些按键是否按下
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseRightButtonUp(Point mousePosition, MouseKey keyState);

        /**
         * @brief               接收到WM_RBUTTONDBLCLK时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState      指示某些按键是否按下
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseRightButtonDoubleClick(Point mousePosition, MouseKey keyState);

        /**
         * @brief               接收到WM_MBUTTONDOWN时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState      指示某些按键是否按下
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseMiddleButtonDown(Point mousePosition, MouseKey keyState);

        /**
         * @brief               接收到WM_MBUTTONUP时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState      指示某些按键是否按下
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseMiddleButtonUp(Point mousePosition, MouseKey keyState);

        /**
         * @brief               接收到WM_MBUTTONDBLCLK时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState      指示某些按键是否按下
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseMiddleButtonDoubleClick(Point mousePosition, MouseKey keyState);

        /**
         * @brief       接收到WM_CHAR时调用该函数
         * @param ch    按键的字符代码
         * @param flags 附加信息
         * @return      若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnChar(wchar_t ch, KeyFlags flags);

        /**
         * @brief       接收到WM_DEADCHAR时调用该函数
         * @param ch    按键的字符代码
         * @param flags 附加信息
         * @return      若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnDeadChar(wchar_t ch, KeyFlags flags);

        /**
         * @brief       接收到WM_KEYDOWN时调用该函数
         * @param key   虚拟按键
         * @param flags 附加信息
         * @return      若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnKeyDown(VirtualKey key, KeyFlags flags);

        /**
         * @brief       接收到WM_KEYUP时调用该函数
         * @param key   虚拟按键
         * @param flags 附加信息
         * @return      若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnKeyUp(VirtualKey key, KeyFlags flags);

        /**
         * @brief       接收到WM_SYSCHAR时调用该函数
         * @param ch    按键的字符代码
         * @param flags 附加信息
         * @return      若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnSysChar(wchar_t ch, KeyFlags flags);

        /**
         * @brief       接收到WM_SYSDEADCHAR时调用该函数
         * @param ch    按键的字符代码
         * @param flags 附加信息
         * @return      若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnSysDeadChar(wchar_t ch, KeyFlags flags);

        /**
         * @brief       接收到WM_SYSKEYDOWN时调用该函数
         * @param key   虚拟按键
         * @param flags 附加信息
         * @return      若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnSysKeyDown(VirtualKey key, KeyFlags flags);

        /**
         * @brief       接收到WM_SYSKEYUP时调用该函数
         * @param key   虚拟按键
         * @param flags 附加信息
         * @return      若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnSysKeyUp(VirtualKey key, KeyFlags flags);

        /**
         * @brief Visible属性改变时调用此函数
         */
        virtual void VisibleChanged(bool newVisible);

        /**
         * @brief  设置父窗口
         * @return 设置是否成功
         */
        virtual bool SetParent(WndBase *parent);

        /**
         * @brief           父窗口改变时调用此函数
         * @param newParent 新的父窗口
         */
        virtual void ParentChanged(WndBase *newParent);

        /**
         * @brief      当父窗口接收到控件的WM_COMMAND时调用该函数
         * @param code 通知代码
         */
        virtual void OnCommand(int code);

        /**
         * @brief          当WM_COMMAND接收到控件命令时调用该函数
         * @param pControl 控件对象指针
         * @param code     通知代码
         * @param id       控件id
         */
        virtual void OnControlCommand(WndBase *pControl, int code, int id);

        /**
         * @brief    当WM_COMMAND接收到菜单命令时调用该函数
         * @param id 菜单id
         */
        virtual void OnMenuCommand(int id);

        /**
         * @brief    当WM_COMMAND接收到快捷键命令时调用该函数
         * @param id 快捷键id
         */
        virtual void OnAcceleratorCommand(int id);

        /**
         * @brief      窗口句柄初始化完成
         * @param hwnd 窗口句柄
         */
        virtual void HandleInitialized(HWND hwnd);

        /**
         * @brief       字体改变时调用该函数
         * @param hfont 字体句柄
         */
        virtual void FontChanged(HFONT hfont);

        /**
         * @brief         接收到WM_SETCURSOR消息时调用该函数
         * @param hwnd    鼠标所在窗口的句柄
         * @param hitTest hit-test的结果，详见WM_NCHITTEST消息的返回值
         * @param message 触发该事件的鼠标消息，如WM_MOUSEMOVE
         * @param result  消息的返回值，默认为false
         * @return        若返回true则将result作为消息的返回值，否则使用DefaultWndProc的返回值
         */
        virtual bool OnSetCursor(HWND hwnd, int hitTest, int message, bool &result);

        /**
         * @brief               接收到WM_CONTEXTMENU后调用目标控件的该函数
         * @param isKeyboardMsg 消息是否由按下快捷键（Shift+F10、VK_APPS）产生
         * @param mousePosition 鼠标在屏幕中的位置
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnContextMenu(bool isKeyboardMsg, Point mousePosition);

        /**
         * @brief        接收到WM_NOTIFY后调用该函数
         * @param pNMHDR 包含有关通知消息的信息
         * @return       若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnNotify(NMHDR *pNMHDR);

        /**
         * @brief 父窗口接收到WM_NOTIFY后调用发出通知控件的该函数
         */
        virtual void OnNotified(NMHDR *pNMHDR);

        /**
         * @brief       接收到WM_VSCROLL时调用目标控件的该函数
         * @param event 事件类型，即消息wParam的低字
         * @param pos   当前滚动条的位置，仅当event为SB_THUMBPOSITION或SB_THUMBTRACK时有效
         * @return      若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnVerticalScroll(int event, int pos);

        /**
         * @brief       接收到WM_HSCROLL时调用目标控件的该函数
         * @param event 事件类型，即消息wParam的低字
         * @param pos   当前滚动条的位置，仅当event为SB_THUMBPOSITION或SB_THUMBTRACK时有效
         * @return      若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnHorizontalScroll(int event, int pos);

        /**
         * @brief          接收到WM_ENABLE时调用该函数
         * @param newValue Enabled的新值
         * @return         若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnEnabledChanged(bool newValue);

        /**
         * @brief           接收到WM_CTLCOLORxxx时调用该函数
         * @param hdc       控件的显示上下文句柄
         * @param hControl  控件的句柄
         * @param hRetBrush 要返回的画笔
         * @return          若返回true则将hRetBrush作为消息的返回值，否则使用DefaultWndProc的返回值
         */
        virtual bool OnCtlColor(HDC hdc, HWND hControl, HBRUSH &hRetBrush);

    public:
        /**
         * @brief 该函数调用ShowWindow
         */
        void Show(int nCmdShow);

        /**
         * @brief 发送关闭消息
         */
        void Close();

        /**
         * @brief 该函数调用UpdateWindow
         */
        void Update();

        /**
         * @brief 更新字体
         */
        void UpdateFont();

        /**
         * @brief       重画
         * @param erase 是否擦除旧的背景
         */
        void Redraw(bool erase = false);

        /**
         * @brief 判断当前对象是否是控件
         */
        bool IsControl();

        /**
         * @brief       获取用户区点在屏幕上点的位置
         * @param point 用户区坐标
         * @return      该点在屏幕上的坐标
         */
        Point PointToScreen(const Point &point);

        /**
         * @brief             获取屏幕上点在当前用户区点的位置
         * @param screenPoint 屏幕上点的坐标
         * @return            该点在用户区的坐标
         */
        Point PointFromScreen(const Point &screenPoint);

        /**
         * @brief 发送消息
         */
        LRESULT SendMessageW(UINT uMsg, WPARAM wParam, LPARAM lParam);

        /**
         * @brief      通过窗口句柄获取WndBase
         * @param hwnd 窗口句柄
         * @return     若函数成功则返回对象的指针，否则返回nullptr
         */
        static WndBase *GetWndBase(HWND hwnd);

        /**
         * @brief 重载==运算符，判断是否为同一个引用
         */
        friend bool operator==(const WndBase &left, const WndBase &right)
        {
            return &left == &right;
        }

        /**
         * @brief 重载!=运算符，判断是否为不同引用
         */
        friend bool operator!=(const WndBase &left, const WndBase &right)
        {
            return &left != &right;
        }
    };
}

// ContextMenu.h


namespace sw
{
    class ContextMenu : public MenuBase
    {
    public:
        /**
         * @brief 初始化上下文菜单
         */
        ContextMenu();

        /**
         * @brief 初始化上下文菜单并设置菜单项
         */
        ContextMenu(std::initializer_list<MenuItem> items);

        /**
         * @brief    判断ID是否为上下文菜单项的ID
         * @param id 要判断的ID
         * @return   ID是否为上下文菜单项的ID
         */
        static bool IsContextMenuID(int id);

    protected:
        /**
         * @brief       根据索引获取ID
         * @param index 索引
         * @return      菜单项的ID
         */
        virtual int IndexToID(int index) override;

        /**
         * @brief    根据ID获取索引
         * @param id 菜单项的ID
         * @return   索引
         */
        virtual int IDToIndex(int id) override;
    };
}

// LayoutHost.h


namespace sw
{
    class LayoutHost : public ILayout
    {
    private:
        /**
         * @brief 关联的对象
         */
        ILayout *_associatedObj = nullptr;

    public:
        /**
         * @brief     设置关联的对象，每个LayoutHost只能关联一个对象
         * @param obj 要关联的对象
         */
        void Associate(ILayout *obj);

    public:
        /**
         * @brief 获取布局标记
         */
        virtual uint64_t GetLayoutTag() override;

        /**
         * @brief 获取关联对象子控件的数量
         */
        virtual int GetChildLayoutCount() override;

        /**
         * @brief 获取关联对象对应索引处的子控件
         */
        virtual ILayout &GetChildLayoutAt(int index) override;

        /**
         * @brief 获取关联对象所需尺寸
         */
        virtual Size GetDesireSize() override;

        /**
         * @brief 设置关联对象所需的尺寸
         */
        virtual void SetDesireSize(const Size &size) override;

        /**
         * @brief               测量控件所需尺寸
         * @param availableSize 可用的尺寸
         */
        virtual void Measure(const Size &availableSize) override;

        /**
         * @brief               安排控件位置
         * @param finalPosition 最终控件所安排的位置
         */
        virtual void Arrange(const Rect &finalPosition) override;

        /**
         * @brief 重写此函数计算所需尺寸
         */
        virtual void MeasureOverride(Size &availableSize) = 0;

        /**
         * @brief 重写此函数安排控件
         */
        virtual void ArrangeOverride(Size &finalSize) = 0;
    };
}

// Menu.h


namespace sw
{
    class Menu : public MenuBase
    {
    public:
        /**
         * @brief 初始化菜单
         */
        Menu();

        /**
         * @brief 初始化菜单并设置菜单项
         */
        Menu(std::initializer_list<MenuItem> items);

    protected:
        /**
         * @brief       根据索引获取ID
         * @param index 索引
         * @return      菜单项的ID
         */
        virtual int IndexToID(int index) override;

        /**
         * @brief    根据ID获取索引
         * @param id 菜单项的ID
         * @return   索引
         */
        virtual int IDToIndex(int id) override;
    };
}

// MsgBox.h


namespace sw
{
    /**
     * @brief 消息框回调
     */
    typedef std::function<void()> MsgBoxCallback;

    /**
     * @brief 消息框按钮类型
     */
    enum class MsgBoxButton {
        Ok          = MB_OK,
        OkCancel    = MB_OKCANCEL,
        YesNoCancel = MB_YESNOCANCEL,
        YesNo       = MB_YESNO,
    };

    /**
     * @brief 消息框结果
     */
    enum class MsgBoxResult {
        Ok     = IDOK,
        Cancel = IDCANCEL,
        Yes    = IDYES,
        No     = IDNO,
    };

    /**
     * @brief 消息框类
     */
    class MsgBox
    {
    public:
        MsgBoxResult result;

    private:
        MsgBox() = delete;
        MsgBox(MsgBoxResult result);

    public:
        static MsgBox Show(const WndBase *owner, const std::wstring &text = L"", const std::wstring &caption = L"", MsgBoxButton button = MsgBoxButton::Ok);
        static MsgBox Show(const WndBase &owner, const std::wstring &text = L"", const std::wstring &caption = L"", MsgBoxButton button = MsgBoxButton::Ok);

        static MsgBox ShowInfo(const WndBase *owner, const std::wstring &text = L"", const std::wstring &caption = L"", MsgBoxButton button = MsgBoxButton::Ok);
        static MsgBox ShowInfo(const WndBase &owner, const std::wstring &text = L"", const std::wstring &caption = L"", MsgBoxButton button = MsgBoxButton::Ok);

        static MsgBox ShowError(const WndBase *owner, const std::wstring &text = L"", const std::wstring &caption = L"", MsgBoxButton button = MsgBoxButton::Ok);
        static MsgBox ShowError(const WndBase &owner, const std::wstring &text = L"", const std::wstring &caption = L"", MsgBoxButton button = MsgBoxButton::Ok);

        static MsgBox ShowWarning(const WndBase *owner, const std::wstring &text = L"", const std::wstring &caption = L"", MsgBoxButton button = MsgBoxButton::Ok);
        static MsgBox ShowWarning(const WndBase &owner, const std::wstring &text = L"", const std::wstring &caption = L"", MsgBoxButton button = MsgBoxButton::Ok);

        static MsgBox ShowQuestion(const WndBase *owner, const std::wstring &text = L"", const std::wstring &caption = L"", MsgBoxButton button = MsgBoxButton::YesNo);
        static MsgBox ShowQuestion(const WndBase &owner, const std::wstring &text = L"", const std::wstring &caption = L"", MsgBoxButton button = MsgBoxButton::YesNo);

    private:
        const MsgBox &_On(MsgBoxResult result, const MsgBoxCallback &callback) const;

    public:
        const MsgBox &OnOk(const MsgBoxCallback &callback) const;
        const MsgBox &OnYes(const MsgBoxCallback &callback) const;
        const MsgBox &OnNo(const MsgBoxCallback &callback) const;
        const MsgBox &OnCancel(const MsgBoxCallback &callback) const;
    };
}

// DockLayout.h


namespace sw
{
    class DockLayout : public LayoutHost
    {
    public:
        /**
         * @brief Dock布局标记
         */
        enum DockLayoutTag {
            Left,   // 左边
            Top,    // 顶边
            Right,  // 右边
            Bottom, // 底边
        };

    public:
        /**
         * @brief 最后一个子元素是否填充剩余空间
         */
        bool lastChildFill = true;

        /**
         * @brief 计算所需尺寸
         */
        virtual void MeasureOverride(Size &availableSize) override;

        /**
         * @brief 安排控件
         */
        virtual void ArrangeOverride(Size &finalSize) override;
    };
}

// FillLayout.h


namespace sw
{
    class FillLayout : public LayoutHost
    {
    public:
        /**
         * @brief 计算所需尺寸
         */
        virtual void MeasureOverride(Size &availableSize) override;

        /**
         * @brief 安排控件
         */
        virtual void ArrangeOverride(Size &finalSize) override;
    };
}

// StackLayoutH.h


namespace sw
{
    class StackLayoutH : virtual public LayoutHost
    {
    public:
        /**
         * @brief 计算所需尺寸
         */
        virtual void MeasureOverride(Size &availableSize) override;

        /**
         * @brief 安排控件
         */
        virtual void ArrangeOverride(Size &finalSize) override;
    };
}

// StackLayoutV.h


namespace sw
{
    class StackLayoutV : virtual public LayoutHost
    {
    public:
        /**
         * @brief 计算所需尺寸
         */
        virtual void MeasureOverride(Size &availableSize) override;

        /**
         * @brief 安排控件
         */
        virtual void ArrangeOverride(Size &finalSize) override;
    };
}

// UIElement.h


namespace sw
{
    class UIElement : public WndBase, public ILayout, public ITag
    {
    private:
        /**
         * @brief 是否正在Arrange，当该字段为true时调用NotifyLayoutUpdated函数不会触发WM_UpdateLayout消息
         */
        bool _arranging = false;

        /**
         * @brief 是否在不可见时不参与布局
         */
        bool _collapseWhenHide = true;

        /**
         * @brief 当前元素所需要占用的尺寸
         */
        Size _desireSize = Size();

        /**
         * @brief 当对齐方式为拉伸时用该字段存储原始大小
         */
        Size _origionalSize = Size();

        /**
         * @brief 边距
         */
        Thickness _margin = Thickness();

        /**
         * @brief 水平对齐方式
         */
        HorizontalAlignment _horizontalAlignment = HorizontalAlignment::Center;

        /**
         * @brief 垂直对齐方式
         */
        VerticalAlignment _verticalAlignment = VerticalAlignment::Center;

        /**
         * @brief 指向父元素的指针，在调用SetParent后会更新该值
         */
        UIElement *_parent = nullptr;

        /**
         * @brief 所有子窗口
         */
        std::vector<UIElement *> _children{};

        /**
         * @brief 参与布局的子窗口，在调用GetChildLayoutCount后会更新，不可见且CollapseWhenHide为true的控件会被忽略
         */
        std::vector<UIElement *> _childrenNotCollapsed{};

        /**
         * @brief 记录路由事件的map
         */
        std::map<RoutedEventType, RoutedEvent> _eventMap{};

        /**
         * @brief 储存用户自定义信息
         */
        uint64_t _tag = 0;

        /**
         * @brief 布局标记
         */
        uint64_t _layoutTag = 0;

        /**
         * @brief 上下文菜单
         */
        sw::ContextMenu *_contextMenu = nullptr;

        /**
         * @brief Arrange时子元素的水平偏移量
         */
        double _arrangeOffsetX = 0;

        /**
         * @brief Arrange时子元素的垂直偏移量
         */
        double _arrangeOffsetY = 0;

        /**
         * @brief 所有子元素在当前元素中最右边的位置
         */
        double _childRightmost = 0;

        /**
         * @brief 所有子元素在当前元素中最底边的位置
         */
        double _childBottommost = 0;

        /**
         * @brief 元素是否悬浮，若元素悬浮则该元素不会随滚动条滚动而改变位置
         */
        bool _float = false;

        /**
         * @brief 表示用户是否可以通过按下Tab键将焦点移动到当前元素
         */
        bool _tabStop = false;

        /**
         * @brief 是否绘制虚线框
         */
        bool _drawFocusRect = false;

        /**
         * @brief 是否使用透明背景
         */
        bool _transparent = false;

    public:
        /**
         * @brief 边距
         */
        const Property<Thickness> Margin;

        /**
         * @brief 水平对齐方式
         */
        const Property<HorizontalAlignment> HorizontalAlignment;

        /**
         * @brief 垂直对齐方式
         */
        const Property<VerticalAlignment> VerticalAlignment;

        /**
         * @brief 子控件数量
         */
        const ReadOnlyProperty<int> ChildCount;

        /**
         * @brief 是否在不可见时不参与布局
         */
        const Property<bool> CollapseWhenHide;

        /**
         * @brief 指向父元素的指针，当前元素为顶级窗口时该值为nullptr
         */
        const ReadOnlyProperty<UIElement *> Parent;

        /**
         * @brief 储存用户自定义信息的标记
         */
        const Property<uint64_t> Tag;

        /**
         * @brief 布局标记，对于不同的布局有不同含义
         */
        const Property<uint64_t> LayoutTag;

        /**
         * @brief 右键按下时弹出的菜单
         */
        const Property<sw::ContextMenu *> ContextMenu;

        /**
         * @brief 元素是否悬浮，若元素悬浮则该元素不会随滚动条滚动而改变位置
         */
        const Property<bool> Float;

        /**
         * @brief 表示用户是否可以通过按下Tab键将焦点移动到当前元素
         */
        const Property<bool> TabStop;

        /**
         * @brief 是否使用透明背景（此属性并非真正意义上的透明，将该属性设为true可继承父元素的背景颜色）
         */
        const Property<bool> Transparent;

    public:
        UIElement();
        virtual ~UIElement();

        /**
         * @brief           注册路由事件处理函数，当事件已注册时会覆盖已注册的函数
         * @param eventType 路由事件类型
         * @param handler   处理函数，当值为nullptr时可取消注册
         */
        void RegisterRoutedEvent(RoutedEventType eventType, const RoutedEvent &handler);

        /**
         * @brief           注册成员函数作为路由事件处理函数，当事件已注册时会覆盖已注册的函数
         * @tparam T        成员函数所在的类
         * @param eventType 路由事件类型
         * @param obj       注册的成员函数所在的对象
         * @param handler   处理函数，当值为nullptr时可取消注册
         */
        template <typename T>
        void RegisterRoutedEvent(RoutedEventType eventType, T &obj, void (T::*handler)(UIElement &, RoutedEventArgs &))
        {
            if (handler == nullptr) {
                this->UnregisterRoutedEvent(eventType);
                return;
            }
            T *p = &obj;
            this->RegisterRoutedEvent(eventType, [p, handler](UIElement &sender, RoutedEventArgs &e) {
                (p->*handler)(sender, e);
            });
        }

        /**
         * @brief             根据事件参数类型注册路由事件
         * @tparam TEventArgs 路由事件的参数类型，必须继承自RoutedEventOfType<...>
         * @param handler     事件的处理函数，当值为nullptr时可取消注册
         */
        template <
            typename TEventArgs,
            typename std::enable_if<std::is_base_of<RoutedEventArgs, TEventArgs>::value, int>::type = 0,
            typename std::enable_if<sw::_IsTypedRoutedEventArgs<TEventArgs>::value, int>::type      = 0>
        void RegisterRoutedEvent(std::function<void(UIElement &, TEventArgs &)> handler)
        {
            if (!handler) {
                this->UnregisterRoutedEvent(TEventArgs::EventType);
                return;
            }
            this->RegisterRoutedEvent(TEventArgs::EventType, [handler](UIElement &sender, RoutedEventArgs &e) {
                handler(sender, static_cast<TEventArgs &>(e));
            });
        }

        /**
         * @brief             根据事件参数类型注册成员函数作为路由事件
         * @tparam TEventArgs 路由事件的参数类型，必须继承自RoutedEventOfType<...>
         * @tparam THandleObj 成员函数所在的类
         * @param obj         注册的成员函数所在的对象
         * @param handler     事件的处理函数，当值为nullptr时可取消注册
         */
        template <
            typename TEventArgs,
            typename THandleObj,
            typename std::enable_if<std::is_base_of<RoutedEventArgs, TEventArgs>::value, int>::type = 0,
            typename std::enable_if<sw::_IsTypedRoutedEventArgs<TEventArgs>::value, int>::type      = 0>
        void RegisterRoutedEvent(THandleObj &obj, void (THandleObj::*handler)(UIElement &, TEventArgs &))
        {
            if (handler == nullptr) {
                this->UnregisterRoutedEvent(TEventArgs::EventType);
                return;
            }
            THandleObj *p = &obj;
            this->RegisterRoutedEvent(TEventArgs::EventType, [p, handler](UIElement &sender, RoutedEventArgs &e) {
                (p->*handler)(sender, static_cast<TEventArgs &>(e));
            });
        }

        /**
         * @brief           取消对应类型路由事件的注册
         * @param eventType 路由事件类型
         */
        void UnregisterRoutedEvent(RoutedEventType eventType);

        /**
         * @brief           判断路由事件是否已被注册
         * @param eventType 路由事件类型
         */
        bool IsRoutedEventRegistered(RoutedEventType eventType);

        /**
         * @brief  添加子控件
         * @return 添加是否成功
         */
        bool AddChild(UIElement *element);

        /**
         * @brief  添加子控件
         * @return 添加是否成功
         */
        bool AddChild(UIElement &element);

        /**
         * @brief  添加子控件并设置布局标记
         * @return 添加是否成功
         */
        bool AddChild(UIElement *element, uint64_t layoutTag);

        /**
         * @brief  添加子控件并设置布局标记
         * @return 添加是否成功
         */
        bool AddChild(UIElement &element, uint64_t layoutTag);

        /**
         * @brief       移除指定索引处的子控件
         * @param index 要移除的索引
         * @return      移除是否成功
         */
        bool RemoveChildAt(int index);

        /**
         * @brief  移除子控件
         * @return 移除是否成功
         */
        bool RemoveChild(UIElement *element);

        /**
         * @brief  移除子控件
         * @return 移除是否成功
         */
        bool RemoveChild(UIElement &element);

        /**
         * @brief 移除所有子控件
         */
        void ClearChildren();

        /**
         * @brief         获取指定元素的索引
         * @param element 要查找的元素
         * @return        若找到指定元素则返回对应的索引，否则返回-1
         */
        int IndexOf(UIElement *element);

        /**
         * @brief         获取指定元素的索引
         * @param element 要查找的元素
         * @return        若找到指定元素则返回对应的索引，否则返回-1
         */
        int IndexOf(UIElement &element);

        /**
         * @brief 通过索引获取子控件
         */
        UIElement &operator[](int index) const;

        /**
         * @brief       弹出当前元素的上下文菜单
         * @param point 弹出菜单左上角在屏幕中的位置
         */
        void ShowContextMenu(const Point &point);

        /**
         * @brief 移动到界面顶部
         */
        void MoveToTop();

        /**
         * @brief 移动到界面底部
         */
        void MoveToBottom();

        /**
         * @brief 判断当前元素是否为根节点
         */
        bool IsRootElement();

        /**
         * @brief 获取当前元素所在界面树的根节点
         */
        UIElement *GetRootElement();

        /**
         * @brief 获取当前元素在界面树上的下一个节点，若已是最后一个节点则返回根节点
         */
        UIElement *GetNextElement();

        /**
         * @brief 获取下一个TabStop属性为true的元素
         */
        UIElement *GetNextTabStopElement();

        /**
         * @brief 获取当前要显示的背景颜色：当Transparent为true时获取到祖先节点中首个Transparent为false的背景颜色，否则返回当前元素的背景颜色
         */
        Color GetRealBackColor();

        /**
         * @brief 获取Tag
         */
        virtual uint64_t GetTag() override;

        /**
         * @brief 设置Tag
         */
        virtual void SetTag(uint64_t tag) override;

        /**
         * @brief 获取布局标记
         */
        virtual uint64_t GetLayoutTag() override;

        /**
         * @brief 获取参与布局的子控件数量
         */
        virtual int GetChildLayoutCount() override;

        /**
         * @brief 获取对应索引处的子控件，使用此函数前必须先调用GetChildLayoutCount
         */
        virtual ILayout &GetChildLayoutAt(int index) override;

        /**
         * @brief 获取控件所需尺寸
         */
        virtual Size GetDesireSize() override;

        /**
         * @brief 设置当前控件所需的尺寸
         */
        virtual void SetDesireSize(const Size &size) override;

        /**
         * @brief               测量控件所需尺寸
         * @param availableSize 可用的尺寸
         */
        virtual void Measure(const Size &availableSize) override;

        /**
         * @brief           安排控件位置
         * @param finalSize 最终控件所安排的位置
         */
        virtual void Arrange(const sw::Rect &finalPosition) override;

    protected:
        /**
         * @brief           触发路由事件
         * @param eventType 事件类型
         */
        void RaiseRoutedEvent(RoutedEventType eventType);

        /**
         * @brief           触发路由事件
         * @param eventArgs 要触发事件的事件参数
         */
        void RaiseRoutedEvent(RoutedEventArgs &eventArgs);

        /**
         * @brief 通知顶级窗口布局改变
         */
        void NotifyLayoutUpdated();

        /**
         * @brief 获取Arrange时子元素的水平偏移量
         */
        double &GetArrangeOffsetX();

        /**
         * @brief 获取Arrange时子元素的垂直偏移量
         */
        double &GetArrangeOffsetY();

        /**
         * @brief        获取所有子元素在当前元素中最右边的位置（只考虑参与布局的子窗口且忽略悬浮的元素）
         * @param update 是否更字段
         * @return       _childRightmost字段
         */
        double GetChildRightmost(bool update);

        /**
         * @brief        获取所有子元素在当前元素中最底边的位置（只考虑参与布局的子窗口且忽略悬浮的元素）
         * @param update 是否更字段
         * @return       _childBottommost字段
         */
        double GetChildBottommost(bool update);

        /**
         * @brief 更新子元素的Z轴位置
         */
        void UpdateChildrenZOrder();

        /**
         * @brief 更新兄弟元素的Z轴位置
         */
        void UpdateSiblingsZOrder();

        /**
         * @brief 设置下一个TabStop属性为true的元素为焦点元素
         */
        void SetNextTabStopFocus();

        /**
         * @brief         添加子元素后调用该函数
         * @param element 添加的子元素
         */
        virtual void OnAddedChild(UIElement &element);

        /**
         * @brief         移除子元素后调用该函数
         * @param element 移除的子元素
         */
        virtual void OnRemovedChild(UIElement &element);

        /**
         * @brief 通过tab键将焦点移动到当前元素时调用该函数
         */
        virtual void OnTabStop();

        /**
         * @brief 绘制虚线框时调用该函数
         */
        virtual void OnDrawFocusRect();

        /**
         * @brief  设置父窗口
         * @return 设置是否成功
         */
        virtual bool SetParent(WndBase *parent) override;

        /**
         * @brief           父窗口改变时调用此函数
         * @param newParent 新的父窗口
         */
        virtual void ParentChanged(WndBase *newParent) override;

        /**
         * @brief 在OnPaint函数完成之后调用该函数
         */
        virtual void OnEndPaint() override;

        /**
         * @brief  接收到WM_CLOSE时调用该函数
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnClose() override;

        /**
         * @brief                   接收到WM_MOVE时调用该函数
         * @param newClientPosition 移动后用户区左上角的位置
         * @return                  若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMove(Point newClientPosition) override;

        /**
         * @brief               接收到WM_SIZE时调用该函数
         * @param newClientSize 改变后的用户区尺寸
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnSize(Size newClientSize) override;

        /**
         * @brief Text属性更改时调用此函数
         */
        virtual void OnTextChanged() override;

        /**
         * @brief Visible属性改变时调用此函数
         */
        virtual void VisibleChanged(bool newVisible) override;

        /**
         * @brief            接收到WM_SETFOCUS时调用该函数
         * @param hPrevFocus 丢失焦点的hwnd，可能为NULL
         * @return           若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnSetFocus(HWND hPrevFocus) override;

        /**
         * @brief            接收到WM_KILLFOCUS时调用该函数
         * @param hNextFocus 接收到焦点的hwnd，可能为NULL
         * @return           若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnKillFocus(HWND hNextFocus) override;

        /**
         * @brief       接收到WM_CHAR时调用该函数
         * @param ch    按键的字符代码
         * @param flags 附加信息
         * @return      若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnChar(wchar_t ch, KeyFlags flags) override;

        /**
         * @brief       接收到WM_KEYDOWN时调用该函数
         * @param key   虚拟按键
         * @param flags 附加信息
         * @return      若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnKeyDown(VirtualKey key, KeyFlags flags) override;

        /**
         * @brief       接收到WM_KEYUP时调用该函数
         * @param key   虚拟按键
         * @param flags 附加信息
         * @return      若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnKeyUp(VirtualKey key, KeyFlags flags) override;

        /**
         * @brief               接收到WM_MOUSEMOVE时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState      指示某些按键是否按下
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseMove(Point mousePosition, MouseKey keyState) override;

        /**
         * @brief  接收到WM_MOUSELEAVE时调用该函数
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseLeave() override;

        /**
         * @brief               接收到WM_MOUSEWHEEL时调用该函数
         * @param wheelDelta    滚轮滚动的距离，为120的倍数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState      指示某些按键是否按下
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseWheel(int wheelDelta, Point mousePosition, MouseKey keyState) override;

        /**
         * @brief               接收到WM_LBUTTONDOWN时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState      指示某些按键是否按下
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseLeftButtonDown(Point mousePosition, MouseKey keyState) override;

        /**
         * @brief               接收到WM_LBUTTONUP时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState      指示某些按键是否按下
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseLeftButtonUp(Point mousePosition, MouseKey keyState) override;

        /**
         * @brief               接收到WM_RBUTTONDOWN时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState      指示某些按键是否按下
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseRightButtonDown(Point mousePosition, MouseKey keyState) override;

        /**
         * @brief               接收到WM_RBUTTONUP时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState      指示某些按键是否按下
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseRightButtonUp(Point mousePosition, MouseKey keyState) override;

        /**
         * @brief               接收到WM_MBUTTONDOWN时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState      指示某些按键是否按下
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseMiddleButtonDown(Point mousePosition, MouseKey keyState) override;

        /**
         * @brief               接收到WM_MBUTTONUP时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState      指示某些按键是否按下
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseMiddleButtonUp(Point mousePosition, MouseKey keyState) override;

        /**
         * @brief               接收到WM_CONTEXTMENU后调用目标控件的该函数
         * @param isKeyboardMsg 消息是否由按下快捷键（Shift+F10、VK_APPS）产生
         * @param mousePosition 鼠标在屏幕中的位置
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnContextMenu(bool isKeyboardMsg, Point mousePosition) override;

        /**
         * @brief    当WM_COMMAND接收到菜单命令时调用该函数
         * @param id 菜单id
         */
        virtual void OnMenuCommand(int id) override;

        /**
         * @brief           接收到WM_CTLCOLORxxx时调用该函数
         * @param hdc       控件的显示上下文句柄
         * @param hControl  控件的句柄
         * @param hRetBrush 要返回的画笔
         * @return          若返回true则将hRetBrush作为消息的返回值，否则使用DefaultWndProc的返回值
         */
        virtual bool OnCtlColor(HDC hdc, HWND hControl, HBRUSH &hRetBrush) override;

    private:
        /**
         * @brief 循环获取界面树上的下一个节点
         */
        static UIElement *_GetNextElement(UIElement *element, bool searchChildren = true);
    };
}

// WrapLayoutH.h


namespace sw
{
    class WrapLayoutH : virtual public LayoutHost
    {
    public:
        /**
         * @brief 计算所需尺寸
         */
        virtual void MeasureOverride(Size &availableSize) override;

        /**
         * @brief 安排控件
         */
        virtual void ArrangeOverride(Size &finalSize) override;
    };
}

// WrapLayoutV.h


namespace sw
{
    class WrapLayoutV : virtual public LayoutHost
    {
    public:
        /**
         * @brief 计算所需尺寸
         */
        virtual void MeasureOverride(Size &availableSize) override;

        /**
         * @brief 安排控件
         */
        virtual void ArrangeOverride(Size &finalSize) override;
    };
}

// Control.h


namespace sw
{
    /**
     * @brief 控件
     */
    class Control : virtual public UIElement
    {
    private:
        /**
         * @brief 是否使用默认的鼠标样式
         */
        bool _useDefaultCursor = true;

        /**
         * @brief 鼠标句柄
         */
        HCURSOR _hCursor = NULL;

    public:
        /**
         * @brief 初始化控件
         */
        Control();

    protected:
        /**
         * @brief 销毁控件句柄并重新初始化，该操作会创建新的句柄并设置样式、文本、字体等
         */
        void ResetHandle();

        /**
         * @brief 控件句柄发生改变时调用该函数
         */
        virtual void HandleChenged();

        /**
         * @brief         接收到WM_SETCURSOR消息时调用该函数
         * @param hwnd    鼠标所在窗口的句柄
         * @param hitTest hit-test的结果，详见WM_NCHITTEST消息的返回值
         * @param message 触发该事件的鼠标消息，如WM_MOUSEMOVE
         * @param result  消息的返回值，默认为false
         * @return        若返回true则将result作为消息的返回值，否则使用DefaultWndProc的返回值
         */
        virtual bool OnSetCursor(HWND hwnd, int hitTest, int message, bool &result) override;

    public:
        /**
         * @brief         设置鼠标样式
         * @param hCursor 鼠标句柄
         */
        void SetCursor(HCURSOR hCursor);

        /**
         * @brief        设置鼠标样式
         * @param cursor 鼠标样式
         */
        void SetCursor(StandardCursor cursor);

        /**
         * @brief 将鼠标样式设置为默认样式
         */
        void ResetCursor();
    };
}

// Layer.h


namespace sw
{
    class Layer : virtual public UIElement
    {
    private:
        /**
         * @brief 是否关闭布局，当该字段为true时调用UpdateLayout不会更新布局，可以用DisableLayout和EnableLayout设置该字段
         */
        bool _layoutDisabled = false;

        /**
         * @brief 指向所自定义的布局方式对象的指针
         */
        LayoutHost *_customLayout = nullptr;

        /**
         * @brief 记录水平滚动条是否已被禁止
         */
        bool _horizontalScrollDisabled = true;

        /**
         * @brief 记录垂直滚动条是否已被禁止
         */
        bool _verticalScrollDisabled = true;

    public:
        /**
         * @brief 自定义的布局方式，赋值后将自动与所指向的布局关联，每个布局只能关联一个对象，设为nullptr可恢复默认布局
         */
        const Property<LayoutHost *> Layout;

        /**
         * @brief 是否显示横向滚动条
         */
        const Property<bool> HorizontalScrollBar;

        /**
         * @brief 是否显示纵向滚动条
         */
        const Property<bool> VerticalScrollBar;

        /**
         * @brief 横向滚动条位置
         */
        const Property<double> HorizontalScrollPos;

        /**
         * @brief 纵向滚动条位置
         */
        const Property<double> VerticalScrollPos;

        /**
         * @brief 横向滚动条可设置的最大位置
         */
        const ReadOnlyProperty<double> HorizontalScrollLimit;

        /**
         * @brief 纵向滚动条可设置的最大位置
         */
        const ReadOnlyProperty<double> VerticalScrollLimit;

    public:
        /**
         * @brief 初始化Layer
         */
        Layer();

    private:
        /**
         * @brief 获取布局对象，若Layout属性被赋值则返回设置的对象，否则返回默认布局对象
         */
        LayoutHost *_GetLayout();

        /**
         * @brief 在没有设定布局方式时，使用该函数对子元素Measure和Arrange
         */
        void _MeasureAndArrangeWithoutLayout();

    protected:
        /**
         * @brief 更新布局
         */
        void UpdateLayout();

        /**
         * @brief 获取默认布局对象
         */
        virtual LayoutHost *GetDefaultLayout();

        /**
         * @brief           触发滚动条相关事件时调用该函数
         * @param scrollbar 滚动条类型
         * @param event     滚动条事件
         * @param pos       当event为ThumbPosition或ThubmTrack时表示当前滚动条位置，其他情况固定为0
         */
        virtual void OnScroll(ScrollOrientation scrollbar, ScrollEvent event, double pos);

        /**
         * @brief       接收到WM_VSCROLL时调用目标控件的该函数
         * @param event 事件类型，即消息wParam的低字
         * @param pos   当前滚动条的位置，仅当event为SB_THUMBPOSITION或SB_THUMBTRACK时有效
         * @return      若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnVerticalScroll(int event, int pos) override;

        /**
         * @brief       接收到WM_HSCROLL时调用目标控件的该函数
         * @param event 事件类型，即消息wParam的低字
         * @param pos   当前滚动条的位置，仅当event为SB_THUMBPOSITION或SB_THUMBTRACK时有效
         * @return      若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnHorizontalScroll(int event, int pos) override;

    public:
        /**
         * @brief               测量控件所需尺寸
         * @param availableSize 可用的尺寸
         */
        virtual void Measure(const Size &availableSize) override;

        /**
         * @brief           安排控件位置
         * @param finalSize 最终控件所安排的位置
         */
        virtual void Arrange(const sw::Rect &finalPosition) override;

        /**
         * @brief 禁用布局
         */
        void DisableLayout();

        /**
         * @brief 启用布局
         */
        void EnableLayout();

        /**
         * @brief        获取横向滚动条的范围
         * @param refMin 滚动范围最小值
         * @param refMax 滚动范围最大值
         */
        void GetHorizontalScrollRange(double &refMin, double &refMax);

        /**
         * @brief        获取纵向滚动条的范围
         * @param refMin 滚动范围最小值
         * @param refMax 滚动范围最大值
         */
        void GetVerticalScrollRange(double &refMin, double &refMax);

        /**
         * @brief     设置横向滚动条的范围
         * @param min 滚动范围最小值
         * @param max 滚动范围最大值
         */
        void SetHorizontalScrollRange(double min, double max);

        /**
         * @brief     设置纵向滚动条的范围
         * @param min 滚动范围最小值
         * @param max 滚动范围最大值
         */
        void SetVerticalScrollRange(double min, double max);

        /**
         * @brief 获取水平滚动条滚动页面大小
         */
        double GetHorizontalScrollPageSize();

        /**
         * @brief 获取垂直滚动条滚动页面大小
         */
        double GetVerticalScrollPageSize();

        /**
         * @brief 设置水平滚动条滚动页面大小
         */
        void SetHorizontalScrollPageSize(double pageSize);

        /**
         * @brief 设置垂直滚动条滚动页面大小
         */
        void SetVerticalScrollPageSize(double pageSize);

        /**
         * @brief 根据子元素更新滚动条范围，未设定布局方式时该函数无效
         */
        void UpdateScrollRange();

        /**
         * @brief 将垂直滚动条移动到顶部
         */
        void ScrollToTop();

        /**
         * @brief 将垂直滚动条移动到底部
         */
        void ScrollToBottom();

        /**
         * @brief 将水平滚动条移动到最左
         */
        void ScrollToLeft();

        /**
         * @brief 将水平滚动条移动到最右
         */
        void ScrollToRight();

        /**
         * @brief        水平滚动
         * @param offset 滚动的偏移量
         */
        void ScrollHorizontal(double offset);

        /**
         * @brief        垂直滚动
         * @param offset 滚动的偏移量
         */
        void ScrollVertical(double offset);
    };
}

// StackLayout.h


namespace sw
{
    class StackLayout : public StackLayoutH,
                        public StackLayoutV
    {
    public:
        /**
         * @brief 排列方式
         */
        Orientation orientation = Orientation::Vertical;

        /**
         * @brief 计算所需尺寸
         */
        virtual void MeasureOverride(Size &availableSize) override;

        /**
         * @brief 安排控件
         */
        virtual void ArrangeOverride(Size &finalSize) override;
    };
}

// WrapLayout.h


namespace sw
{
    class WrapLayout : public WrapLayoutH,
                       public WrapLayoutV
    {
    public:
        /**
         * @brief 排列方式
         */
        Orientation orientation = Orientation::Horizontal;

        /**
         * @brief 计算所需尺寸
         */
        virtual void MeasureOverride(Size &availableSize) override;

        /**
         * @brief 安排控件
         */
        virtual void ArrangeOverride(Size &finalSize) override;
    };
}

// ButtonBase.h


namespace sw
{
    class ButtonBase : public Control
    {
    protected:
        /**
         * @brief 初始化ButtonBase
         */
        ButtonBase();

        /**
         * @brief 初始化控件
         */
        void InitButtonBase(LPCWSTR lpWindowName, DWORD dwStyle, DWORD dwExStyle);

        /**
         * @brief      当父窗口接收到控件的WM_COMMAND时调用该函数
         * @param code 通知代码
         */
        virtual void OnCommand(int code) override;

        /**
         * @brief 被单击时调用该函数
         */
        virtual void OnClicked();

        /**
         * @brief 被双击时调用该函数
         */
        virtual void OnDoubleClicked();
    };
}

// ItemsControl.h


namespace sw
{
    template <typename TItem>
    class ItemsControl; // 向前声明

    /**
     * @brief 表示可用于呈现一组字符串的控件
     */
    typedef ItemsControl<std::wstring> StrItemsControl;

    /**
     * @brief 表示可用于呈现一组项的控件
     */
    template <typename TItem>
    class ItemsControl : public Control
    {
    public:
        /**
         * @brief 项数
         */
        const ReadOnlyProperty<int> ItemsCount = ReadOnlyProperty<int>(
            // get
            [&]() -> const int & {
                static int result;
                result = this->GetItemsCount();
                return result;
            });

        /**
         * @brief 选中项的索引，当无选中项时为-1
         */
        const Property<int> SelectedIndex = Property<int>(
            // get
            [&]() -> const int & {
                static int result;
                result = this->GetSelectedIndex();
                return result;
            },
            // set
            [&](const int &value) {
                this->SetSelectedIndex(value);
            });

        /**
         * @brief 选中项
         */
        const ReadOnlyProperty<TItem> SelectedItem = ReadOnlyProperty<TItem>(
            // get
            [&]() -> const TItem & {
                static TItem result;
                result = this->GetSelectedItem();
                return result;
            });

    protected:
        /**
         * @brief 选中项改变时调用该函数
         */
        virtual void OnSelectionChanged()
        {
            this->RaiseRoutedEvent(ItemsControl_SelectionChanged);
        }

    protected:
        /**
         * @brief  获取子项数
         */
        virtual int GetItemsCount() = 0;

        /**
         * @brief 选中项的索引，当无选中项时为-1
         */
        virtual int GetSelectedIndex() = 0;

        /**
         * @brief 设置选中项索引
         */
        virtual void SetSelectedIndex(int index) = 0;

        /**
         * @brief 获取选中项
         */
        virtual TItem GetSelectedItem() = 0;

    public:
        /**
         * @brief 清空所有子项
         */
        virtual void Clear() = 0;

        /**
         * @brief       获取指定索引处子项的值
         * @param index 子项的索引
         */
        virtual TItem GetItemAt(int index) = 0;

        /**
         * @brief      添加新的子项
         * @param item 要添加的子项
         * @return     是否添加成功
         */
        virtual bool AddItem(const TItem &item) = 0;

        /**
         * @brief       添加子项到指定索引
         * @param index 要插入的位置
         * @param item  要添加的子项
         * @return      是否添加成功
         */
        virtual bool InsertItem(int index, const TItem &item) = 0;

        /**
         * @brief          更新指定位置的子项
         * @param index    要更新子项的位置
         * @param newValue 子项的新值
         * @return         操作是否成功
         */
        virtual bool UpdateItem(int index, const TItem &newValue) = 0;

        /**
         * @brief       移除指定索引处的子项
         * @param index 要移除子项的索引
         * @return      操作是否成功
         */
        virtual bool RemoveItemAt(int index) = 0;
    };
}

// PanelBase.h


namespace sw
{
    class PanelBase : public Control, public Layer
    {
    protected:
        /**
         * @brief 初始化PanelBase
         */
        PanelBase();

    protected:
        /**
         * @brief         接收到WM_SETCURSOR消息时调用该函数
         * @param hwnd    鼠标所在窗口的句柄
         * @param hitTest hit-test的结果，详见WM_NCHITTEST消息的返回值
         * @param message 触发该事件的鼠标消息，如WM_MOUSEMOVE
         * @param result  消息的返回值，默认为false
         * @return        若返回true则将result作为消息的返回值，否则使用DefaultWndProc的返回值
         */
        virtual bool OnSetCursor(HWND hwnd, int hitTest, int message, bool &result) override;

        /**
         * @brief       接收到WM_VSCROLL时调用目标控件的该函数
         * @param event 事件类型，即消息wParam的低字
         * @param pos   当前滚动条的位置，仅当event为SB_THUMBPOSITION或SB_THUMBTRACK时有效
         * @return      若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnVerticalScroll(int event, int pos) override;

        /**
         * @brief       接收到WM_HSCROLL时调用目标控件的该函数
         * @param event 事件类型，即消息wParam的低字
         * @param pos   当前滚动条的位置，仅当event为SB_THUMBPOSITION或SB_THUMBTRACK时有效
         * @return      若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnHorizontalScroll(int event, int pos) override;

    public:
        /**
         * @brief               测量控件所需尺寸
         * @param availableSize 可用的尺寸
         */
        virtual void Measure(const Size &availableSize) override;

        /**
         * @brief           安排控件位置
         * @param finalSize 最终控件所安排的位置
         */
        virtual void Arrange(const sw::Rect &finalPosition) override;
    };
}

// ProgressBar.h


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

// Slider.h


namespace sw
{
    /**
     * @brief 滑块控件
     */
    class Slider : public Control
    {
    public:
        /**
         * @brief 最小值
         */
        const Property<int> Minimum;

        /**
         * @brief 最大值
         */
        const Property<int> Maximum;

        /**
         * @brief 当前滑块的值
         */
        const Property<int> Value;

        /**
         * @brief 是否显示为垂直滑块
         */
        const Property<bool> Vertical;

        /**
         * @brief 是否在滑动时显示数值提示
         */
        const Property<bool> ValueTooltips;

    public:
        /**
         * @brief 初始化滑块控件
         */
        Slider();

    protected:
        /**
         * @brief       接收到WM_VSCROLL时调用目标控件的该函数
         * @param event 事件类型，即消息wParam的低字
         * @param pos   当前滚动条的位置，仅当event为SB_THUMBPOSITION或SB_THUMBTRACK时有效
         * @return      若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnVerticalScroll(int event, int pos) override;

        /**
         * @brief       接收到WM_HSCROLL时调用目标控件的该函数
         * @param event 事件类型，即消息wParam的低字
         * @param pos   当前滚动条的位置，仅当event为SB_THUMBPOSITION或SB_THUMBTRACK时有效
         * @return      若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnHorizontalScroll(int event, int pos) override;

        /**
         * @brief 滑块的值改变时调用该函数
         */
        virtual void OnValueChanged();

        /**
         * @brief 释放滑块时调用该函数
         */
        virtual void OnEndTrack();
    };
}

// StaticControl.h


namespace sw
{
    /**
     * @brief 静态控件
     */
    class StaticControl : public Control
    {
    public:
        /**
         * @brief 初始化静态控件
         */
        StaticControl();
    };
}

// TabControl.h


namespace sw
{
    /**
     * @brief TabControl标签的位置
     */
    enum class TabAlignment {
        Top,    // 顶部
        Bottom, // 底部
        Left,   // 左边
        Right   // 右边
    };

    /**
     * @brief 标签页控件
     */
    class TabControl : public Control
    {
    public:
        /**
         * @brief 内容区域位置与尺寸
         */
        const ReadOnlyProperty<sw::Rect> ContentRect;

        /**
         * @brief 当前页面的索引
         */
        const Property<int> SelectedIndex;

        /**
         * @brief 标签的位置
         */
        const Property<TabAlignment> Alignment;

        /**
         * @brief 是否开启多行标签
         */
        const Property<bool> MultiLine;

    public:
        /**
         * @brief 初始化标签页控件
         */
        TabControl();

        /**
         * @brief 获取标签项的数量
         */
        int GetTabCount();

        /**
         * @brief 更新标签项信息
         */
        void UpdateTab();

        /**
         * @brief       更新指定索引处页面项的文本
         * @param index 要更新的索引
         */
        void UpdateTabText(int index);

        /**
         * @brief               安排控件位置
         * @param finalPosition 最终控件所安排的位置
         */
        virtual void Arrange(const sw::Rect &finalPosition) override;

    protected:
        /**
         * @brief         添加子元素后调用该函数
         * @param element 添加的子元素
         */
        virtual void OnAddedChild(UIElement &element) override;

        /**
         * @brief         移除子元素后调用该函数
         * @param element 移除的子元素
         */
        virtual void OnRemovedChild(UIElement &element) override;

        /**
         * @brief 父窗口接收到WM_NOTIFY后调用发出通知控件的该函数
         */
        virtual void OnNotified(NMHDR *pNMHDR) override;

        /**
         * @brief SelectedIndex属性更改时调用该函数
         */
        virtual void OnSelectedIndexChanged();

    private:
        /**
         * @brief 根据选中的tab更新子元素的Visible属性
         */
        void _UpdateChildVisible();

        /**
         * @brief 发送TCM_INSERTITEMW消息
         */
        int _InsertItem(int index, TCITEMW &item);

        /**
         * @brief 发送TCM_SETITEMW消息
         */
        bool _SetItem(int index, TCITEMW &item);

        /**
         * @brief 发送TCM_DELETEITEM消息
         */
        bool _DeleteItem(int index);

        /**
         * @brief 发送TCM_DELETEALLITEMS消息
         */
        bool _DeleteAllItems();
    };
}

// TextBoxBase.h


namespace sw
{
    class TextBoxBase : public Control
    {
    private:
        /**
         * @brief 在读取Text属性时用于判断是否需要更新储存的文本
         */
        bool _isTextChanged = false;

        /**
         * @brief 是否允许输入制表符
         */
        bool _acceptTab = false;

    public:
        /**
         * @brief 是否只读
         */
        const Property<bool> ReadOnly;

        /**
         * @brief 文本的水平对齐方式，可设为左对齐、中心对齐、右对齐
         */
        const Property<sw::HorizontalAlignment> HorizontalContentAlignment;

        /**
         * @brief 是否可以撤销
         */
        const ReadOnlyProperty<bool> CanUndo;

        /**
         * @brief 是否允许输入制表符
         */
        const Property<bool> AcceptTab;

    protected:
        /**
         * @brief 初始化TextBoxBase
         */
        TextBoxBase();

        /**
         * @brief 初始化控件
         */
        void InitTextBoxBase(DWORD dwStyle, DWORD dwExStyle);

        /**
         * @brief  获取窗口文本
         * @return 编辑框的文本内容
         */
        virtual std::wstring &GetText() override;

        /**
         * @brief      当父窗口接收到控件的WM_COMMAND时调用该函数
         * @param code 通知代码
         */
        virtual void OnCommand(int code) override;

        /**
         * @brief       接收到WM_CHAR时调用该函数
         * @param ch    按键的字符代码
         * @param flags 附加信息
         * @return      若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnChar(wchar_t ch, KeyFlags flags) override;

        /**
         * @brief       接收到WM_KEYDOWN时调用该函数
         * @param key   虚拟按键
         * @param flags 附加信息
         * @return      若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnKeyDown(VirtualKey key, KeyFlags flags) override;

        /**
         * @brief 绘制虚线框时调用该函数
         */
        virtual void OnDrawFocusRect() override;

    public:
        /**
         * @brief        选择指定文本内容
         * @param start  起始位置
         * @param length 选择文本的长度
         */
        void Select(int start, int length);

        /**
         * @brief 选中所有文本
         */
        void SelectAll();

        /**
         * @brief 将控件内容滚动到当前插入符号位置
         */
        void ScrollToCaret();

        /**
         * @brief  撤销
         * @return 操作是否成功
         */
        bool Undo();

        /**
         * @brief 清空内容
         */
        void Clear();
    };
}

// Window.h


namespace sw
{
    /**
     * @brief 窗口状态
     */
    enum class WindowState {
        Normal,    // 普通窗口
        Minimized, // 最小化窗口
        Maximized, // 最大化窗口
    };

    /**
     * @brief 窗口启动位置
     */
    enum class WindowStartupLocation {
        Manual,       // 使用系统默认或手动设置
        CenterScreen, // 屏幕中心
        CenterOwner,  // 所有者窗口中心，只在ShowDialog时有效
    };

    /**
     * @brief 窗口
     */
    class Window : public Layer
    {
    private:
        /**
         * @brief 是否为第一次显示，该字段使StartupLocation生效
         */
        bool _isFirstShow = true;

        /**
         * @brief 窗口的尺寸限制，当值不大于0时表示不限制
         */
        double _maxWidth = -1, _maxHeight = -1, _minWidth = -1, _minHeight = -1;

        /**
         * @brief 窗口初次启动的位置
         */
        WindowStartupLocation _startupLocation = WindowStartupLocation::Manual;

        /**
         * @brief 鼠标句柄，窗口初始化时会赋值
         */
        HCURSOR _hCursor;

        /**
         * @brief 以模态窗口显示时保存所有者窗口，非模态时该值始终为nullptr
         */
        Window *_modalOwner = nullptr;

        /**
         * @brief 窗口顶部菜单
         */
        sw::Menu *_menu = nullptr;

        /**
         * @brief 窗口在失去焦点前保存句柄，用于窗口重新获得焦点时保留原先焦点控件的焦点
         */
        HWND _hPrevFocused = NULL;

        /**
         * @brief 窗口的默认布局方式
         */
        std::shared_ptr<LayoutHost> _layout;

    public:
        /**
         * @brief 程序的当前活动窗体
         */
        static const ReadOnlyProperty<Window *> ActiveWindow;

        /**
         * @brief 当前已创建的窗口数
         */
        static const ReadOnlyProperty<int> WindowCount;

        /**
         * @brief 窗口初次启动的位置
         */
        const Property<WindowStartupLocation> StartupLocation;

        /**
         * @brief 窗口状态
         */
        const Property<WindowState> State;

        /**
         * @brief 窗口是否可调整大小
         */
        const Property<bool> SizeBox;

        /**
         * @brief 最大化按钮是否可用
         */
        const Property<bool> MaximizeBox;

        /**
         * @brief 最小化按钮是否可用
         */
        const Property<bool> MinimizeBox;

        /**
         * @brief 窗口是否置顶
         */
        const Property<bool> Topmost;

        /**
         * @brief 是否显示为ToolWindow (窄边框)
         */
        const Property<bool> ToolWindow;

        /**
         * @brief 最大宽度，当值不大于0时表示不限制
         */
        const Property<double> MaxWidth;

        /**
         * @brief 最大高度，当值不大于0时表示不限制
         */
        const Property<double> MaxHeight;

        /**
         * @brief 最小宽度，当值不大于0时表示不限制
         */
        const Property<double> MinWidth;

        /**
         * @brief 最小高度，当值不大于0时表示不限制
         */
        const Property<double> MinHeight;

        /**
         * @brief 窗口顶部的菜单栏
         */
        const Property<sw::Menu *> Menu;

    public:
        /**
         * @brief 初始化窗口
         */
        Window();

    protected:
        /**
         * @brief 对WndProc的封装
         */
        virtual LRESULT WndProc(const ProcMsg &refMsg) override;

        /**
         * @brief 获取默认布局对象
         */
        virtual LayoutHost *GetDefaultLayout() override;

        /**
         * @brief  接收到WM_CLOSE时调用该函数
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnClose() override;

        /**
         * @brief  接收到WM_DESTROY时调用该函数
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnDestroy() override;

        /**
         * @brief  接收到WM_PAINT时调用该函数
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnPaint() override;

        /**
         * @brief               接收到WM_MOUSEMOVE时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState      指示某些按键是否按下
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseMove(Point mousePosition, MouseKey keyState) override;

        /**
         * @brief    当OnCommand接收到菜单命令时调用该函数
         * @param id 菜单id
         */
        virtual void OnMenuCommand(int id) override;

        /**
         * @brief 窗口第一次显示时调用该函数
         */
        virtual void OnFirstShow();

        /**
         * @brief 窗口成为前台窗口时调用该函数
         */
        virtual void OnActived();

        /**
         * @brief 窗口成为后台窗口时调用该函数
         */
        virtual void OnInactived();

    public:
        /**
         * @brief 显示窗口
         */
        void Show();

        /**
         * @brief       将窗体显示为模式对话框
         * @param owner 窗体的所有者，窗体显示期间该窗体的Enabled属性将被设为false，该参数不能设为自己
         */
        void ShowDialog(Window &owner);

        /**
         * @brief         设置鼠标样式
         * @param hCursor 鼠标句柄
         */
        void SetCursor(HCURSOR hCursor);

        /**
         * @brief        设置鼠标样式
         * @param cursor 鼠标样式
         */
        void SetCursor(StandardCursor cursor);

        /**
         * @brief       设置图标
         * @param hIcon 图标句柄
         */
        void SetIcon(HICON hIcon);

        /**
         * @brief 重回窗口的菜单栏
         */
        void DrawMenuBar();

        /**
         * @brief  窗口是否显示为模态窗口
         * @return 当调用ShowDialog时该函数返回true，否则返回false
         */
        bool IsModal();

        /**
         * @brief 设置窗口的默认布局方式
         */
        template <
            typename TLayout,
            typename std::enable_if<std::is_base_of<LayoutHost, TLayout>::value, int>::type = 0>
        void SetLayout()
        {
            auto layout = std::make_shared<TLayout>();
            layout->Associate(this);
            this->_layout = layout;
            this->NotifyLayoutUpdated();
        }

        /**
         * @brief 取消通过SetLayout设置的布局方式
         */
        template <std::nullptr_t>
        void SetLayout()
        {
            this->_layout = nullptr;
            this->NotifyLayoutUpdated();
        }
    };
}

// Button.h


namespace sw
{
    /**
     * @brief 按钮
     */
    class Button : public ButtonBase
    {
    public:
        /**
         * @brief 初始化按钮
         */
        Button();

    protected:
        /**
         * @brief 绘制虚线框时调用该函数
         */
        virtual void OnDrawFocusRect() override;

        /**
         * @brief           接收到WM_SETFOCUS时调用该函数
         * @param hPreFocus 丢失焦点的hwnd，可能为NULL
         * @return          若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnSetFocus(HWND hPreFocus) override;

        /**
         * @brief            接收到WM_KILLFOCUS时调用该函数
         * @param hNextFocus 接收到焦点的hwnd，可能为NULL
         * @return           若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnKillFocus(HWND hNextFocus) override;

        /**
         * @brief       接收到WM_KEYDOWN时调用该函数
         * @param key   虚拟按键
         * @param flags 附加信息
         * @return      若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnKeyDown(VirtualKey key, KeyFlags flags) override;
    };
}

// CheckableButton.h


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
    };
}

// ComboBox.h


namespace sw
{
    /**
     * @brief 组合框
     */
    class ComboBox : public StrItemsControl
    {
    private:
        /**
         * @brief 在读取Text属性时用于判断是否需要更新储存的文本
         */
        bool _isTextChanged = false;

    public:
        /**
         * @brief 组合框内容是否可编辑，更新该属性会导致已添加的子项被清空
         */
        const Property<bool> IsEditable;

    public:
        /**
         * @brief 初始化组合框
         */
        ComboBox();

    protected:
        /**
         * @brief  获取子项数
         */
        virtual int GetItemsCount() override;

        /**
         * @brief 选中项的索引，当无选中项时为-1
         */
        virtual int GetSelectedIndex() override;

        /**
         * @brief 设置选中项索引
         */
        virtual void SetSelectedIndex(int index) override;

        /**
         * @brief 获取选中项
         */
        virtual std::wstring GetSelectedItem() override;

        /**
         * @brief  获取可编辑状态下的编辑框文本内容
         */
        virtual std::wstring &GetText() override;

        /**
         * @brief       设置Text属性时调用该函数
         * @param value 要设置的文本
         */
        virtual void SetText(const std::wstring &value) override;

        /**
         * @brief      当父窗口接收到控件的WM_COMMAND时调用该函数
         * @param code 通知代码
         */
        virtual void OnCommand(int code) override;

        /**
         * @brief 选中项改变时调用该函数
         */
        virtual void OnSelectionChanged() override;

    public:
        /**
         * @brief 清空所有子项
         */
        virtual void Clear() override;

        /**
         * @brief       获取指定索引处子项的值
         * @param index 子项的索引
         */
        virtual std::wstring GetItemAt(int index) override;

        /**
         * @brief      添加新的子项
         * @param item 要添加的子项
         * @return     是否添加成功
         */
        virtual bool AddItem(const std::wstring &item) override;

        /**
         * @brief       添加子项到指定索引
         * @param index 要插入的位置
         * @param item  要添加的子项
         * @return      是否添加成功
         */
        virtual bool InsertItem(int index, const std::wstring &item) override;

        /**
         * @brief          更新指定位置的子项
         * @param index    要更新子项的位置
         * @param newValue 子项的新值
         * @return         操作是否成功
         */
        virtual bool UpdateItem(int index, const std::wstring &newValue) override;

        /**
         * @brief       移除指定索引处的子项
         * @param index 要移除子项的索引
         * @return      操作是否成功
         */
        virtual bool RemoveItemAt(int index) override;

        /**
         * @brief 显示下拉列表
         */
        void ShowDropDown();

        /**
         * @brief 关闭下拉列表
         */
        void CloseDropDown();
    };
}

// GroupBox.h


namespace sw
{
    /**
     * @brief 组合框
     */
    class GroupBox : public PanelBase
    {
    public:
        /**
         * @brief 初始化组合框
         */
        GroupBox();
    };
}

// Label.h


namespace sw
{
    /**
     * @brief 文本过长时末尾的处理方式
     */
    enum class TextTrimming {
        None,         // 不处理
        EndEllipsis,  // 按字符截断并显示“...”
        WordEllipsis, // 按单词截断并显示“...”
    };

    /**
     * @brief 标签
     */
    class Label : public StaticControl
    {
    private:
        /**
         * @brief 文本所需的尺寸
         */
        Size _textSize;

        /**
         * @brief 是否根据文本自动调整尺寸
         */
        bool _autoSize = true;

    public:
        /**
         * @brief 文本的水平对齐方式，可设为左对齐、中心对齐、右对齐
         */
        const Property<sw::HorizontalAlignment> HorizontalContentAlignment;

        /**
         * @brief 文本的垂直对齐方式，仅支持顶部对齐和中心对齐，当使用中心对齐时会使自动换行失效
         */
        const Property<sw::VerticalAlignment> VerticalContentAlignment;

        /**
         * @brief 文本过长时末尾的处理方式，当使用截断时水平对齐和自动换行将失效
         */
        const Property<sw::TextTrimming> TextTrimming;

        /**
         * @brief 是否自动换行
         */
        const Property<bool> AutoWrap;

        /**
         * @brief 是否根据文本自动调整尺寸
         */
        const Property<bool> AutoSize;

    public:
        /**
         * @brief 初始化标签
         */
        Label();

    private:
        /**
         * @brief 更新_textSize
         */
        void _UpdateTextSize();

        /**
         * @brief 调整尺寸为_textSize
         */
        void _ResizeToTextSize();

    protected:
        /**
         * @brief Text属性更改时调用此函数
         */
        virtual void OnTextChanged() override;

        /**
         * @brief       字体改变时调用该函数
         * @param hfont 字体句柄
         */
        virtual void FontChanged(HFONT hfont) override;

        /**
         * @brief               测量控件所需尺寸
         * @param availableSize 可用的尺寸
         */
        virtual void Measure(const Size &availableSize) override;
    };
}

// ListBox.h


namespace sw
{
    /**
     * @brief 列表框
     */
    class ListBox : public StrItemsControl
    {
    public:
        /**
         * @brief 当前列表框页面第一个子项的索引
         */
        const Property<int> TopIndex;

        /**
         * @brief 是否允许多选，更新该属性会导致已添加的子项被清空
         */
        const Property<bool> MultiSelect;

        /**
         * @brief 多选状态下可通过该属性获取选中项的个数
         */
        const ReadOnlyProperty<int> SelectedCount;

    public:
        /**
         * @brief 初始化列表框
         */
        ListBox();

    protected:
        /**
         * @brief  获取子项数
         */
        virtual int GetItemsCount() override;

        /**
         * @brief 选中项的索引，当无选中项时为-1
         */
        virtual int GetSelectedIndex() override;

        /**
         * @brief 设置选中项索引
         */
        virtual void SetSelectedIndex(int index) override;

        /**
         * @brief 获取选中项
         */
        virtual std::wstring GetSelectedItem() override;

        /**
         * @brief               接收到WM_CONTEXTMENU后调用目标控件的该函数
         * @param isKeyboardMsg 消息是否由按下快捷键（Shift+F10、VK_APPS）产生
         * @param mousePosition 鼠标在屏幕中的位置
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnContextMenu(bool isKeyboardMsg, Point mousePosition) override;

        /**
         * @brief      当父窗口接收到控件的WM_COMMAND时调用该函数
         * @param code 通知代码
         */
        virtual void OnCommand(int code) override;

    public:
        /**
         * @brief 清空所有子项
         */
        virtual void Clear() override;

        /**
         * @brief       获取指定索引处子项的值
         * @param index 子项的索引
         */
        virtual std::wstring GetItemAt(int index) override;

        /**
         * @brief      添加新的子项
         * @param item 要添加的子项
         * @return     是否添加成功
         */
        virtual bool AddItem(const std::wstring &item) override;

        /**
         * @brief       添加子项到指定索引
         * @param index 要插入的位置
         * @param item  要添加的子项
         * @return      是否添加成功
         */
        virtual bool InsertItem(int index, const std::wstring &item) override;

        /**
         * @brief          更新指定位置的子项
         * @param index    要更新子项的位置
         * @param newValue 子项的新值
         * @return         操作是否成功
         */
        virtual bool UpdateItem(int index, const std::wstring &newValue) override;

        /**
         * @brief       移除指定索引处的子项
         * @param index 要移除子项的索引
         * @return      操作是否成功
         */
        virtual bool RemoveItemAt(int index) override;

        /**
         * @brief       获取指定点处子项的索引
         * @param point 相对于列表框用户区左上角点的位置
         */
        int GetItemIndexFromPoint(const Point &point);

        /**
         * @brief  多选状态下可通过该函数获取所有选中项的索引
         * @return 所有选中项的索引
         */
        List<int> GetSelectedIndices();

        /**
         * @brief  多选状态下可通过该函数获取所有选中项的内容
         * @return 所有选中项的内容
         */
        StrList GetSelectedItems();

        /**
         * @brief       获取指定索引处子项的选中状态
         * @param index 子项的索引
         * @return      若子项选中则返回true，否则返回false
         */
        bool GetItemSelectionState(int index);

        /**
         * @brief       多选状态下设置指定索引处子项的选中状态
         * @param index 子项的索引，输入-1可设置所有子项的选中状态
         * @param value 要设置的子项状态
         */
        void SetItemSelectionState(int index, bool value);
    };
}

// ListView.h


namespace sw
{
    /**
     * @brief 列表视图的列对齐方式
     */
    enum class ListViewColumnAlignment {
        Left   = LVCFMT_LEFT,   // 左对齐
        Right  = LVCFMT_RIGHT,  // 右对齐
        Center = LVCFMT_CENTER, // 居中
    };

    /**
     * @brief 列表视图的列信息
     */
    struct ListViewColumn {
        /**
         * @brief 列标题
         */
        std::wstring header;

        /**
         * @brief 列宽度
         */
        double width;

        /**
         * @brief 对齐方式
         */
        ListViewColumnAlignment alignment = ListViewColumnAlignment::Left;

        ListViewColumn(const std::wstring &header);
        ListViewColumn(const std::wstring &header, double width);
        ListViewColumn(const LVCOLUMNW &lvc);
        operator LVCOLUMNW() const;
    };

    /**
     * @brief 列表视图
     */
    class ListView : public ItemsControl<StrList>
    {
    public:
        /**
         * @brief 列数
         */
        const ReadOnlyProperty<int> ColumnsCount;

        /**
         * @brief 是否显示网格线
         */
        const Property<bool> GridLines;

        /**
         * @brief 是否允许多选
         */
        const Property<bool> MultiSelect;

        /**
         * @brief 多选状态下可通过该属性获取选中项的个数
         */
        const ReadOnlyProperty<int> SelectedCount;

        /**
         * @brief 是否在第一列显示复选框
         */
        const Property<bool> CheckBoxes;

        /**
         * @brief 当前列表框页面第一个子项的索引
         */
        const ReadOnlyProperty<int> TopIndex;

    public:
        /**
         * @brief 初始化ListView
         */
        ListView();

    protected:
        /**
         * @brief  获取子项数
         */
        virtual int GetItemsCount() override;

        /**
         * @brief 选中项的索引，当无选中项时为-1
         */
        virtual int GetSelectedIndex() override;

        /**
         * @brief 设置选中项索引
         */
        virtual void SetSelectedIndex(int index) override;

        /**
         * @brief 获取选中项
         */
        virtual StrList GetSelectedItem() override;

        /**
         * @brief        设置背景颜色
         * @param color  要设置的颜色
         * @param redraw 是否重绘
         */
        virtual void SetBackColor(Color color, bool redraw) override;

        /**
         * @brief        设置文本颜色
         * @param color  要设置的颜色
         * @param redraw 是否重绘
         */
        virtual void SetTextColor(Color color, bool redraw) override;

        /**
         * @brief        接收到WM_NOTIFY后调用该函数
         * @param pNMHDR 包含有关通知消息的信息
         * @return       若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnNotify(NMHDR *pNMHDR) override;

        /**
         * @brief 父窗口接收到WM_NOTIFY后调用发出通知控件的该函数
         */
        virtual void OnNotified(NMHDR *pNMHDR) override;

        /**
         * @brief 列表项某些属性发生变化时调用该函数
         */
        virtual void OnItemChanged(NMLISTVIEW *pNMLV);

        /**
         * @brief       复选框选中状态发生改变
         * @param index 改变项的索引
         */
        virtual void OnCheckStateChanged(int index);

        /**
         * @brief 鼠标左键单击列标题时调用该函数
         */
        virtual void OnHeaderItemClicked(NMHEADERW *pNMH);

        /**
         * @brief 鼠标左键双击列标题时调用该函数
         */
        virtual void OnHeaderItemDoubleClicked(NMHEADERW *pNMH);

        /**
         * @brief 鼠标左键单击某一项时调用该函数
         */
        virtual void OnItemClicked(NMITEMACTIVATE *pNMIA);

        /**
         * @brief 鼠标左键双击某一项调用该函数
         */
        virtual void OnItemDoubleClicked(NMITEMACTIVATE *pNMIA);

    public:
        /**
         * @brief 清空所有子项
         */
        virtual void Clear() override;

        /**
         * @brief       获取指定索引处子项的值
         * @param index 子项的索引
         */
        virtual StrList GetItemAt(int index) override;

        /**
         * @brief      添加新的子项
         * @param item 要添加的子项
         * @return     是否添加成功
         */
        virtual bool AddItem(const StrList &item) override;

        /**
         * @brief       添加子项到指定索引
         * @param index 要插入的位置
         * @param item  要添加的子项
         * @return      是否添加成功
         */
        virtual bool InsertItem(int index, const StrList &item) override;

        /**
         * @brief          更新指定位置的子项
         * @param index    要更新子项的位置
         * @param newValue 子项的新值
         * @return         操作是否成功
         */
        virtual bool UpdateItem(int index, const StrList &newValue) override;

        /**
         * @brief       移除指定索引处的子项
         * @param index 要移除子项的索引
         * @return      操作是否成功
         */
        virtual bool RemoveItemAt(int index) override;

        /**
         * @brief     获取指定位置处文本
         * @param row 所在行
         * @param col 所在列
         * @return    对应位置的文本，若获取失败则返回空字符串
         */
        std::wstring GetItemAt(int row, int col);

        /**
         * @brief          更新指定位置处文本
         * @param row      所在行
         * @param col      所在列
         * @param newValue 要设置的文本
         * @return         操作是否成功
         */
        bool UpdateItem(int row, int col, const std::wstring &newValue);

        /**
         * @brief        添加新的列
         * @param column 要添加的列信息
         * @return       操作是否成功
         */
        bool AddColumn(const ListViewColumn &column);

        /**
         * @brief        添加新的列
         * @param header 要添加列的标题
         * @return       操作是否成功
         */
        bool AddColumn(const std::wstring &header);

        /**
         * @brief        添加新的列到指定索引
         * @param index  要插入的位置
         * @param column 要添加的列信息
         * @return       操作是否成功
         */
        bool InsertColumn(int index, const ListViewColumn &column);

        /**
         * @brief        添加新的列到指定索引
         * @param index  要插入的位置
         * @param header 要添加列的标题
         * @return       操作是否成功
         */
        bool InsertColumn(int index, const std::wstring &header);

        /**
         * @brief       设置指定列的标题
         * @param index 列的索引
         * @return      操作是否成功
         */
        bool SetColumnHeader(int index, const std::wstring &header);

        /**
         * @brief       获取指定列的宽度
         * @param index 列的索引
         * @return      列的宽度，若列不存在则返回-1
         */
        double GetColumnWidth(int index);

        /**
         * @brief       设置指定列的宽度
         * @param index 列的索引
         * @param width 要设置的宽度
         * @return      操作是否成功
         */
        bool SetColumnWidth(int index, double width);

        /**
         * @brief       移除指定列
         * @param index 列的索引
         * @return      操作是否成功
         */
        bool RemoveColumnAt(int index);

        /**
         * @brief 获取所有选中项的索引
         */
        List<int> GetSelectedIndices();

        /**
         * @brief 获取所有复选框选中的项的索引
         */
        List<int> GetCheckedIndices();

        /**
         * @brief 获取指定索引项的复选框是否选中
         */
        bool GetItemCheckState(int index);

        /**
         * @brief 设置指定索引项复选框的选中状态
         */
        void SetItemCheckState(int index, bool value);

        /**
         * @brief       获取指定点处子项的索引
         * @param point 相对于用户区左上角点的位置
         */
        int GetItemIndexFromPoint(const Point &point);

    private:
        /**
         * @brief 获取行数
         */
        int _GetRowCount();

        /**
         * @brief 获取列数
         */
        int _GetColCount();

        /**
         * @brief 获取ListView扩展样式
         */
        DWORD _GetExtendedListViewStyle();

        /**
         * @brief  设置ListView扩展样式
         * @return 先前的样式
         */
        DWORD _SetExtendedListViewStyle(DWORD style);
    };
};

// Panel.h


namespace sw
{
    /**
     * @brief 边框类型
     */
    enum class BorderStyle {
        None   = 0,           // 无边框
        Bump   = EDGE_BUMP,   // 突出的凸起边框
        Etched = EDGE_ETCHED, // 刻痕式边框
        Raised = EDGE_RAISED, // 凸起边框
        Sunked = EDGE_SUNKEN, // 凹陷边框
    };

    /**
     * @brief 面板
     */
    class Panel : public PanelBase
    {
    private:
        /**
         * @brief 边框类型，默认为无边框
         */
        BorderStyle _borderStyle = sw::BorderStyle::None;

    public:
        /**
         * @brief 边框样式
         */
        const Property<sw::BorderStyle> BorderStyle;

    public:
        /**
         * @brief 初始化面板
         */
        Panel();

    protected:
        /**
         * @brief  接收到WM_PAINT时调用该函数
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnPaint() override;

        /**
         * @brief               接收到WM_SIZE时调用该函数
         * @param newClientSize 改变后的用户区尺寸
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnSize(Size newClientSize) override;

        /**
         * @brief               接收到WM_MOUSEMOVE时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState      指示某些按键是否按下
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseMove(Point mousePosition, MouseKey keyState) override;
    };
}

// PasswordBox.h


namespace sw
{
    class PasswordBox : public TextBoxBase
    {
    public:
        /**
         * @brief 密码框显示的字符
         */
        const Property<wchar_t> PasswordChar;

    public:
        /**
         * @brief 初始化密码框
         */
        PasswordBox();
    };
}

// TextBox.h


namespace sw
{
    /**
     * @brief 编辑框
     */
    class TextBox : public TextBoxBase
    {
    private:
        /**
         * @brief 是否自动换行
         */
        bool _autoWrap = false;

    public:
        /**
         * @brief 是否自动换行，仅在MultiLine属性为true时有效
         */
        const Property<bool> AutoWrap;

        /**
         * @brief 是否允许多行文本
         */
        const Property<bool> MultiLine;

        /**
         * @brief 是否显示横向滚动条
         */
        const Property<bool> HorizontalScrollBar;

        /**
         * @brief 是否显示纵向滚动条
         */
        const Property<bool> VerticalScrollBar;

    public:
        /**
         * @brief 初始化编辑框
         */
        TextBox();
    };
}

// CheckBox.h


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

// DockPanel.h


namespace sw
{
    /**
     * @brief 停靠面板
     */
    class DockPanel : public Panel
    {
    private:
        /**
         * @brief 默认布局对象
         */
        DockLayout _dockLayout = DockLayout();

    public:
        /**
         * @brief 最后一个子元素是否填充剩余空间
         */
        const Property<bool> LastChildFill;

    public:
        /**
         * @brief 初始化DockPanel
         */
        DockPanel();

        /**
         * @brief 获取指定元素的Dock
         */
        static DockLayout::DockLayoutTag GetDock(UIElement &element);

        /**
         * @brief 设置指定元素的Dock
         */
        static void SetDock(UIElement &element, DockLayout::DockLayoutTag dock);

    protected:
        /**
         * @brief 获取默认布局对象
         */
        virtual LayoutHost *GetDefaultLayout() override;
    };
}

// RadioButton.h


namespace sw
{
    /**
     * @brief 单选框
     */
    class RadioButton : public CheckableButton
    {
    public:
        /**
         * @brief 初始化单选框
         */
        RadioButton();
    };
}

// StackPanel.h


namespace sw
{
    /**
     * @brief 堆叠面板
     */
    class StackPanel : public Panel
    {
    private:
        /**
         * @brief 默认布局对象
         */
        StackLayout _stackLayout = StackLayout();

    public:
        /**
         * @brief 排列方式
         */
        const Property<sw::Orientation> Orientation;

    public:
        /**
         * @brief 初始化StackPanel
         */
        StackPanel();

    protected:
        /**
         * @brief 获取默认布局对象
         */
        virtual LayoutHost *GetDefaultLayout() override;
    };
}

// WrapPanel.h


namespace sw
{
    /**
     * @brief 自动换行面板
     */
    class WrapPanel : public Panel
    {
    private:
        /**
         * @brief 默认布局对象
         */
        WrapLayout _wrapLayout = WrapLayout();

    public:
        /**
         * @brief 排列方式
         */
        const Property<sw::Orientation> Orientation;

    public:
        /**
         * @brief 初始化WrapPanel
         */
        WrapPanel();

    protected:
        /**
         * @brief 获取默认布局对象
         */
        virtual LayoutHost *GetDefaultLayout() override;
    };
}

// SimpleWindow.h

// 包含SimpleWindow所有头文件

// 启用视觉样式
#pragma comment(linker, "\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
