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
        template <class First, class... Rest>
        static void _BuildStr(std::wstringstream &wss, const First &first, const Rest &...rest);

    public:
        /**
         * @brief 拼接字符串，也可使用此函数将其他类型转为wstring
         */
        template <class... Args>
        static std::wstring BuildStr(const Args &...args);
    };

    /*================================================================================*/

    template <class T>
    inline void Utils::_BuildStr(std::wstringstream &wss, const T &arg)
    {
        wss << arg;
    }

    template <class First, class... Rest>
    inline void Utils::_BuildStr(std::wstringstream &wss, const First &first, const Rest &...rest)
    {
        wss << first;
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
