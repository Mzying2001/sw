#pragma once

#include "IValueConverter.h"
#include "Utils.h"
#include <cwchar>

// clang-format off

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4996) // make sscanf great again
#endif

// clang-format on

namespace sw
{
    /**
     * @brief 反向转换器模板声明
     * @tparam TBaseConverter 基础转换器类型
     */
    template <typename TBaseConverter, typename = void>
    class ReverseConverter;

    /**
     * @brief 反向转换器，将已有转换器的转换方向反转
     * @tparam TBaseConverter 基础转换器类型
     */
    template <typename TBaseConverter>
    class ReverseConverter<
        TBaseConverter,
        typename std::enable_if<
            std::is_base_of<
                IValueConverter<
                    typename std::decay<typename TBaseConverter::TSourceParam>::type,
                    typename std::decay<typename TBaseConverter::TTargetParam>::type>,
                TBaseConverter>::value>::type>
        : public IValueConverter<
              typename std::decay<typename TBaseConverter::TTargetParam>::type,
              typename std::decay<typename TBaseConverter::TSourceParam>::type>
    {
    public:
        using TSourceParam = typename TBaseConverter::TTargetParam;
        using TTargetParam = typename TBaseConverter::TSourceParam;
        using TSource      = typename std::decay<TSourceParam>::type;
        using TTarget      = typename std::decay<TTargetParam>::type;

        template <typename... Args>
        ReverseConverter(Args &&...args)
            : baseConverter(std::forward<Args>(args)...)
        {
        }
        virtual TTarget Convert(TSourceParam source) override
        {
            return baseConverter.ConvertBack(source);
        }
        virtual TSource ConvertBack(TTargetParam target) override
        {
            return baseConverter.Convert(target);
        }

    private:
        TBaseConverter baseConverter;
    };

    /*================================================================================*/

    /**
     * @brief 数值类型转换器，将源数值类型转换为目标数值类型
     * @tparam TSource 源数值类型
     * @tparam TTarget 目标数值类型
     */
    template <
        typename TSource,
        typename TTarget,
        typename std::enable_if<std::is_arithmetic<TSource>::value && std::is_arithmetic<TTarget>::value, int>::type = 0>
    class NumericConverter : public IValueConverter<TSource, TTarget>
    {
    public:
        virtual TTarget Convert(TSource source) override
        {
            return static_cast<TTarget>(source);
        }
        virtual TSource ConvertBack(TTarget target) override
        {
            return static_cast<TSource>(target);
        }
    };

    using IntToFloatConverter  = NumericConverter<int, float>;
    using IntToDoubleConverter = NumericConverter<int, double>;
    using FloatToIntConverter  = NumericConverter<float, int>;
    using DoubleToIntConverter = NumericConverter<double, int>;

    /*================================================================================*/

    /**
     * @brief 数值取反转换器
     * @tparam T 数值类型
     */
    template <
        typename T,
        typename std::enable_if<std::is_arithmetic<T>::value, int>::type = 0>
    class NumericNegationConverter : public IValueConverter<T, T>
    {
    public:
        virtual T Convert(T source) override
        {
            return -source;
        }
        virtual T ConvertBack(T target) override
        {
            return -target;
        }
    };

    /*================================================================================*/

    /**
     * @brief 布尔值取反转换器
     */
    class BoolNegationConverter : public IValueConverter<bool, bool>
    {
    public:
        virtual bool Convert(bool source) override
        {
            return !source;
        }
        virtual bool ConvertBack(bool target) override
        {
            return !target;
        }
    };

    /**
     * @brief 数值与布尔值转换器，非零数值转换为true，零数值转换为false
     * @tparam TSource 数值类型
     */
    template <
        typename TSource,
        typename std::enable_if<std::is_arithmetic<TSource>::value, int>::type = 0>
    class NumericToBoolConverter : public IValueConverter<TSource, bool>
    {
    public:
        virtual bool Convert(TSource source) override
        {
            return source != static_cast<TSource>(0);
        }
        virtual TSource ConvertBack(bool target) override
        {
            return target ? static_cast<TSource>(1) : static_cast<TSource>(0);
        }
    };

    /**
     * @brief 布尔值与数值转换器，true转换为1，false转换为0
     * @tparam TTarget 数值类型
     */
    template <
        typename TTarget,
        typename std::enable_if<std::is_arithmetic<TTarget>::value, int>::type = 0>
    class BoolToNumericConverter : public IValueConverter<bool, TTarget>
    {
    public:
        virtual TTarget Convert(bool source) override
        {
            return source ? static_cast<TTarget>(1) : static_cast<TTarget>(0);
        }
        virtual bool ConvertBack(TTarget target) override
        {
            return target != static_cast<TTarget>(0);
        }
    };

    using BoolToIntConverter = BoolToNumericConverter<int>;
    using IntToBoolConverter = NumericToBoolConverter<int>;

    /*================================================================================*/

    /**
     * @brief 整数与字符串转换器
     */
    class IntToStringConverter : public IValueConverter<int, std::wstring>
    {
    public:
        explicit IntToStringConverter(int defaultValue = 0)
            : defaultValue(defaultValue)
        {
        }
        virtual std::wstring Convert(int source) override
        {
            return std::to_wstring(source);
        }
        virtual int ConvertBack(const std::wstring &target) override
        {
            int result;
            bool success = std::swscanf(target.c_str(), L"%d", &result) == 1;
            return success ? result : defaultValue;
        }

    private:
        int defaultValue;
    };

    /**
     * @brief 浮点数与字符串转换器
     */
    class FloatToStringConverter : public IValueConverter<float, std::wstring>
    {
    public:
        explicit FloatToStringConverter(float defaultValue = 0.0f)
            : defaultValue(defaultValue)
        {
        }
        virtual std::wstring Convert(float source) override
        {
            std::wstring result(32, L'\0');
            int len = std::swprintf(&result[0], result.size(), L"%g", source);
            result.resize(len > 0 ? len : 0);
            return result;
        }
        virtual float ConvertBack(const std::wstring &target) override
        {
            float result;
            bool success = std::swscanf(target.c_str(), L"%f", &result) == 1;
            return success ? result : defaultValue;
        }

    private:
        float defaultValue;
    };

    /**
     * @brief 双精度浮点数与字符串转换器
     */
    class DoubleToStringConverter : public IValueConverter<double, std::wstring>
    {
    public:
        explicit DoubleToStringConverter(double defaultValue = 0.0)
            : defaultValue(defaultValue)
        {
        }
        virtual std::wstring Convert(double source) override
        {
            std::wstring result(32, L'\0');
            int len = std::swprintf(&result[0], result.size(), L"%g", source);
            result.resize(len > 0 ? len : 0);
            return result;
        }
        virtual double ConvertBack(const std::wstring &target) override
        {
            double result;
            bool success = std::swscanf(target.c_str(), L"%lf", &result) == 1;
            return success ? result : defaultValue;
        }

    private:
        double defaultValue;
    };

    /**
     * @brief 字符串与整数转换器
     */
    class StringToIntConverter : public IValueConverter<std::wstring, int>
    {
    public:
        explicit StringToIntConverter(int defaultValue = 0)
            : defaultValue(defaultValue)
        {
        }
        virtual int Convert(const std::wstring &source) override
        {
            int result;
            bool success = std::swscanf(source.c_str(), L"%d", &result) == 1;
            return success ? result : defaultValue;
        }
        virtual std::wstring ConvertBack(int target) override
        {
            return std::to_wstring(target);
        }

    private:
        int defaultValue;
    };

    /**
     * @brief 字符串与浮点数转换器
     */
    class StringToFloatConverter : public IValueConverter<std::wstring, float>
    {
    public:
        explicit StringToFloatConverter(float defaultValue = 0.0f)
            : defaultValue(defaultValue)
        {
        }
        virtual float Convert(const std::wstring &source) override
        {
            float result;
            bool success = std::swscanf(source.c_str(), L"%f", &result) == 1;
            return success ? result : defaultValue;
        }
        virtual std::wstring ConvertBack(float target) override
        {
            std::wstring result(32, L'\0');
            int len = std::swprintf(&result[0], result.size(), L"%g", target);
            result.resize(len > 0 ? len : 0);
            return result;
        }

    private:
        float defaultValue;
    };

    /**
     * @brief 字符串与双精度浮点数转换器
     */
    class StringToDoubleConverter : public IValueConverter<std::wstring, double>
    {
    public:
        explicit StringToDoubleConverter(double defaultValue = 0.0)
            : defaultValue(defaultValue)
        {
        }
        virtual double Convert(const std::wstring &source) override
        {
            double result;
            bool success = std::swscanf(source.c_str(), L"%lf", &result) == 1;
            return success ? result : defaultValue;
        }
        virtual std::wstring ConvertBack(double target) override
        {
            std::wstring result(32, L'\0');
            int len = std::swprintf(&result[0], result.size(), L"%g", target);
            result.resize(len > 0 ? len : 0);
            return result;
        }

    private:
        double defaultValue;
    };

    /*================================================================================*/

    /**
     * @brief 整数与窄字符串转换器
     */
    class IntToAnsiStringConverter : public IValueConverter<int, std::string>
    {
    public:
        explicit IntToAnsiStringConverter(int defaultValue = 0)
            : defaultValue(defaultValue)
        {
        }
        virtual std::string Convert(int source) override
        {
            return std::to_string(source);
        }
        virtual int ConvertBack(const std::string &target) override
        {
            int result;
            bool success = std::sscanf(target.c_str(), "%d", &result) == 1;
            return success ? result : defaultValue;
        }

    private:
        int defaultValue;
    };

    /**
     * @brief 浮点数与窄字符串转换器
     */
    class FloatToAnsiStringConverter : public IValueConverter<float, std::string>
    {
    public:
        explicit FloatToAnsiStringConverter(float defaultValue = 0.0f)
            : defaultValue(defaultValue)
        {
        }
        virtual std::string Convert(float source) override
        {
            std::string result(32, '\0');
            int len = std::snprintf(&result[0], result.size(), "%g", source);
            result.resize(len > 0 ? len : 0);
            return result;
        }
        virtual float ConvertBack(const std::string &target) override
        {
            float result;
            bool success = std::sscanf(target.c_str(), "%f", &result) == 1;
            return success ? result : defaultValue;
        }

    private:
        float defaultValue;
    };

    /**
     * @brief 双精度浮点数与窄字符串转换器
     */
    class DoubleToAnsiStringConverter : public IValueConverter<double, std::string>
    {
    public:
        explicit DoubleToAnsiStringConverter(double defaultValue = 0.0)
            : defaultValue(defaultValue)
        {
        }
        virtual std::string Convert(double source) override
        {
            std::string result(32, '\0');
            int len = std::snprintf(&result[0], result.size(), "%g", source);
            result.resize(len > 0 ? len : 0);
            return result;
        }
        virtual double ConvertBack(const std::string &target) override
        {
            double result;
            bool success = std::sscanf(target.c_str(), "%lf", &result) == 1;
            return success ? result : defaultValue;
        }

    private:
        double defaultValue;
    };

    /**
     * @brief 窄字符串与整数转换器
     */
    class AnsiStringToIntConverter : public IValueConverter<std::string, int>
    {
    public:
        explicit AnsiStringToIntConverter(int defaultValue = 0)
            : defaultValue(defaultValue)
        {
        }
        virtual int Convert(const std::string &source) override
        {
            int result;
            bool success = std::sscanf(source.c_str(), "%d", &result) == 1;
            return success ? result : defaultValue;
        }
        virtual std::string ConvertBack(int target) override
        {
            return std::to_string(target);
        }

    private:
        int defaultValue;
    };

    /**
     * @brief 窄字符串与浮点数转换器
     */
    class AnsiStringToFloatConverter : public IValueConverter<std::string, float>
    {
    public:
        explicit AnsiStringToFloatConverter(float defaultValue = 0.0f)
            : defaultValue(defaultValue)
        {
        }
        virtual float Convert(const std::string &source) override
        {
            float result;
            bool success = std::sscanf(source.c_str(), "%f", &result) == 1;
            return success ? result : defaultValue;
        }
        virtual std::string ConvertBack(float target) override
        {
            std::string result(32, '\0');
            int len = std::snprintf(&result[0], result.size(), "%g", target);
            result.resize(len > 0 ? len : 0);
            return result;
        }

    private:
        float defaultValue;
    };

    /**
     * @brief 窄字符串与双精度浮点数转换器
     */
    class AnsiStringToDoubleConverter : public IValueConverter<std::string, double>
    {
    public:
        explicit AnsiStringToDoubleConverter(double defaultValue = 0.0)
            : defaultValue(defaultValue)
        {
        }
        virtual double Convert(const std::string &source) override
        {
            double result;
            bool success = std::sscanf(source.c_str(), "%lf", &result) == 1;
            return success ? result : defaultValue;
        }
        virtual std::string ConvertBack(double target) override
        {
            std::string result(32, '\0');
            int len = std::snprintf(&result[0], result.size(), "%g", target);
            result.resize(len > 0 ? len : 0);
            return result;
        }

    private:
        double defaultValue;
    };

    /*================================================================================*/

    /**
     * @brief 宽字符串与窄字符串转换器
     */
    class StringToAnsiStringConverter : public IValueConverter<std::wstring, std::string>
    {
    public:
        virtual std::string Convert(const std::wstring &source) override
        {
            return Utils::ToMultiByteStr(source);
        }
        virtual std::wstring ConvertBack(const std::string &target) override
        {
            return Utils::ToWideStr(target);
        }
    };

    /**
     * @brief 窄字符串与宽字符串转换器
     */
    class AnsiStringToStringConverter : public IValueConverter<std::string, std::wstring>
    {
    public:
        virtual std::wstring Convert(const std::string &source) override
        {
            return Utils::ToWideStr(source);
        }
        virtual std::string ConvertBack(const std::wstring &target) override
        {
            return Utils::ToMultiByteStr(target);
        }
    };

    /**
     * @brief 宽字符串与UTF-8编码窄字符串转换器
     */
    class StringToUtf8StringConverter : public IValueConverter<std::wstring, std::string>
    {
    public:
        virtual std::string Convert(const std::wstring &source) override
        {
            return Utils::ToMultiByteStr(source, true);
        }
        virtual std::wstring ConvertBack(const std::string &target) override
        {
            return Utils::ToWideStr(target, true);
        }
    };

    /**
     * @brief UTF-8编码窄字符串与宽字符串转换器
     */
    class Utf8StringToStringConverter : public IValueConverter<std::string, std::wstring>
    {
    public:
        virtual std::wstring Convert(const std::string &source) override
        {
            return Utils::ToWideStr(source, true);
        }
        virtual std::string ConvertBack(const std::wstring &target) override
        {
            return Utils::ToMultiByteStr(target, true);
        }
    };
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif
