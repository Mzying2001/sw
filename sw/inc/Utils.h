#pragma once

#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace sw
{
    /**
     * @brief 工具类
     */
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

        /**
         * @brief 使BuildStr支持std::vector
         */
        template <class T>
        friend std::wstringstream &operator<<(std::wstringstream &wss, const std::vector<T> &vec);

        /**
         * @brief 使BuildStr支持std::map
         */
        template <class TKey, class TVal>
        friend std::wstringstream &operator<<(std::wstringstream &wss, const std::map<TKey, TVal> &map);

        /**
         * @brief 取两值中的较大值
         */
        template <class T>
        static constexpr inline T Max(const T &a, const T &b)
        {
            return a > b ? a : b;
        }

        /**
         * @brief 取两值中的较小值
         */
        template <class T>
        static constexpr inline T Min(const T &a, const T &b)
        {
            return a < b ? a : b;
        }
    };

    /**
     * @brief 使BuildStr支持窄字符串
     */
    std::wostream &operator<<(std::wostream &wos, const char *str);

    /**
     * @brief 使BuildStr支持窄字符串
     */
    std::wostream &operator<<(std::wostream &wos, const std::string &str);

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

    template <class T>
    inline std::wstringstream &operator<<(std::wstringstream &wss, const std::vector<T> &vec)
    {
        auto beg = vec.begin();
        auto end = vec.end();
        wss << L"[";
        for (auto it = beg; it != end; ++it) {
            if (it != beg)
                wss << L", ";
            Utils::_BuildStr(wss, *it);
        }
        wss << L"]";
        return wss;
    }

    template <class TKey, class TVal>
    inline std::wstringstream &operator<<(std::wstringstream &wss, const std::map<TKey, TVal> &map)
    {
        auto beg = map.begin();
        auto end = map.end();
        wss << L"{";
        for (auto it = beg; it != end; ++it) {
            if (it != beg)
                wss << L", ";
            Utils::_BuildStr(wss, it->first);
            wss << L":";
            Utils::_BuildStr(wss, it->second);
        }
        wss << L"}";
        return wss;
    }
}
