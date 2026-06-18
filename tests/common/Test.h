#pragma once

#include <exception>
#include <functional>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace swtest
{
    struct TestCase {
        const char *name;
        void (*func)();
    };

    inline std::vector<TestCase> &Registry()
    {
        static std::vector<TestCase> tests;
        return tests;
    }

    struct Registrar {
        Registrar(const char *name, void (*func)())
        {
            Registry().push_back(TestCase{name, func});
        }
    };

    class AssertionFailure : public std::runtime_error
    {
    public:
        explicit AssertionFailure(const std::string &message)
            : std::runtime_error(message)
        {
        }
    };

    inline void Fail(
        const char *kind,
        const char *expr,
        const char *file,
        int line,
        const std::string &detail = {})
    {
        std::ostringstream oss;
        oss << file << ":" << line << ": " << kind << " failed: " << expr;
        if (!detail.empty()) {
            oss << " (" << detail << ")";
        }
        throw AssertionFailure(oss.str());
    }

    inline void Check(bool condition, const char *expr, const char *file, int line)
    {
        if (!condition) {
            Fail("CHECK", expr, file, line);
        }
    }

    template <typename TExpected, typename TActual>
    void CheckEqual(
        const TExpected &expected,
        const TActual &actual,
        const char *expectedExpr,
        const char *actualExpr,
        const char *file,
        int line)
    {
        if (!(expected == actual)) {
            std::ostringstream oss;
            oss << expectedExpr << " == " << actualExpr;
            Fail("CHECK_EQ", oss.str().c_str(), file, line);
        }
    }

    template <typename TException, typename TFunc>
    void RequireThrowsAs(TFunc func, const char *expr, const char *exceptionName, const char *file, int line)
    {
        try {
            func();
        } catch (const TException &) {
            return;
        } catch (const std::exception &ex) {
            std::ostringstream oss;
            oss << "expected " << exceptionName << ", got std::exception: " << ex.what();
            Fail("REQUIRE_THROWS_AS", expr, file, line, oss.str());
        } catch (...) {
            std::ostringstream oss;
            oss << "expected " << exceptionName << ", got non-standard exception";
            Fail("REQUIRE_THROWS_AS", expr, file, line, oss.str());
        }

        std::ostringstream oss;
        oss << "expected " << exceptionName << ", no exception thrown";
        Fail("REQUIRE_THROWS_AS", expr, file, line, oss.str());
    }
}

#define SWTEST_CONCAT_IMPL(a, b) a##b
#define SWTEST_CONCAT(a, b) SWTEST_CONCAT_IMPL(a, b)

#define TEST_CASE(name)                                                                    \
    static void SWTEST_CONCAT(SwTestFunc_, __LINE__)();                                    \
    static ::swtest::Registrar SWTEST_CONCAT(SwTestRegistrar_, __LINE__)(                  \
        name, &SWTEST_CONCAT(SwTestFunc_, __LINE__));                                      \
    static void SWTEST_CONCAT(SwTestFunc_, __LINE__)()

#define CHECK(expr) ::swtest::Check(static_cast<bool>(expr), #expr, __FILE__, __LINE__)
#define REQUIRE(expr) CHECK(expr)
#define CHECK_EQ(expected, actual) \
    ::swtest::CheckEqual((expected), (actual), #expected, #actual, __FILE__, __LINE__)
#define REQUIRE_THROWS_AS(expr, exceptionType) \
    ::swtest::RequireThrowsAs<exceptionType>([&]() { (void)(expr); }, #expr, #exceptionType, __FILE__, __LINE__)
