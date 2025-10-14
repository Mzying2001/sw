#include "Color.h"
#include "Utils.h"

sw::Color::Color(uint8_t r, uint8_t g, uint8_t b)
    : r(r), g(g), b(b), _reserved(0)
{
}

sw::Color::Color(KnownColor knownColor)
    : Color(static_cast<COLORREF>(knownColor))
{
}

sw::Color::Color(COLORREF color)
    : r((color >> 0) & 0xFF), g((color >> 8) & 0xFF), b((color >> 16) & 0xFF)
{
}

sw::Color::operator COLORREF() const
{
    return RGB(this->r, this->g, this->b);
}

bool sw::Color::Equals(const Color &other) const
{
    return (this->r == other.r) && (this->g == other.g) && (this->b == other.b);
}

std::wstring sw::Color::ToString() const
{
    return Utils::FormatStr(L"Color{r=%u, g=%u, b=%u}", this->r, this->g, this->b);
}
