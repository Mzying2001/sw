#pragma once

#include <sstream>
#include <string>

namespace sw
{
    class Utils
    {
    private:
        Utils() = delete;

        template <class T>
        static void _BuildStr(std::wstringstream &wss, const T &arg);
        static void _BuildStr(std::wstringstream &wss, const char *str);
        static void _BuildStr(std::wstringstream &wss, const std::string &str);

        template <class First, class... Rest>
        static void _BuildStr(std::wstringstream &wss, const First &first, const Rest &...rest);

        template <class... Rest>
        static void _BuildStr(std::wstringstream &wss, const char *str, const Rest &...rest);

        template <class... Rest>
        static void _BuildStr(std::wstringstream &wss, const std::string &str, const Rest &...rest);

    public:
        /**
         * @brief 拼接字符串，也可使用此函数将其他类型转为wstring
         */
        template <class... Args>
        static std::wstring BuildStr(const Args &...args);

        /**
         * @brief      将窄字符串转为宽字符串
         * @param str  要转换的字符串
         * @param utf8 是否使用utf8编码
         * @return     转换后的字符串
         */
        static std::wstring ToWideStr(const std::string &str, bool utf8 = false);

        /**
         * @brief      将宽字符串转为窄字符串
         * @param wstr 要转换的字符串
         * @param utf8 是否使用utf8编码
         * @return     转换后的字符串
         */
        static std::string ToMultiByteStr(const std::wstring &wstr, bool utf8 = false);
    };

    /*================================================================================*/

    template <class T>
    inline void Utils::_BuildStr(std::wstringstream &wss, const T &arg)
    {
        wss << arg;
    }

    inline void sw::Utils::_BuildStr(std::wstringstream &wss, const char *str)
    {
        wss << Utils::ToWideStr(str);
    }

    inline void sw::Utils::_BuildStr(std::wstringstream &wss, const std::string &str)
    {
        wss << Utils::ToWideStr(str);
    }

    template <class First, class... Rest>
    inline void Utils::_BuildStr(std::wstringstream &wss, const First &first, const Rest &...rest)
    {
        wss << first;
        Utils::_BuildStr(wss, rest...);
    }

    template <class... Rest>
    inline void Utils::_BuildStr(std::wstringstream &wss, const char *str, const Rest &...rest)
    {
        wss << Utils::ToWideStr(str);
        Utils::_BuildStr(wss, rest...);
    }

    template <class... Rest>
    inline void Utils::_BuildStr(std::wstringstream &wss, const std::string &str, const Rest &...rest)
    {
        wss << Utils::ToWideStr(str);
        Utils::_BuildStr(wss, rest...);
    }

    template <class... Args>
    inline std::wstring Utils::BuildStr(const Args &...args)
    {
        std::wstringstream wss;
        Utils::_BuildStr(wss, args...);
        return wss.str();
    }
}
