#pragma once

#include <Windows.h>
#include <cstdint>

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
            FloralWhite          = RGB(255, 250, 240), // 花的白色
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
