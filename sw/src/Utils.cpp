#include "Utils.h"
#include <Windows.h>

std::wstring sw::Utils::ToWideStr(const std::string &str, bool utf8)
{
    int code = utf8 ? CP_UTF8 : CP_ACP;
    int size = MultiByteToWideChar(code, 0, str.c_str(), -1, nullptr, 0);
    std::wstring wstr(size - 1, L'\0');
    MultiByteToWideChar(code, 0, str.c_str(), -1, &wstr[0], size);
    return wstr;
}

std::string sw::Utils::ToMultiByteStr(const std::wstring &wstr, bool utf8)
{
    int code = utf8 ? CP_UTF8 : CP_ACP;
    int size = WideCharToMultiByte(code, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string str(size - 1, '\0');
    WideCharToMultiByte(code, 0, wstr.c_str(), -1, &str[0], size, nullptr, nullptr);
    return str;
}

std::wstring sw::Utils::Trim(const std::wstring &str)
{
    size_t firstNonSpace = str.find_first_not_of(L" \t\n\r\f\v");
    if (firstNonSpace == std::wstring::npos) {
        return L"";
    }
    size_t lastNonSpace = str.find_last_not_of(L" \t\n\r\f\v");
    return str.substr(firstNonSpace, lastNonSpace - firstNonSpace + 1);
}

std::wstring sw::Utils::TrimStart(const std::wstring &str)
{
    size_t firstNonSpace = str.find_first_not_of(L" \t\n\r\f\v");
    if (firstNonSpace == std::wstring::npos) {
        return L"";
    }
    return str.substr(firstNonSpace);
}

std::wstring sw::Utils::TrimEnd(const std::wstring &str)
{
    size_t lastNonSpace = str.find_last_not_of(L" \t\n\r\f\v");
    if (lastNonSpace == std::wstring::npos) {
        return L"";
    }
    return str.substr(0, lastNonSpace + 1);
}

std::vector<std::wstring> sw::Utils::Split(const std::wstring &str, const std::wstring &delimiter)
{
    std::vector<std::wstring> result;

    if (delimiter.empty()) {
        result.push_back(str);
        return result;
    }

    size_t start = 0;
    size_t end   = str.find(delimiter);

    const size_t delimiterLength = delimiter.length();

    while (end != std::wstring::npos) {
        result.emplace_back(str, start, end - start);
        start = end + delimiterLength;
        end   = str.find(delimiter, start);
    }

    result.emplace_back(str, start);

    return result;
}
