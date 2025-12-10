#pragma once

#include "IValueConverter.h"
#include "Utils.h"

namespace sw
{
    /**
     * @brief 数值类型转换器，将源数值类型转换为目标数值类型
     * @tparam TSource 源数值类型
     * @tparam TTarget 目标数值类型
     */
    template <
        typename TSource,
        typename TTarget,
        std::enable_if<std::is_arithmetic<TSource>::value && std::is_arithmetic<TTarget>::value, int>::type = 0>
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
        std::enable_if<std::is_arithmetic<TSource>::value, int>::type = 0>
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
        std::enable_if<std::is_arithmetic<TTarget>::value, int>::type = 0>
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

    /*================================================================================*/

    /**
     * @brief 整数与字符串转换器
     */
    class IntToStringConverter : public IValueConverter<int, std::wstring>
    {
    public:
        virtual std::wstring Convert(int source) override
        {
            return std::to_wstring(source);
        }
        virtual int ConvertBack(const std::wstring &target) override
        {
            return std::stoi(target);
        }
    };

    /**
     * @brief 浮点数与字符串转换器
     */
    class FloatToStringConverter : public IValueConverter<float, std::wstring>
    {
    public:
        virtual std::wstring Convert(float source) override
        {
            return std::to_wstring(source);
        }
        virtual float ConvertBack(const std::wstring &target) override
        {
            return std::stof(target);
        }
    };

    /**
     * @brief 双精度浮点数与字符串转换器
     */
    class DoubleToStringConverter : public IValueConverter<double, std::wstring>
    {
    public:
        virtual std::wstring Convert(double source) override
        {
            return std::to_wstring(source);
        }
        virtual double ConvertBack(const std::wstring &target) override
        {
            return std::stod(target);
        }
    };

    /**
     * @brief 字符串与整数转换器
     */
    class StringToIntConverter : public IValueConverter<std::wstring, int>
    {
    public:
        virtual int Convert(const std::wstring &source) override
        {
            return std::stoi(source);
        }
        virtual std::wstring ConvertBack(int target) override
        {
            return std::to_wstring(target);
        }
    };

    /**
     * @brief 字符串与浮点数转换器
     */
    class StringToFloatConverter : public IValueConverter<std::wstring, float>
    {
    public:
        virtual float Convert(const std::wstring &source) override
        {
            return std::stof(source);
        }
        virtual std::wstring ConvertBack(float target) override
        {
            return std::to_wstring(target);
        }
    };

    /**
     * @brief 字符串与双精度浮点数转换器
     */
    class StringToDoubleConverter : public IValueConverter<std::wstring, double>
    {
    public:
        virtual double Convert(const std::wstring &source) override
        {
            return std::stod(source);
        }
        virtual std::wstring ConvertBack(double target) override
        {
            return std::to_wstring(target);
        }
    };

    /*================================================================================*/

    /**
     * @brief 整数与窄字符串转换器
     */
    class IntToAnsiStringConverter : public IValueConverter<int, std::string>
    {
    public:
        virtual std::string Convert(int source) override
        {
            return std::to_string(source);
        }
        virtual int ConvertBack(const std::string &target) override
        {
            return std::stoi(target);
        }
    };

    /**
     * @brief 浮点数与窄字符串转换器
     */
    class FloatToAnsiStringConverter : public IValueConverter<float, std::string>
    {
    public:
        virtual std::string Convert(float source) override
        {
            return std::to_string(source);
        }
        virtual float ConvertBack(const std::string &target) override
        {
            return std::stof(target);
        }
    };

    /**
     * @brief 双精度浮点数与窄字符串转换器
     */
    class DoubleToAnsiStringConverter : public IValueConverter<double, std::string>
    {
    public:
        virtual std::string Convert(double source) override
        {
            return std::to_string(source);
        }
        virtual double ConvertBack(const std::string &target) override
        {
            return std::stod(target);
        }
    };

    /**
     * @brief 窄字符串与整数转换器
     */
    class AnsiStringToIntConverter : public IValueConverter<std::string, int>
    {
    public:
        virtual int Convert(const std::string &source) override
        {
            return std::stoi(source);
        }
        virtual std::string ConvertBack(int target) override
        {
            return std::to_string(target);
        }
    };

    /**
     * @brief 窄字符串与浮点数转换器
     */
    class AnsiStringToFloatConverter : public IValueConverter<std::string, float>
    {
    public:
        virtual float Convert(const std::string &source) override
        {
            return std::stof(source);
        }
        virtual std::string ConvertBack(float target) override
        {
            return std::to_string(target);
        }
    };

    /**
     * @brief 窄字符串与双精度浮点数转换器
     */
    class AnsiStringToDoubleConverter : public IValueConverter<std::string, double>
    {
    public:
        virtual double Convert(const std::string &source) override
        {
            return std::stod(source);
        }
        virtual std::string ConvertBack(double target) override
        {
            return std::to_string(target);
        }
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
