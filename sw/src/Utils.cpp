#include "Utils.h"
#include <Window.h>

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
