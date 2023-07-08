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

std::wostream &sw::operator<<(std::wostream &wos, const Color &color)
{
    return wos << L"Color{r=" << (int)color.r << L", g=" << (int)color.g << L", b=" << (int)color.b << L"}";
}
