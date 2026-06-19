#include "Test.h"

#include "EnumBit.h"
#include "Internal.h"
#include "Path.h"
#include "ProcMsg.h"
#include "Property.h"
#include "Utils.h"
#include "WndMsg.h"

#include <map>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>
#include <windows.h>

namespace sw
{
    enum class TestFlags {
        None = 0,
        A    = 1,
        B    = 2,
        C    = 4,
    };

    _SW_ENUM_ENABLE_BIT_OPERATIONS(TestFlags);
}

namespace
{
    struct StringLike {
        std::wstring ToString() const
        {
            return L"string-like";
        }
    };

    struct ScalarComparable {
        int value;

        bool operator>(const ScalarComparable &other) const
        {
            return value > other.value;
        }

        bool operator<(const ScalarComparable &other) const
        {
            return value < other.value;
        }
    };

    struct CastSource {
        explicit operator int() const
        {
            return 3;
        }
    };

    struct Base {
        virtual ~Base() = default;
    };

    struct Derived : Base {
    };

    struct PropertyOwner {
        int value = 0;

        sw::Property<int> Value{
            sw::Property<int>::Init(this).Getter<&PropertyOwner::value>().Setter<&PropertyOwner::value>()};
    };
}

TEST_CASE("Utils converts UTF-8 wide and narrow strings")
{
    sw::Utils::UseUtf8Encoding(true);

    const std::wstring wide = L"ASCII";
    const std::string narrow = sw::Utils::ToMultiByteStr(wide);

    CHECK_EQ(std::string("ASCII"), narrow);
    CHECK_EQ(wide, sw::Utils::ToWideStr(narrow));

    sw::Utils::UseUtf8Encoding(false);
    CHECK_EQ(std::wstring(L"plain"), sw::Utils::ToWideStr(std::string("plain")));

    sw::Utils::UseUtf8Encoding(true);
}

TEST_CASE("Utils trims and splits strings")
{
    CHECK_EQ(std::wstring(L"alpha"), sw::Utils::Trim(L" \talpha\r\n"));
    CHECK_EQ(std::wstring(L"alpha  "), sw::Utils::TrimStart(L" \nalpha  "));
    CHECK_EQ(std::wstring(L"  alpha"), sw::Utils::TrimEnd(L"  alpha\t\r"));
    CHECK_EQ(std::wstring(), sw::Utils::Trim(L" \t\r\n"));
    CHECK_EQ(std::wstring(), sw::Utils::TrimStart(L" \t"));
    CHECK_EQ(std::wstring(), sw::Utils::TrimEnd(L"\n\r"));

    auto parts = sw::Utils::Split(L",alpha,,beta,", L",");
    REQUIRE_EQ(5, static_cast<int>(parts.size()));
    CHECK_EQ(std::wstring(L""), parts[0]);
    CHECK_EQ(std::wstring(L"alpha"), parts[1]);
    CHECK_EQ(std::wstring(L""), parts[2]);
    CHECK_EQ(std::wstring(L"beta"), parts[3]);
    CHECK_EQ(std::wstring(L""), parts[4]);

    auto unchanged = sw::Utils::Split(L"abc", L"");
    REQUIRE_EQ(1, static_cast<int>(unchanged.size()));
    CHECK_EQ(std::wstring(L"abc"), unchanged[0]);

    auto multi = sw::Utils::Split(L"left--right--", L"--");
    REQUIRE_EQ(3, static_cast<int>(multi.size()));
    CHECK_EQ(std::wstring(L"left"), multi[0]);
    CHECK_EQ(std::wstring(L"right"), multi[1]);
    CHECK_EQ(std::wstring(L""), multi[2]);
}

TEST_CASE("Utils formats and builds strings from supported values")
{
    CHECK_EQ(std::wstring(L"value=42 ok"), sw::Utils::FormatStr(L"value=%d %ls", 42, L"ok"));

    PropertyOwner owner;
    owner.Value = 7;

    std::vector<int> vec{1, 2, 3};
    std::map<std::wstring, int> map{{L"one", 1}, {L"two", 2}};
    std::unordered_map<int, std::wstring> unordered{{2, L"two"}};

    CHECK_EQ(std::wstring(L"true false narrow wide string-like 7"),
             sw::Utils::BuildStr(true, L" ", false, L" ", "narrow", L" ", std::wstring(L"wide"), L" ", StringLike{}, L" ", owner.Value));
    CHECK_EQ(std::wstring(L"[1, 2, 3]"), sw::Utils::BuildStr(vec));
    CHECK_EQ(std::wstring(L"{one:1, two:2}"), sw::Utils::BuildStr(map));
    CHECK_EQ(std::wstring(L"{2:two}"), sw::Utils::BuildStr(unordered));
}

TEST_CASE("Utils min max work for scalar and object values")
{
    CHECK_EQ(7, sw::Utils::Max(3, 7));
    CHECK_EQ(3, sw::Utils::Min(3, 7));

    ScalarComparable low{1};
    ScalarComparable high{5};
    CHECK_EQ(5, sw::Utils::Max(low, high).value);
    CHECK_EQ(1, sw::Utils::Min(low, high).value);
}

TEST_CASE("Path extracts file names directories and extensions")
{
    CHECK_EQ(std::wstring(L"file.txt"), sw::Path::GetFileName(L"C:\\dir\\file.txt"));
    CHECK_EQ(std::wstring(L"file.txt"), sw::Path::GetFileName(L"/dir/file.txt"));
    CHECK_EQ(std::wstring(L"file"), sw::Path::GetFileNameWithoutExt(L"C:\\dir\\file.txt"));
    CHECK_EQ(std::wstring(L".bashrc"), sw::Path::GetFileNameWithoutExt(L"/home/.bashrc"));
    CHECK_EQ(std::wstring(L"trailing."), sw::Path::GetFileNameWithoutExt(L"trailing."));

    CHECK_EQ(std::wstring(L".txt"), sw::Path::GetExtension(L"C:\\dir.name\\file.txt"));
    CHECK_EQ(std::wstring(), sw::Path::GetExtension(L"C:\\dir.name\\file"));
    CHECK_EQ(std::wstring(), sw::Path::GetExtension(L"C:\\dir.name\\"));
    CHECK_EQ(std::wstring(), sw::Path::GetExtension(L".bashrc"));
    CHECK_EQ(std::wstring(), sw::Path::GetExtension(L"trailing."));

    CHECK_EQ(std::wstring(L"C:\\dir\\"), sw::Path::GetDirectory(L"C:\\dir\\file.txt"));
    CHECK_EQ(std::wstring(L"/dir/"), sw::Path::GetDirectory(L"/dir/file.txt"));
    CHECK_EQ(std::wstring(), sw::Path::GetDirectory(L"file.txt"));
}

TEST_CASE("Path combines segments and resolves absolute paths")
{
    CHECK_EQ(std::wstring(L"C:\\root\\child\\leaf"), sw::Path::Combine({L"C:\\root", L"child", L"leaf"}));
    CHECK_EQ(std::wstring(L"C:\\root\\child"), sw::Path::Combine({L"C:\\root\\", L"\\child"}));
    CHECK_EQ(std::wstring(L"\\\\server\\share\\dir"), sw::Path::Combine({L"\\\\server\\share", L"dir"}));
    CHECK_EQ(std::wstring(L"first\\second"), sw::Path::Combine({L"", L"first", L"", L"second"}));
    CHECK_EQ(std::wstring(L"a\\b\\c"), sw::Path::Combine(L"a", L"b", L"c"));

    auto absolute = sw::Path::GetAbsolutePath(L".");
    CHECK_FALSE(absolute.empty());
    CHECK_NE(std::wstring::npos, absolute.find(L":\\"));
}

TEST_CASE("EnumBit enables bit operations only for opted-in enum types")
{
    static_assert(sw::_EnumSupportBitOperations<sw::TestFlags>::value, "TestFlags should support bit operations");

    sw::TestFlags flags = sw::TestFlags::A | sw::TestFlags::B;
    CHECK(static_cast<int>(flags) == 3);
    CHECK(static_cast<int>(flags & sw::TestFlags::A) == 1);
    CHECK(static_cast<int>(flags ^ sw::TestFlags::B) == 1);

    flags |= sw::TestFlags::C;
    CHECK(static_cast<int>(flags) == 7);
    flags &= sw::TestFlags::A;
    CHECK(static_cast<int>(flags) == 1);
    flags ^= sw::TestFlags::C;
    CHECK(static_cast<int>(flags) == 5);

    CHECK(static_cast<int>(~sw::TestFlags::None) == ~0);
}

TEST_CASE("Internal traits classify conversions delegates and ToString support")
{
    static_assert(std::is_same<sw::_OptimalParamType<int>, int>::value, "Scalar parameters should pass by value");
    static_assert(std::is_same<sw::_OptimalParamType<std::wstring>, const std::wstring &>::value, "Objects should pass by const reference");
    static_assert(sw::_IsStaticCastable<CastSource, int>::value, "Explicit conversion should be detected");
    static_assert(!sw::_IsStaticCastable<std::wstring, int>::value, "Invalid conversion should not be detected");
    static_assert(sw::_IsDynamicCastable<Base *, Derived *>::value, "Polymorphic downcast should be detected");
    static_assert(!sw::_IsDynamicCastable<int, int>::value, "Non-class dynamic cast should not be detected");
    static_assert(sw::_HasToString<StringLike>::value, "ToString should be detected");
    static_assert(!sw::_HasToString<int>::value, "Plain int has no ToString");

    CHECK(true);
}

TEST_CASE("ProcMsg and WndMsg expose message data without creating windows")
{
    HWND hwnd = reinterpret_cast<HWND>(static_cast<INT_PTR>(0x1234));
    sw::ProcMsg msg(hwnd, WM_USER + 1, 2, 3);

    CHECK(msg.hwnd == hwnd);
    CHECK_EQ(static_cast<UINT>(WM_USER + 1), msg.uMsg);
    CHECK_EQ(static_cast<WPARAM>(2), msg.wParam);
    CHECK_EQ(static_cast<LPARAM>(3), msg.lParam);

    CHECK_GT(static_cast<UINT>(sw::WM_UpdateLayout), static_cast<UINT>(sw::WM_SimpleWindowBegin));
    CHECK_GT(static_cast<UINT>(sw::WM_SimpleWindowEnd), static_cast<UINT>(sw::WM_UpdateLayout));
}
