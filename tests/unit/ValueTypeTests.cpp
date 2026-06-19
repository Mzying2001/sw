#include "Test.h"

#include "Color.h"
#include "Dip.h"
#include "Point.h"
#include "Rect.h"
#include "Size.h"
#include "Thickness.h"

#include <windows.h>

namespace
{
    struct DipResetGuard {
        ~DipResetGuard()
        {
            sw::Dip::Update(96, 96);
        }
    };
}

TEST_CASE("Point supports construction conversion equality and string form")
{
    DipResetGuard guard;
    sw::Dip::Update(96, 96);

    sw::Point point(12.5, -3.25);

    CHECK_EQ(12.5, point.x);
    CHECK_EQ(-3.25, point.y);
    CHECK(point == sw::Point(12.5, -3.25));
    CHECK(point != sw::Point(12.5, -3.0));
    CHECK_EQ(std::wstring(L"(12.5, -3.25)"), point.ToString());

    POINT native = point;
    CHECK_EQ(13, native.x);
    CHECK_EQ(-3, native.y);

    sw::Dip::Update(192, 96);
    sw::Point scaled(POINT{24, 10});
    CHECK_EQ(12.0, scaled.x);
    CHECK_EQ(10.0, scaled.y);
}

TEST_CASE("Size supports construction conversion equality and string form")
{
    DipResetGuard guard;
    sw::Dip::Update(96, 96);

    sw::Size size(80.0, 24.5);

    CHECK_EQ(80.0, size.width);
    CHECK_EQ(24.5, size.height);
    CHECK(size == sw::Size(80.0, 24.5));
    CHECK(size != sw::Size(80.0, 25.0));
    CHECK_EQ(std::wstring(L"Size{width=80, height=24.5}"), size.ToString());

    SIZE native = size;
    CHECK_EQ(80, native.cx);
    CHECK_EQ(25, native.cy);

    sw::Dip::Update(96, 192);
    sw::Size scaled(SIZE{10, 40});
    CHECK_EQ(10.0, scaled.width);
    CHECK_EQ(20.0, scaled.height);
}

TEST_CASE("Rect exposes position size conversion equality and string form")
{
    DipResetGuard guard;
    sw::Dip::Update(96, 96);

    sw::Rect rect(2.0, 4.0, 16.0, 8.0);

    CHECK_EQ(sw::Point(2.0, 4.0), rect.GetPos());
    CHECK_EQ(sw::Size(16.0, 8.0), rect.GetSize());
    CHECK(rect == sw::Rect(2.0, 4.0, 16.0, 8.0));
    CHECK(rect != sw::Rect(2.0, 4.0, 16.0, 9.0));
    CHECK_EQ(std::wstring(L"Rect{left=2, top=4, width=16, height=8}"), rect.ToString());

    RECT native = rect;
    CHECK_EQ(2, native.left);
    CHECK_EQ(4, native.top);
    CHECK_EQ(18, native.right);
    CHECK_EQ(12, native.bottom);

    sw::Dip::Update(192, 192);
    sw::Rect scaled(RECT{4, 8, 20, 24});
    CHECK_EQ(2.0, scaled.left);
    CHECK_EQ(4.0, scaled.top);
    CHECK_EQ(8.0, scaled.width);
    CHECK_EQ(8.0, scaled.height);
}

TEST_CASE("Thickness supports all constructors conversion equality and string form")
{
    DipResetGuard guard;
    sw::Dip::Update(96, 96);

    sw::Thickness uniform(3.0);
    CHECK_EQ(sw::Thickness(3.0, 3.0, 3.0, 3.0), uniform);

    sw::Thickness axes(2.0, 5.0);
    CHECK_EQ(sw::Thickness(2.0, 5.0, 2.0, 5.0), axes);

    sw::Thickness edges(1.0, 2.0, 3.0, 4.0);
    CHECK(edges == sw::Thickness(1.0, 2.0, 3.0, 4.0));
    CHECK(edges != sw::Thickness(1.0, 2.0, 3.0, 5.0));
    CHECK_EQ(std::wstring(L"Thickness{left=1, top=2, right=3, bottom=4}"), edges.ToString());

    RECT native = static_cast<RECT>(edges);
    CHECK_EQ(1, native.left);
    CHECK_EQ(2, native.top);
    CHECK_EQ(3, native.right);
    CHECK_EQ(4, native.bottom);

    sw::Dip::Update(192, 96);
    sw::Thickness scaled(RECT{10, 20, 30, 40});
    CHECK_EQ(5.0, scaled.left);
    CHECK_EQ(20.0, scaled.top);
    CHECK_EQ(15.0, scaled.right);
    CHECK_EQ(40.0, scaled.bottom);
}

TEST_CASE("Color supports rgb known color and COLORREF conversion")
{
    sw::Color color(1, 2, 3);

    CHECK_EQ(1, color.r);
    CHECK_EQ(2, color.g);
    CHECK_EQ(3, color.b);
    CHECK_EQ(0, color._reserved);
    CHECK_EQ(std::wstring(L"Color{r=1, g=2, b=3}"), color.ToString());

    COLORREF native = static_cast<COLORREF>(color);
    CHECK_EQ(static_cast<COLORREF>(RGB(1, 2, 3)), native);

    sw::Color fromNative(static_cast<COLORREF>(RGB(10, 20, 30)));
    CHECK_EQ(sw::Color(10, 20, 30), fromNative);

    sw::Color known(sw::KnownColors::RebeccaPurple);
    CHECK_EQ(sw::Color(0x66, 0x33, 0x99), known);

    sw::Color reservedA(4, 5, 6);
    sw::Color reservedB(4, 5, 6);
    reservedB._reserved = 99;
    CHECK(reservedA == reservedB);
}

TEST_CASE("Dip converts pixels and DIPs using current thread scale")
{
    DipResetGuard guard;

    sw::Dip::Update(144, 192);

    CHECK_EQ(96.0 / 144.0, sw::Dip::ScaleX.Get());
    CHECK_EQ(0.5, sw::Dip::ScaleY.Get());
    CHECK_EQ(16.0, sw::Dip::PxToDipX(24));
    CHECK_EQ(15.0, sw::Dip::PxToDipY(30));
    CHECK_EQ(24, sw::Dip::DipToPxX(16.0));
    CHECK_EQ(30, sw::Dip::DipToPxY(15.0));

    sw::Dip::Update(96, 96);
    CHECK_EQ(12.0, sw::Dip::PxToDipX(12));
    CHECK_EQ(12, sw::Dip::DipToPxX(12.0));
}
