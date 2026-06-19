#pragma once

#include <chrono>
#include <codecvt>
#include <cstddef>
#include <cstring>
#include <exception>
#include <iomanip>
#include <iostream>
#include <locale>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

/**
 * @brief sw 单元测试使用的轻量级测试框架。
 *
 * 该框架刻意保持为单头文件实现，避免为测试工程引入第三方依赖。
 * CHECK 系列断言会记录失败并继续执行当前用例，REQUIRE 系列断言会立即终止当前用例。
 */
namespace swtest
{
    /**
     * @brief 单个测试用例的注册信息。
     */
    struct TestCase {
        const char *name; ///< 测试用例名称
        void (*func)();   ///< 测试用例入口函数
        const char *file; ///< 定义测试用例的源文件
        int line;         ///< 定义测试用例的行号
    };

    /**
     * @brief 单条断言失败记录。
     */
    struct Failure {
        std::string kind;       ///< 断言类型，如 CHECK_EQ
        std::string expression; ///< 失败的表达式
        std::string file;       ///< 失败位置所在文件
        int line = 0;           ///< 失败位置所在行号
        std::string detail;     ///< 额外诊断信息
        bool fatal = false;     ///< 是否为致命断言
    };

    /**
     * @brief 单个测试用例的执行结果。
     */
    struct TestResult {
        const TestCase *test = nullptr;  ///< 对应的测试用例
        std::vector<Failure> failures;   ///< 断言失败列表
        int assertions = 0;              ///< 执行过的断言数量
        double elapsedMilliseconds = 0.; ///< 用例耗时

        bool Passed() const
        {
            return failures.empty();
        }
    };

    /**
     * @brief 测试运行配置。
     */
    struct TestOptions {
        bool listOnly = false; ///< 只列出测试用例，不执行
        std::string filter;    ///< 按名称子串过滤测试用例
    };

    /**
     * @brief 测试运行汇总结果。
     */
    struct TestRunSummary {
        int total = 0;                   ///< 已注册用例数量
        int selected = 0;                ///< 本次实际选中的用例数量
        int passed = 0;                  ///< 通过用例数量
        int failed = 0;                  ///< 失败用例数量
        int assertions = 0;              ///< 执行过的断言总数
        double elapsedMilliseconds = 0.; ///< 总耗时
    };

    /**
     * @brief 获取全局测试注册表。
     */
    inline std::vector<TestCase> &Registry()
    {
        static std::vector<TestCase> tests;
        return tests;
    }

    /**
     * @brief 静态注册测试用例。
     */
    struct Registrar {
        Registrar(const char *name, void (*func)(), const char *file, int line)
        {
            Registry().push_back(TestCase{name, func, file, line});
        }

        Registrar(const char *name, void (*func)())
            : Registrar(name, func, nullptr, 0)
        {
        }
    };

    /**
     * @brief 断言失败异常基类。
     */
    class AssertionFailure : public std::runtime_error
    {
    public:
        explicit AssertionFailure(const std::string &message)
            : std::runtime_error(message)
        {
        }
    };

    /**
     * @brief 致命断言失败，用于立即终止当前测试用例。
     */
    class FatalAssertionFailure : public AssertionFailure
    {
    public:
        explicit FatalAssertionFailure(const std::string &message)
            : AssertionFailure(message)
        {
        }
    };

    namespace Detail
    {
        inline TestResult *&CurrentResult()
        {
            static TestResult *result = nullptr;
            return result;
        }

        class ScopedCurrentResult
        {
        public:
            explicit ScopedCurrentResult(TestResult &result)
                : _previous(CurrentResult())
            {
                CurrentResult() = &result;
            }

            ~ScopedCurrentResult()
            {
                CurrentResult() = _previous;
            }

        private:
            TestResult *_previous;
        };

        inline void CountAssertion()
        {
            if (CurrentResult() != nullptr) {
                ++CurrentResult()->assertions;
            }
        }

        inline std::string Narrow(const std::wstring &value)
        {
            try {
                std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
                return converter.to_bytes(value);
            } catch (...) {
                std::string fallback;
                fallback.reserve(value.size());
                for (auto ch : value) {
                    fallback.push_back(ch >= 0 && ch <= 0x7f ? static_cast<char>(ch) : '?');
                }
                return fallback;
            }
        }

        inline std::string EscapeString(const std::string &value)
        {
            std::ostringstream oss;
            for (char raw : value) {
                const unsigned char ch = static_cast<unsigned char>(raw);
                switch (raw) {
                    case '\\':
                        oss << "\\\\";
                        break;
                    case '"':
                        oss << "\\\"";
                        break;
                    case '\n':
                        oss << "\\n";
                        break;
                    case '\r':
                        oss << "\\r";
                        break;
                    case '\t':
                        oss << "\\t";
                        break;
                    default:
                        if (ch < 0x20) {
                            oss << "\\x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(ch);
                        } else {
                            oss << raw;
                        }
                        break;
                }
            }
            return oss.str();
        }

        template <typename T>
        class IsOstreamWritable
        {
        private:
            template <typename U>
            static auto Test(int) -> decltype(std::declval<std::ostream &>() << std::declval<const U &>(), std::true_type());

            template <typename>
            static std::false_type Test(...);

        public:
            static const bool value = decltype(Test<T>(0))::value;
        };

        inline std::string ValueToString(const std::string &value)
        {
            return "\"" + EscapeString(value) + "\"";
        }

        inline std::string ValueToString(const char *value)
        {
            return value == nullptr ? "nullptr" : ValueToString(std::string(value));
        }

        inline std::string ValueToString(char *value)
        {
            return ValueToString(static_cast<const char *>(value));
        }

        template <std::size_t N>
        std::string ValueToString(const char (&value)[N])
        {
            return ValueToString(static_cast<const char *>(value));
        }

        inline std::string ValueToString(const std::wstring &value)
        {
            return "L\"" + EscapeString(Narrow(value)) + "\"";
        }

        inline std::string ValueToString(const wchar_t *value)
        {
            return value == nullptr ? "nullptr" : ValueToString(std::wstring(value));
        }

        inline std::string ValueToString(wchar_t *value)
        {
            return ValueToString(static_cast<const wchar_t *>(value));
        }

        template <std::size_t N>
        std::string ValueToString(const wchar_t (&value)[N])
        {
            return ValueToString(static_cast<const wchar_t *>(value));
        }

        inline std::string ValueToString(std::nullptr_t)
        {
            return "nullptr";
        }

        template <typename T>
        typename std::enable_if<IsOstreamWritable<T>::value, std::string>::type ValueToString(const T &value)
        {
            std::ostringstream oss;
            oss << std::boolalpha << value;
            return oss.str();
        }

        template <typename T>
        typename std::enable_if<!IsOstreamWritable<T>::value && std::is_enum<T>::value, std::string>::type ValueToString(
            const T &value)
        {
            using Underlying = typename std::underlying_type<T>::type;
            return ValueToString(static_cast<Underlying>(value));
        }

        template <typename T>
        typename std::enable_if<!IsOstreamWritable<T>::value && !std::is_enum<T>::value, std::string>::type ValueToString(
            const T &)
        {
            return "<unprintable>";
        }

        inline std::string FormatFailure(const Failure &failure)
        {
            std::ostringstream oss;
            oss << failure.file << ":" << failure.line << ": " << failure.kind << " failed";
            if (!failure.expression.empty()) {
                oss << ": " << failure.expression;
            }
            if (!failure.detail.empty()) {
                oss << " (" << failure.detail << ")";
            }
            return oss.str();
        }

        inline Failure MakeFailure(
            const char *kind,
            const std::string &expression,
            const char *file,
            int line,
            const std::string &detail,
            bool fatal)
        {
            Failure failure;
            failure.kind       = kind == nullptr ? "ASSERT" : kind;
            failure.expression = expression;
            failure.file       = file == nullptr ? "<unknown>" : file;
            failure.line       = line;
            failure.detail     = detail;
            failure.fatal      = fatal;
            return failure;
        }

        inline void ReportFailure(
            const char *kind,
            const std::string &expression,
            const char *file,
            int line,
            const std::string &detail,
            bool fatal)
        {
            auto failure        = MakeFailure(kind, expression, file, line, detail, fatal);
            const auto message  = FormatFailure(failure);
            TestResult *current = CurrentResult();

            if (current != nullptr) {
                current->failures.push_back(failure);
            }

            if (fatal) {
                throw FatalAssertionFailure(message);
            }
            if (current == nullptr) {
                throw AssertionFailure(message);
            }
        }

        inline std::string FormatBinaryDetail(
            const std::string &leftName,
            const std::string &leftValue,
            const std::string &rightName,
            const std::string &rightValue)
        {
            std::ostringstream oss;
            oss << leftName << ": " << leftValue << ", " << rightName << ": " << rightValue;
            return oss.str();
        }

        inline std::string JoinExpression(const char *leftExpr, const char *op, const char *rightExpr)
        {
            std::ostringstream oss;
            oss << leftExpr << " " << op << " " << rightExpr;
            return oss.str();
        }

        inline std::string FormatDuration(double milliseconds)
        {
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(2) << milliseconds << " ms";
            return oss.str();
        }

        inline bool StartsWith(const std::string &value, const char *prefix)
        {
            const auto length = std::strlen(prefix);
            return value.size() >= length && value.compare(0, length, prefix) == 0;
        }

        inline bool MatchesFilter(const TestCase &test, const std::string &filter)
        {
            return filter.empty() || std::string(test.name).find(filter) != std::string::npos;
        }
    }

    inline void Fail(
        const char *kind,
        const char *expr,
        const char *file,
        int line,
        const std::string &detail = {})
    {
        Detail::CountAssertion();
        Detail::ReportFailure(kind, expr == nullptr ? "" : expr, file, line, detail, true);
    }

    inline void Fail(const char *message, const char *file, int line)
    {
        Fail("FAIL", message, file, line);
    }

    inline void Check(bool condition, const char *expr, const char *file, int line)
    {
        Detail::CountAssertion();
        if (!condition) {
            Detail::ReportFailure("CHECK", expr, file, line, {}, false);
        }
    }

    inline void Require(bool condition, const char *expr, const char *file, int line)
    {
        Detail::CountAssertion();
        if (!condition) {
            Detail::ReportFailure("REQUIRE", expr, file, line, {}, true);
        }
    }

    inline void CheckFalse(bool condition, const char *expr, const char *file, int line)
    {
        Detail::CountAssertion();
        if (condition) {
            Detail::ReportFailure("CHECK_FALSE", expr, file, line, "expected false", false);
        }
    }

    inline void RequireFalse(bool condition, const char *expr, const char *file, int line)
    {
        Detail::CountAssertion();
        if (condition) {
            Detail::ReportFailure("REQUIRE_FALSE", expr, file, line, "expected false", true);
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
        Detail::CountAssertion();
        if (!(expected == actual)) {
            Detail::ReportFailure(
                "CHECK_EQ",
                Detail::JoinExpression(expectedExpr, "==", actualExpr),
                file,
                line,
                Detail::FormatBinaryDetail(
                    "expected",
                    Detail::ValueToString(expected),
                    "actual",
                    Detail::ValueToString(actual)),
                false);
        }
    }

    template <typename TExpected, typename TActual>
    void RequireEqual(
        const TExpected &expected,
        const TActual &actual,
        const char *expectedExpr,
        const char *actualExpr,
        const char *file,
        int line)
    {
        Detail::CountAssertion();
        if (!(expected == actual)) {
            Detail::ReportFailure(
                "REQUIRE_EQ",
                Detail::JoinExpression(expectedExpr, "==", actualExpr),
                file,
                line,
                Detail::FormatBinaryDetail(
                    "expected",
                    Detail::ValueToString(expected),
                    "actual",
                    Detail::ValueToString(actual)),
                true);
        }
    }

    template <typename TLeft, typename TRight>
    void CheckNotEqual(
        const TLeft &left,
        const TRight &right,
        const char *leftExpr,
        const char *rightExpr,
        const char *file,
        int line)
    {
        Detail::CountAssertion();
        if (!(left != right)) {
            Detail::ReportFailure(
                "CHECK_NE",
                Detail::JoinExpression(leftExpr, "!=", rightExpr),
                file,
                line,
                Detail::FormatBinaryDetail("left", Detail::ValueToString(left), "right", Detail::ValueToString(right)),
                false);
        }
    }

    template <typename TLeft, typename TRight>
    void RequireNotEqual(
        const TLeft &left,
        const TRight &right,
        const char *leftExpr,
        const char *rightExpr,
        const char *file,
        int line)
    {
        Detail::CountAssertion();
        if (!(left != right)) {
            Detail::ReportFailure(
                "REQUIRE_NE",
                Detail::JoinExpression(leftExpr, "!=", rightExpr),
                file,
                line,
                Detail::FormatBinaryDetail("left", Detail::ValueToString(left), "right", Detail::ValueToString(right)),
                true);
        }
    }

    template <typename TLeft, typename TRight>
    void CheckLess(
        const TLeft &left,
        const TRight &right,
        const char *leftExpr,
        const char *rightExpr,
        const char *file,
        int line)
    {
        Detail::CountAssertion();
        if (!(left < right)) {
            Detail::ReportFailure(
                "CHECK_LT",
                Detail::JoinExpression(leftExpr, "<", rightExpr),
                file,
                line,
                Detail::FormatBinaryDetail("left", Detail::ValueToString(left), "right", Detail::ValueToString(right)),
                false);
        }
    }

    template <typename TLeft, typename TRight>
    void CheckLessOrEqual(
        const TLeft &left,
        const TRight &right,
        const char *leftExpr,
        const char *rightExpr,
        const char *file,
        int line)
    {
        Detail::CountAssertion();
        if (!(left <= right)) {
            Detail::ReportFailure(
                "CHECK_LE",
                Detail::JoinExpression(leftExpr, "<=", rightExpr),
                file,
                line,
                Detail::FormatBinaryDetail("left", Detail::ValueToString(left), "right", Detail::ValueToString(right)),
                false);
        }
    }

    template <typename TLeft, typename TRight>
    void CheckGreater(
        const TLeft &left,
        const TRight &right,
        const char *leftExpr,
        const char *rightExpr,
        const char *file,
        int line)
    {
        Detail::CountAssertion();
        if (!(left > right)) {
            Detail::ReportFailure(
                "CHECK_GT",
                Detail::JoinExpression(leftExpr, ">", rightExpr),
                file,
                line,
                Detail::FormatBinaryDetail("left", Detail::ValueToString(left), "right", Detail::ValueToString(right)),
                false);
        }
    }

    template <typename TLeft, typename TRight>
    void CheckGreaterOrEqual(
        const TLeft &left,
        const TRight &right,
        const char *leftExpr,
        const char *rightExpr,
        const char *file,
        int line)
    {
        Detail::CountAssertion();
        if (!(left >= right)) {
            Detail::ReportFailure(
                "CHECK_GE",
                Detail::JoinExpression(leftExpr, ">=", rightExpr),
                file,
                line,
                Detail::FormatBinaryDetail("left", Detail::ValueToString(left), "right", Detail::ValueToString(right)),
                false);
        }
    }

    template <typename TException, typename TFunc>
    void CheckThrowsAs(TFunc func, const char *expr, const char *exceptionName, const char *file, int line)
    {
        Detail::CountAssertion();
        try {
            func();
        } catch (const TException &) {
            return;
        } catch (const std::exception &ex) {
            std::ostringstream oss;
            oss << "expected " << exceptionName << ", got std::exception: " << ex.what();
            Detail::ReportFailure("CHECK_THROWS_AS", expr, file, line, oss.str(), false);
            return;
        } catch (...) {
            std::ostringstream oss;
            oss << "expected " << exceptionName << ", got non-standard exception";
            Detail::ReportFailure("CHECK_THROWS_AS", expr, file, line, oss.str(), false);
            return;
        }

        std::ostringstream oss;
        oss << "expected " << exceptionName << ", no exception thrown";
        Detail::ReportFailure("CHECK_THROWS_AS", expr, file, line, oss.str(), false);
    }

    template <typename TException, typename TFunc>
    void RequireThrowsAs(TFunc func, const char *expr, const char *exceptionName, const char *file, int line)
    {
        Detail::CountAssertion();
        try {
            func();
        } catch (const TException &) {
            return;
        } catch (const std::exception &ex) {
            std::ostringstream oss;
            oss << "expected " << exceptionName << ", got std::exception: " << ex.what();
            Detail::ReportFailure("REQUIRE_THROWS_AS", expr, file, line, oss.str(), true);
            return;
        } catch (...) {
            std::ostringstream oss;
            oss << "expected " << exceptionName << ", got non-standard exception";
            Detail::ReportFailure("REQUIRE_THROWS_AS", expr, file, line, oss.str(), true);
            return;
        }

        std::ostringstream oss;
        oss << "expected " << exceptionName << ", no exception thrown";
        Detail::ReportFailure("REQUIRE_THROWS_AS", expr, file, line, oss.str(), true);
    }

    template <typename TFunc>
    void CheckNoThrow(TFunc func, const char *expr, const char *file, int line)
    {
        Detail::CountAssertion();
        try {
            func();
        } catch (const std::exception &ex) {
            Detail::ReportFailure("CHECK_NOTHROW", expr, file, line, ex.what(), false);
        } catch (...) {
            Detail::ReportFailure("CHECK_NOTHROW", expr, file, line, "non-standard exception", false);
        }
    }

    template <typename TFunc>
    void RequireNoThrow(TFunc func, const char *expr, const char *file, int line)
    {
        Detail::CountAssertion();
        try {
            func();
        } catch (const std::exception &ex) {
            Detail::ReportFailure("REQUIRE_NOTHROW", expr, file, line, ex.what(), true);
        } catch (...) {
            Detail::ReportFailure("REQUIRE_NOTHROW", expr, file, line, "non-standard exception", true);
        }
    }

    inline TestResult RunOne(const TestCase &test)
    {
        TestResult result;
        result.test = &test;

        const auto started = std::chrono::steady_clock::now();
        {
            Detail::ScopedCurrentResult context(result);
            try {
                test.func();
            } catch (const FatalAssertionFailure &) {
            } catch (const AssertionFailure &ex) {
                if (result.failures.empty()) {
                    result.failures.push_back(Detail::MakeFailure(
                        "ASSERTION",
                        "unexpected assertion failure",
                        test.file,
                        test.line,
                        ex.what(),
                        true));
                }
            } catch (const std::exception &ex) {
                result.failures.push_back(Detail::MakeFailure(
                    "EXCEPTION",
                    "unexpected exception",
                    test.file,
                    test.line,
                    ex.what(),
                    true));
            } catch (...) {
                result.failures.push_back(Detail::MakeFailure(
                    "EXCEPTION",
                    "unexpected exception",
                    test.file,
                    test.line,
                    "non-standard exception",
                    true));
            }
        }

        const auto ended = std::chrono::steady_clock::now();
        result.elapsedMilliseconds =
            std::chrono::duration<double, std::milli>(ended - started).count();
        return result;
    }

    inline void PrintRegisteredTests(std::ostream &out)
    {
        const auto &tests = Registry();
        for (std::size_t i = 0; i < tests.size(); ++i) {
            out << i + 1 << ". " << tests[i].name;
            if (tests[i].file != nullptr) {
                out << " (" << tests[i].file << ":" << tests[i].line << ")";
            }
            out << "\n";
        }
    }

    inline TestRunSummary RunTests(
        const TestOptions &options,
        std::ostream &out = std::cout,
        std::ostream &err = std::cerr)
    {
        TestRunSummary summary;
        summary.total = static_cast<int>(Registry().size());
        const auto started = std::chrono::steady_clock::now();

        for (const auto &test : Registry()) {
            if (!Detail::MatchesFilter(test, options.filter)) {
                continue;
            }

            ++summary.selected;
            out << "[ RUN      ] " << test.name << "\n";

            const auto result = RunOne(test);
            summary.assertions += result.assertions;

            if (result.Passed()) {
                ++summary.passed;
                out << "[       OK ] " << test.name << " ("
                    << Detail::FormatDuration(result.elapsedMilliseconds) << ")\n";
            } else {
                ++summary.failed;
                err << "[  FAILED  ] " << test.name << " ("
                    << Detail::FormatDuration(result.elapsedMilliseconds) << ")\n";
                for (const auto &failure : result.failures) {
                    err << "    " << Detail::FormatFailure(failure) << "\n";
                }
            }
        }

        const auto ended = std::chrono::steady_clock::now();
        summary.elapsedMilliseconds =
            std::chrono::duration<double, std::milli>(ended - started).count();

        out << "\n";
        if (summary.selected == 0) {
            err << "No tests selected";
            if (!options.filter.empty()) {
                err << " by filter \"" << options.filter << "\"";
            }
            err << ".\n";
        }
        out << "Test summary: " << summary.passed << " passed, " << summary.failed
            << " failed, " << summary.selected << " selected, " << summary.total
            << " registered, " << summary.assertions << " assertions ("
            << Detail::FormatDuration(summary.elapsedMilliseconds) << ")\n";
        return summary;
    }

    inline void PrintUsage(std::ostream &out, const char *program)
    {
        out << "Usage: " << program << " [--list] [--filter <substring>]\n"
            << "  --list                 List registered tests without running them.\n"
            << "  --filter <substring>   Run tests whose names contain the substring.\n"
            << "  --help                 Show this help message.\n";
    }

    inline int RunAll(int argc, char **argv)
    {
        TestOptions options;
        const char *program = argc > 0 && argv[0] != nullptr ? argv[0] : "sw_unit_tests";

        for (int i = 1; i < argc; ++i) {
            const std::string arg = argv[i] == nullptr ? "" : argv[i];
            if (arg == "--help" || arg == "-h") {
                PrintUsage(std::cout, program);
                return 0;
            }
            if (arg == "--list") {
                options.listOnly = true;
                continue;
            }
            if (arg == "--filter") {
                if (i + 1 >= argc) {
                    std::cerr << "--filter requires a substring.\n";
                    return 2;
                }
                options.filter = argv[++i];
                continue;
            }
            if (Detail::StartsWith(arg, "--filter=")) {
                options.filter = arg.substr(std::strlen("--filter="));
                continue;
            }

            std::cerr << "Unknown option: " << arg << "\n";
            PrintUsage(std::cerr, program);
            return 2;
        }

        if (options.listOnly) {
            PrintRegisteredTests(std::cout);
            return 0;
        }

        const auto summary = RunTests(options);
        return summary.failed == 0 && summary.selected > 0 ? 0 : 1;
    }

    inline int RunAll()
    {
        char program[] = "sw_unit_tests";
        char *argv[]  = {program};
        return RunAll(1, argv);
    }
}

#define SWTEST_CONCAT_IMPL(a, b) a##b
#define SWTEST_CONCAT(a, b) SWTEST_CONCAT_IMPL(a, b)

#define TEST_CASE(name)                                                                    \
    static void SWTEST_CONCAT(SwTestFunc_, __LINE__)();                                    \
    static ::swtest::Registrar SWTEST_CONCAT(SwTestRegistrar_, __LINE__)(                  \
        name, &SWTEST_CONCAT(SwTestFunc_, __LINE__), __FILE__, __LINE__);                  \
    static void SWTEST_CONCAT(SwTestFunc_, __LINE__)()

#define CHECK(expr) ::swtest::Check(static_cast<bool>(expr), #expr, __FILE__, __LINE__)
#define REQUIRE(expr) ::swtest::Require(static_cast<bool>(expr), #expr, __FILE__, __LINE__)
#define CHECK_FALSE(expr) ::swtest::CheckFalse(static_cast<bool>(expr), #expr, __FILE__, __LINE__)
#define REQUIRE_FALSE(expr) ::swtest::RequireFalse(static_cast<bool>(expr), #expr, __FILE__, __LINE__)
#define CHECK_EQ(expected, actual) \
    ::swtest::CheckEqual((expected), (actual), #expected, #actual, __FILE__, __LINE__)
#define REQUIRE_EQ(expected, actual) \
    ::swtest::RequireEqual((expected), (actual), #expected, #actual, __FILE__, __LINE__)
#define CHECK_NE(left, right) \
    ::swtest::CheckNotEqual((left), (right), #left, #right, __FILE__, __LINE__)
#define REQUIRE_NE(left, right) \
    ::swtest::RequireNotEqual((left), (right), #left, #right, __FILE__, __LINE__)
#define CHECK_LT(left, right) \
    ::swtest::CheckLess((left), (right), #left, #right, __FILE__, __LINE__)
#define CHECK_LE(left, right) \
    ::swtest::CheckLessOrEqual((left), (right), #left, #right, __FILE__, __LINE__)
#define CHECK_GT(left, right) \
    ::swtest::CheckGreater((left), (right), #left, #right, __FILE__, __LINE__)
#define CHECK_GE(left, right) \
    ::swtest::CheckGreaterOrEqual((left), (right), #left, #right, __FILE__, __LINE__)
#define CHECK_THROWS_AS(expr, exceptionType) \
    ::swtest::CheckThrowsAs<exceptionType>([&]() { (void)(expr); }, #expr, #exceptionType, __FILE__, __LINE__)
#define REQUIRE_THROWS_AS(expr, exceptionType) \
    ::swtest::RequireThrowsAs<exceptionType>([&]() { (void)(expr); }, #expr, #exceptionType, __FILE__, __LINE__)
#define CHECK_NOTHROW(expr) \
    ::swtest::CheckNoThrow([&]() { (void)(expr); }, #expr, __FILE__, __LINE__)
#define REQUIRE_NOTHROW(expr) \
    ::swtest::RequireNoThrow([&]() { (void)(expr); }, #expr, __FILE__, __LINE__)
#define FAIL(message) ::swtest::Fail((message), __FILE__, __LINE__)
