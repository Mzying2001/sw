#include "Color.h"
#include "Utils.h"

sw::Color::Color(uint8_t r, uint8_t g, uint8_t b) noexcept
    : r(r), g(g), b(b), _reserved(0)
{
}

sw::Color::Color(KnownColors knownColor) noexcept
    : Color(static_cast<COLORREF>(knownColor))
{
}

sw::Color::Color(COLORREF color) noexcept
    : r(GetRValue(color)), g(GetGValue(color)), b(GetBValue(color)), _reserved(0)
{
}

sw::Color::operator COLORREF() const noexcept
{
    return RGB(this->r, this->g, this->b);
}

bool sw::Color::Equals(const Color &other) const noexcept
{
    return (this->r == other.r) && (this->g == other.g) && (this->b == other.b);
}

std::wstring sw::Color::ToString() const
{
    return Utils::FormatStr(L"Color{r=%u, g=%u, b=%u}", this->r, this->g, this->b);
}
