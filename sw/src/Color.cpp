#include "Color.h"

sw::Color::Color()
    : Color(0, 0, 0)
{
}

sw::Color::Color(uint8_t r, uint8_t g, uint8_t b)
    : r(r), g(g), b(b)
{
}

sw::Color::Color(COLORREF color)
{
    this->r = (color >> 0) & 0xFF;
    this->g = (color >> 8) & 0xFF;
    this->b = (color >> 16) & 0xFF;
}

sw::Color::operator COLORREF() const
{
    return RGB(this->r, this->g, this->b);
}

const sw::Color AliceBlue(240, 248, 255);
const sw::Color AntiqueWhite(250, 235, 215);
const sw::Color Aqua(0, 255, 255);
const sw::Color Aquamarine(127, 255, 212);
const sw::Color Azure(240, 255, 255);
const sw::Color Beige(245, 245, 220);
const sw::Color Bisque(255, 228, 196);
const sw::Color Black(0, 0, 0);
const sw::Color BlanchedAlmond(255, 235, 205);
const sw::Color Blue(0, 0, 255);
const sw::Color BlueViolet(138, 43, 226);
const sw::Color Brown(165, 42, 42);
const sw::Color BurlyWood(222, 184, 135);
const sw::Color CadetBlue(95, 158, 160);
const sw::Color Chartreuse(127, 255, 0);
const sw::Color Chocolate(210, 105, 30);
const sw::Color Coral(255, 127, 80);
const sw::Color CornflowerBlue(100, 149, 237);
const sw::Color Cornsilk(255, 248, 220);
const sw::Color Crimson(220, 20, 60);
const sw::Color Cyan(0, 255, 255);
const sw::Color DarkBlue(0, 0, 139);
const sw::Color DarkCyan(0, 139, 139);
const sw::Color DarkGoldenrod(184, 134, 11);
const sw::Color DarkGray(169, 169, 169);
const sw::Color DarkGreen(0, 100, 0);
const sw::Color DarkKhaki(189, 183, 107);
const sw::Color DarkMagenta(139, 0, 139);
const sw::Color DarkOliveGreen(85, 107, 47);
const sw::Color DarkOrange(255, 140, 0);
const sw::Color DarkOrchid(153, 50, 204);
const sw::Color DarkRed(139, 0, 0);
const sw::Color DarkSalmon(233, 150, 122);
const sw::Color DarkSeaGreen(143, 188, 143);
const sw::Color DarkSlateBlue(72, 61, 139);
const sw::Color DarkSlateGray(47, 79, 79);
const sw::Color DarkTurquoise(0, 206, 209);
const sw::Color DarkViolet(148, 0, 211);
const sw::Color DeepPink(255, 20, 147);
const sw::Color DeepSkyBlue(0, 191, 255);
const sw::Color DimGray(105, 105, 105);
const sw::Color DodgerBlue(30, 144, 255);
const sw::Color Firebrick(178, 34, 34);
const sw::Color FloralWhite(255, 250, 240);
const sw::Color ForestGreen(34, 139, 34);
const sw::Color Fuchsia(255, 0, 255);
const sw::Color Gainsboro(220, 220, 220);
const sw::Color GhostWhite(248, 248, 255);
const sw::Color Gold(255, 215, 0);
const sw::Color Goldenrod(218, 165, 32);
const sw::Color Gray(128, 128, 128);
const sw::Color Green(0, 128, 0);
const sw::Color GreenYellow(173, 255, 47);
const sw::Color Honeydew(240, 255, 240);
const sw::Color HotPink(255, 105, 180);
const sw::Color IndianRed(205, 92, 92);
const sw::Color Indigo(75, 0, 130);
const sw::Color Ivory(255, 255, 240);
const sw::Color Khaki(240, 230, 140);
const sw::Color Lavender(230, 230, 250);
const sw::Color LavenderBlush(255, 240, 245);
const sw::Color LawnGreen(124, 252, 0);
const sw::Color LemonChiffon(255, 250, 205);
const sw::Color LightBlue(173, 216, 230);
const sw::Color LightCoral(240, 128, 128);
const sw::Color LightCyan(224, 255, 255);
const sw::Color LightGoldenrodYellow(250, 250, 210);
const sw::Color LightGray(211, 211, 211);
const sw::Color LightGreen(144, 238, 144);
const sw::Color LightPink(255, 182, 193);
const sw::Color LightSalmon(255, 160, 122);
const sw::Color LightSeaGreen(32, 178, 170);
const sw::Color LightSkyBlue(135, 206, 250);
const sw::Color LightSlateGray(119, 136, 153);
const sw::Color LightSteelBlue(176, 196, 222);
const sw::Color LightYellow(255, 255, 224);
const sw::Color Lime(0, 255, 0);
const sw::Color LimeGreen(50, 205, 50);
const sw::Color Linen(250, 240, 230);
const sw::Color Magenta(255, 0, 255);
const sw::Color Maroon(128, 0, 0);
const sw::Color MediumAquamarine(102, 205, 170);
const sw::Color MediumBlue(0, 0, 205);
const sw::Color MediumOrchid(186, 85, 211);
const sw::Color MediumPurple(147, 112, 219);
const sw::Color MediumSeaGreen(60, 179, 113);
const sw::Color MediumSlateBlue(123, 104, 238);
const sw::Color MediumSpringGreen(0, 250, 154);
const sw::Color MediumTurquoise(72, 209, 204);
const sw::Color MediumVioletRed(199, 21, 133);
const sw::Color MidnightBlue(25, 25, 112);
const sw::Color MintCream(245, 255, 250);
const sw::Color MistyRose(255, 228, 225);
const sw::Color Moccasin(255, 228, 181);
const sw::Color NavajoWhite(255, 222, 173);
const sw::Color Navy(0, 0, 128);
const sw::Color OldLace(253, 245, 230);
const sw::Color Olive(128, 128, 0);
const sw::Color OliveDrab(107, 142, 35);
const sw::Color Orange(255, 165, 0);
const sw::Color OrangeRed(255, 69, 0);
const sw::Color Orchid(218, 112, 214);
const sw::Color PaleGoldenrod(238, 232, 170);
const sw::Color PaleGreen(152, 251, 152);
const sw::Color PaleTurquoise(175, 238, 238);
const sw::Color PaleVioletRed(219, 112, 147);
const sw::Color PapayaWhip(255, 239, 213);
const sw::Color PeachPuff(255, 218, 185);
const sw::Color Peru(205, 133, 63);
const sw::Color Pink(255, 192, 203);
const sw::Color Plum(221, 160, 221);
const sw::Color PowderBlue(176, 224, 230);
const sw::Color Purple(128, 0, 128);
const sw::Color Red(255, 0, 0);
const sw::Color RosyBrown(188, 143, 143);
const sw::Color RoyalBlue(65, 105, 225);
const sw::Color SaddleBrown(139, 69, 19);
const sw::Color Salmon(250, 128, 114);
const sw::Color SandyBrown(244, 164, 96);
const sw::Color SeaGreen(46, 139, 87);
const sw::Color SeaShell(255, 245, 238);
const sw::Color Sienna(160, 82, 45);
const sw::Color Silver(192, 192, 192);
const sw::Color SkyBlue(135, 206, 235);
const sw::Color SlateBlue(106, 90, 205);
const sw::Color SlateGray(112, 128, 144);
const sw::Color Snow(255, 250, 250);
const sw::Color SpringGreen(0, 255, 127);
const sw::Color SteelBlue(70, 130, 180);
const sw::Color Tan(210, 180, 140);
const sw::Color Teal(0, 128, 128);
const sw::Color Thistle(216, 191, 216);
const sw::Color Tomato(255, 99, 71);
const sw::Color Turquoise(64, 224, 208);
const sw::Color Violet(238, 130, 238);
const sw::Color Wheat(245, 222, 179);
const sw::Color White(255, 255, 255);
const sw::Color WhiteSmoke(245, 245, 245);
const sw::Color Yellow(255, 255, 0);
const sw::Color YellowGreen(154, 205, 50);
