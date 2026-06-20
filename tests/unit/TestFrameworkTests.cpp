#include "Test.h"

#include <sstream>
#include <stdexcept>
#include <string>

namespace
{
    int g_manualTestCalls = 0;

    void PassingManualTest()
    {
        ++g_manualTestCalls;
        CHECK_EQ(2, 1 + 1);
    }

    void NonFatalManualTest()
    {
        CHECK_EQ(1, 2);
        CHECK_EQ(3, 3);
    }

    void FatalManualTest()
    {
        REQUIRE_EQ(1, 2);
        CHECK_EQ(3, 4);
    }
}

TEST_CASE("Test framework records passing test metadata")
{
    swtest::TestCase test{"manual passing test", PassingManualTest, __FILE__, __LINE__};
    g_manualTestCalls = 0;

    auto result = swtest::RunOne(test);

    CHECK(result.Passed());
    CHECK_EQ(1, g_manualTestCalls);
    CHECK_EQ(1, result.assertions);
    CHECK_EQ(0, static_cast<int>(result.failures.size()));
    CHECK_GE(result.elapsedMilliseconds, 0.0);
}

TEST_CASE("Test framework keeps non-fatal failures and continues")
{
    swtest::TestCase test{"manual non-fatal test", NonFatalManualTest, __FILE__, __LINE__};

    auto result = swtest::RunOne(test);

    REQUIRE_FALSE(result.Passed());
    REQUIRE_EQ(1, static_cast<int>(result.failures.size()));
    CHECK_EQ(2, result.assertions);
    CHECK_EQ(std::string("CHECK_EQ"), result.failures[0].kind);
    CHECK_FALSE(result.failures[0].fatal);
    CHECK_NE(std::string::npos, result.failures[0].detail.find("expected: 1"));
    CHECK_NE(std::string::npos, result.failures[0].detail.find("actual: 2"));
}

TEST_CASE("Test framework stops current test on fatal failure")
{
    swtest::TestCase test{"manual fatal test", FatalManualTest, __FILE__, __LINE__};

    auto result = swtest::RunOne(test);

    REQUIRE_FALSE(result.Passed());
    REQUIRE_EQ(1, static_cast<int>(result.failures.size()));
    CHECK_EQ(1, result.assertions);
    CHECK_EQ(std::string("REQUIRE_EQ"), result.failures[0].kind);
    CHECK(result.failures[0].fatal);
}

TEST_CASE("Test framework reports unexpected exceptions")
{
    swtest::TestCase test{
        "manual throwing test",
        []() {
            throw std::runtime_error("boom");
        },
        __FILE__,
        __LINE__};

    auto result = swtest::RunOne(test);

    REQUIRE_FALSE(result.Passed());
    REQUIRE_EQ(1, static_cast<int>(result.failures.size()));
    CHECK_EQ(std::string("EXCEPTION"), result.failures[0].kind);
    CHECK_EQ(std::string("boom"), result.failures[0].detail);
}

TEST_CASE("Test framework supports filtered execution summaries")
{
    swtest::TestOptions options;
    options.filter = "Delegate empty invocation throws";

    std::ostringstream out;
    std::ostringstream err;
    const auto summary = swtest::RunTests(options, out, err);

    CHECK_EQ(1, summary.selected);
    CHECK_EQ(1, summary.passed);
    CHECK_EQ(0, summary.failed);
    CHECK_EQ(0, static_cast<int>(err.str().size()));
    CHECK_NE(std::string::npos, out.str().find("[ RUN      ] Delegate empty invocation throws"));
}

TEST_CASE("Test framework stringifies narrow and wide strings")
{
    CHECK_EQ(std::string("alpha"), std::string("alpha"));
    CHECK_EQ(std::wstring(L"beta"), std::wstring(L"beta"));
    CHECK_THROWS_AS(throw std::logic_error("expected"), std::logic_error);
    CHECK_NOTHROW(std::string("ok"));
}
