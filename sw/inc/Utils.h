#pragma once

#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace sw
{
    /**
     * @brief 判断一个类型是否有ToString方法
     */
    template <typename T>
    struct _HasToString {
    private:
        template <typename U>
        static auto test(int) -> decltype(std::declval<U>().ToString(), std::true_type());

        template <typename U>
        static auto test(...) -> std::false_type;

    public:
        static constexpr bool value = decltype(test<T>(0))::value;
    };

    /**
     * @brief 工具类
     */
    class Utils
    {
    private:
        Utils() = delete; // 删除构造函数

    public:
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
         * @brief     删除首尾空白字符
         * @param str 输入的字符串
         * @return    删除首位空白字符后的字符串
         */
        static std::wstring Trim(const std::wstring &str);

        /**
         * @brief     删除串首空白字符
         * @param str 输入的字符串
         * @return    删除串首空白字符后的字符串
         */
        static std::wstring TrimStart(const std::wstring &str);

        /**
         * @brief     删除串尾空白字符
         * @param str 输入的字符串
         * @return    删除串尾空白字符后的字符串
         */
        static std::wstring TrimEnd(const std::wstring &str);

        /**
         * @brief           对字符串按照指定分隔符进行拆分
         * @param str       输入的字符串
         * @param delimiter 分隔符
         * @result          包含字串的vector
         */
        static std::vector<std::wstring> Split(const std::wstring &str, const std::wstring &delimiter);

        /**
         * @brief     格式化字符串，类似于 `swprintf`，但返回一个动态分配的 `std::wstring`
         * @param fmt 格式化字符串
         * @param ... 可变参数，符合 `fmt` 格式的输入
         * @return    返回一个包含格式化结果的字符串
         */
        static std::wstring FormatStr(const wchar_t *fmt, ...);

    public:
        /**
         * @brief 取两值中的较大值
         */
        template <typename T>
        static constexpr inline T Max(const T &a, const T &b)
        {
            return a > b ? a : b;
        }

        /**
         * @brief 取两值中的较小值
         */
        template <typename T>
        static constexpr inline T Min(const T &a, const T &b)
        {
            return a < b ? a : b;
        }

        /**
         * @brief 拼接字符串，也可使用此函数将其他类型转为wstring
         */
        template <typename... Args>
        static inline std::wstring BuildStr(const Args &...args)
        {
            std::wstringstream wss;
            int _[]{(Utils::_BuildStr(wss, args), 0)...};
            return wss.str();
        }

    private:
        /**
         * @brief BuildStr函数内部实现
         */
        template <typename T>
        static inline typename std::enable_if<!_HasToString<T>::value>::type
        _BuildStr(std::wostream &wos, const T &arg)
        {
            wos << arg;
        }

        /**
         * @brief 让BuildStr函数支持自定义类型
         */
        template <typename T>
        static inline typename std::enable_if<_HasToString<T>::value>::type
        _BuildStr(std::wostream &wos, const T &arg)
        {
            Utils::_BuildStr(wos, arg.ToString());
        }

        /**
         * @brief 让BuildStr函数将bool类型转化为"true"或"false"而不是数字1或0
         */
        static inline void _BuildStr(std::wostream &wos, bool b)
        {
            wos << (b ? L"true" : L"false");
        }

        /**
         * @brief 让BuildStr函数支持窄字符串
         */
        static inline void _BuildStr(std::wostream &wos, const char *str)
        {
            wos << Utils::ToWideStr(str);
        }

        /**
         * @brief 让BuildStr函数支持窄字符串
         */
        static inline void _BuildStr(std::wostream &wos, const std::string &str)
        {
            wos << Utils::ToWideStr(str);
        }

        /**
         * @brief 让BuildStr函数支持std::vector
         */
        template <typename T>
        static inline void _BuildStr(std::wostream &wos, const std::vector<T> &vec)
        {
            auto beg = vec.begin();
            auto end = vec.end();
            wos << L"[";
            for (auto it = beg; it != end; ++it) {
                if (it != beg)
                    wos << L", ";
                Utils::_BuildStr(wos, *it);
            }
            wos << L"]";
        }

        /**
         * @brief 让BildStr函数支持std::map
         */
        template <typename TKey, typename TVal>
        static inline void _BuildStr(std::wostream &wos, const std::map<TKey, TVal> &map)
        {
            auto beg = map.begin();
            auto end = map.end();
            wos << L"{";
            for (auto it = beg; it != end; ++it) {
                if (it != beg)
                    wos << L", ";
                Utils::_BuildStr(wos, it->first);
                wos << L":";
                Utils::_BuildStr(wos, it->second);
            }
            wos << L"}";
        }
    };
}
