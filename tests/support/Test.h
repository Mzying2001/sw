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

        /**
         * @brief 判断当前测试用例是否通过。
         * @return 没有记录任何失败时返回true。
         */
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
        /**
         * @brief 创建注册器并将测试用例加入全局注册表。
         * @param name 测试用例名称
         * @param func 测试用例入口函数
         * @param file 定义测试用例的源文件
         * @param line 定义测试用例的行号
         */
        Registrar(const char *name, void (*func)(), const char *file, int line)
        {
            Registry().push_back(TestCase{name, func, file, line});
        }

        /**
         * @brief 兼容旧调用方式的注册构造函数。
         * @param name 测试用例名称
         * @param func 测试用例入口函数
         */
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
        /**
         * @brief 获取当前正在执行的测试结果指针。
         *
         * 断言函数通过该指针把失败和断言数量写回当前用例；当断言在测试运行器外被调用时，
         * 该指针为空，此时失败会直接以异常形式向外抛出。
         */
        inline TestResult *&CurrentResult()
        {
            static TestResult *result = nullptr;
            return result;
        }

        /**
         * @brief 在当前作用域内切换CurrentResult，并在离开作用域时恢复旧值。
         */
        class ScopedCurrentResult
        {
        public:
            /**
             * @brief 进入指定测试用例的断言记录上下文。
             * @param result 当前测试用例的执行结果对象
             */
            explicit ScopedCurrentResult(TestResult &result)
                : _previous(CurrentResult())
            {
                CurrentResult() = &result;
            }

            /**
             * @brief 恢复进入作用域前的测试结果上下文。
             */
            ~ScopedCurrentResult()
            {
                CurrentResult() = _previous;
            }

        private:
            TestResult *_previous;
        };

        /**
         * @brief 为当前测试用例增加一次断言计数。
         */
        inline void CountAssertion()
        {
            if (CurrentResult() != nullptr) {
                ++CurrentResult()->assertions;
            }
        }

        /**
         * @brief 将宽字符串转换为UTF-8窄字符串，便于在测试失败信息中输出。
         * @param value 需要转换的宽字符串
         * @return UTF-8编码的字符串；转换失败时用问号替代非ASCII字符。
         */
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

        /**
         * @brief 转义字符串中的控制字符和引号，生成适合诊断输出的文本。
         * @param value 原始字符串
         * @return 转义后的字符串内容，不包含外层引号。
         */
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

        /**
         * @brief 判断类型T是否可以直接写入std::ostream。
         */
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

        /**
         * @brief 将std::string格式化为带引号的诊断文本。
         */
        inline std::string ValueToString(const std::string &value)
        {
            return "\"" + EscapeString(value) + "\"";
        }

        /**
         * @brief 将窄字符串指针格式化为诊断文本。
         */
        inline std::string ValueToString(const char *value)
        {
            return value == nullptr ? "nullptr" : ValueToString(std::string(value));
        }

        /**
         * @brief 将可变窄字符串指针格式化为诊断文本。
         */
        inline std::string ValueToString(char *value)
        {
            return ValueToString(static_cast<const char *>(value));
        }

        /**
         * @brief 将窄字符串字面量或字符数组格式化为诊断文本。
         */
        template <std::size_t N>
        std::string ValueToString(const char (&value)[N])
        {
            return ValueToString(static_cast<const char *>(value));
        }

        /**
         * @brief 将std::wstring格式化为带L前缀和引号的诊断文本。
         */
        inline std::string ValueToString(const std::wstring &value)
        {
            return "L\"" + EscapeString(Narrow(value)) + "\"";
        }

        /**
         * @brief 将宽字符串指针格式化为诊断文本。
         */
        inline std::string ValueToString(const wchar_t *value)
        {
            return value == nullptr ? "nullptr" : ValueToString(std::wstring(value));
        }

        /**
         * @brief 将可变宽字符串指针格式化为诊断文本。
         */
        inline std::string ValueToString(wchar_t *value)
        {
            return ValueToString(static_cast<const wchar_t *>(value));
        }

        /**
         * @brief 将宽字符串字面量或字符数组格式化为诊断文本。
         */
        template <std::size_t N>
        std::string ValueToString(const wchar_t (&value)[N])
        {
            return ValueToString(static_cast<const wchar_t *>(value));
        }

        /**
         * @brief 将空指针格式化为诊断文本。
         */
        inline std::string ValueToString(std::nullptr_t)
        {
            return "nullptr";
        }

        /**
         * @brief 将可写入std::ostream的值格式化为诊断文本。
         */
        template <typename T>
        typename std::enable_if<IsOstreamWritable<T>::value, std::string>::type ValueToString(const T &value)
        {
            std::ostringstream oss;
            oss << std::boolalpha << value;
            return oss.str();
        }

        /**
         * @brief 将不可直接输出的枚举值按底层整数类型格式化。
         */
        template <typename T>
        typename std::enable_if<!IsOstreamWritable<T>::value && std::is_enum<T>::value, std::string>::type ValueToString(
            const T &value)
        {
            using Underlying = typename std::underlying_type<T>::type;
            return ValueToString(static_cast<Underlying>(value));
        }

        /**
         * @brief 为不可输出的非枚举类型生成占位诊断文本。
         */
        template <typename T>
        typename std::enable_if<!IsOstreamWritable<T>::value && !std::is_enum<T>::value, std::string>::type ValueToString(
            const T &)
        {
            return "<unprintable>";
        }

        /**
         * @brief 将失败记录格式化为单行可读消息。
         * @param failure 失败记录
         * @return 包含文件、行号、断言类型、表达式和详情的消息。
         */
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

        /**
         * @brief 构造一条失败记录，并补齐空指针输入的默认文本。
         */
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

        /**
         * @brief 记录一次断言失败，并按断言类型决定是否抛出异常。
         *
         * 非致命断言只写入当前测试结果并继续执行；致命断言会抛出FatalAssertionFailure，
         * 由RunOne捕获后结束当前用例。若当前没有测试上下文，任何失败都会抛出异常。
         */
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

        /**
         * @brief 格式化二元比较断言中的左右值详情。
         */
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

        /**
         * @brief 拼接二元断言的表达式文本。
         */
        inline std::string JoinExpression(const char *leftExpr, const char *op, const char *rightExpr)
        {
            std::ostringstream oss;
            oss << leftExpr << " " << op << " " << rightExpr;
            return oss.str();
        }

        /**
         * @brief 将毫秒耗时格式化为固定两位小数的字符串。
         */
        inline std::string FormatDuration(double milliseconds)
        {
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(2) << milliseconds << " ms";
            return oss.str();
        }

        /**
         * @brief 判断字符串是否以指定前缀开头。
         */
        inline bool StartsWith(const std::string &value, const char *prefix)
        {
            const auto length = std::strlen(prefix);
            return value.size() >= length && value.compare(0, length, prefix) == 0;
        }

        /**
         * @brief 判断测试用例名称是否匹配过滤条件。
         */
        inline bool MatchesFilter(const TestCase &test, const std::string &filter)
        {
            return filter.empty() || std::string(test.name).find(filter) != std::string::npos;
        }
    }

    /**
     * @brief 记录致命失败并终止当前测试用例。
     * @param kind 失败类型
     * @param expr 失败表达式或消息
     * @param file 失败位置所在文件
     * @param line 失败位置所在行号
     * @param detail 额外诊断信息
     */
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

    /**
     * @brief 记录自定义致命失败并终止当前测试用例。
     */
    inline void Fail(const char *message, const char *file, int line)
    {
        Fail("FAIL", message, file, line);
    }

    /**
     * @brief 检查表达式是否为true；失败时记录错误并继续执行当前用例。
     */
    inline void Check(bool condition, const char *expr, const char *file, int line)
    {
        Detail::CountAssertion();
        if (!condition) {
            Detail::ReportFailure("CHECK", expr, file, line, {}, false);
        }
    }

    /**
     * @brief 检查表达式是否为true；失败时立即终止当前用例。
     */
    inline void Require(bool condition, const char *expr, const char *file, int line)
    {
        Detail::CountAssertion();
        if (!condition) {
            Detail::ReportFailure("REQUIRE", expr, file, line, {}, true);
        }
    }

    /**
     * @brief 检查表达式是否为false；失败时记录错误并继续执行当前用例。
     */
    inline void CheckFalse(bool condition, const char *expr, const char *file, int line)
    {
        Detail::CountAssertion();
        if (condition) {
            Detail::ReportFailure("CHECK_FALSE", expr, file, line, "expected false", false);
        }
    }

    /**
     * @brief 检查表达式是否为false；失败时立即终止当前用例。
     */
    inline void RequireFalse(bool condition, const char *expr, const char *file, int line)
    {
        Detail::CountAssertion();
        if (condition) {
            Detail::ReportFailure("REQUIRE_FALSE", expr, file, line, "expected false", true);
        }
    }

    /**
     * @brief 检查两个值是否相等；失败时记录expected/actual并继续执行当前用例。
     */
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

    /**
     * @brief 检查两个值是否相等；失败时记录expected/actual并立即终止当前用例。
     */
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

    /**
     * @brief 检查两个值是否不相等；失败时继续执行当前用例。
     */
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

    /**
     * @brief 检查两个值是否不相等；失败时立即终止当前用例。
     */
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

    /**
     * @brief 检查left < right；失败时继续执行当前用例。
     */
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

    /**
     * @brief 检查left <= right；失败时继续执行当前用例。
     */
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

    /**
     * @brief 检查left > right；失败时继续执行当前用例。
     */
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

    /**
     * @brief 检查left >= right；失败时继续执行当前用例。
     */
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

    /**
     * @brief 检查表达式是否抛出指定异常类型；失败时继续执行当前用例。
     */
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

    /**
     * @brief 检查表达式是否抛出指定异常类型；失败时立即终止当前用例。
     */
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

    /**
     * @brief 检查表达式是否不抛出异常；失败时继续执行当前用例。
     */
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

    /**
     * @brief 检查表达式是否不抛出异常；失败时立即终止当前用例。
     */
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

    /**
     * @brief 执行单个测试用例并返回完整结果。
     * @param test 待执行的测试用例
     * @return 包含失败列表、断言数量和耗时的执行结果。
     */
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

    /**
     * @brief 输出当前注册的全部测试用例。
     * @param out 输出流
     */
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

    /**
     * @brief 按配置运行测试用例并输出测试报告。
     * @param options 运行配置
     * @param out 标准报告输出流
     * @param err 失败详情输出流
     * @return 测试运行汇总信息。
     */
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

    /**
     * @brief 输出命令行用法说明。
     */
    inline void PrintUsage(std::ostream &out, const char *program)
    {
        out << "Usage: " << program << " [--list] [--filter <substring>]\n"
            << "  --list                 List registered tests without running them.\n"
            << "  --filter <substring>   Run tests whose names contain the substring.\n"
            << "  --help                 Show this help message.\n";
    }

    /**
     * @brief 解析命令行参数并运行测试。
     *
     * 支持 --list、--filter <substring>、--filter=<substring> 和 --help。
     *
     * @return 0表示成功；1表示测试失败或没有选中任何测试；2表示参数错误。
     */
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

    /**
     * @brief 使用默认参数运行全部测试。
     */
    inline int RunAll()
    {
        char program[] = "sw_unit_tests";
        char *argv[]  = {program};
        return RunAll(1, argv);
    }
}

#define SWTEST_CONCAT_IMPL(a, b) a##b
#define SWTEST_CONCAT(a, b) SWTEST_CONCAT_IMPL(a, b)

/**
 * @brief 定义并自动注册一个测试用例。
 *
 * name会出现在测试报告和--filter匹配中；宏内部使用当前行号生成静态函数名，
 * 因此同一源文件中不要在同一行写多个TEST_CASE。
 */
#define TEST_CASE(name)                                                   \
    static void SWTEST_CONCAT(SwTestFunc_, __LINE__)();                   \
    static ::swtest::Registrar SWTEST_CONCAT(SwTestRegistrar_, __LINE__)( \
        name, &SWTEST_CONCAT(SwTestFunc_, __LINE__), __FILE__, __LINE__); \
    static void SWTEST_CONCAT(SwTestFunc_, __LINE__)()

// CHECK_*断言失败后继续执行当前测试；REQUIRE_*断言失败后立即结束当前测试。
#define CHECK(expr) \
    ::swtest::Check(static_cast<bool>(expr), #expr, __FILE__, __LINE__)
#define REQUIRE(expr) \
    ::swtest::Require(static_cast<bool>(expr), #expr, __FILE__, __LINE__)
#define CHECK_FALSE(expr) \
    ::swtest::CheckFalse(static_cast<bool>(expr), #expr, __FILE__, __LINE__)
#define REQUIRE_FALSE(expr) \
    ::swtest::RequireFalse(static_cast<bool>(expr), #expr, __FILE__, __LINE__)
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
#define FAIL(message) \
    ::swtest::Fail((message), __FILE__, __LINE__)
