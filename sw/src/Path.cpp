#include "Path.h"
#include <windows.h>

std::wstring sw::Path::GetFileName(const std::wstring &path)
{
    size_t lastSlashPos = path.find_last_of(L"/\\");
    size_t start        = (lastSlashPos == std::wstring::npos) ? 0 : lastSlashPos + 1;
    return path.substr(start);
}

std::wstring sw::Path::GetFileNameWithoutExt(const std::wstring &path)
{
    size_t lastSlashPos = path.find_last_of(L"/\\");
    size_t start        = (lastSlashPos == std::wstring::npos) ? 0 : lastSlashPos + 1;

    size_t lastDotPos = path.find_last_of(L'.');

    // 点号不在文件名段内、或是文件名首字符（如 ".bashrc"）、或是末尾的裸点，均视为无扩展名
    if (lastDotPos == std::wstring::npos || lastDotPos <= start || lastDotPos == path.length() - 1) {
        return path.substr(start);
    }
    return path.substr(start, lastDotPos - start);
}

std::wstring sw::Path::GetExtension(const std::wstring &path)
{
    size_t lastSlashPos = path.find_last_of(L"/\\");
    size_t lastDotPos   = path.find_last_of(L'.');

    if (lastDotPos == std::wstring::npos || lastDotPos == path.length() - 1) {
        return L"";
    }
    // 点号若落在目录段，不视为扩展名
    if (lastSlashPos != std::wstring::npos && lastDotPos <= lastSlashPos) {
        return L"";
    }
    // 点号若是文件名首字符（".bashrc"），不视为扩展名
    size_t start = (lastSlashPos == std::wstring::npos) ? 0 : lastSlashPos + 1;
    if (lastDotPos == start) {
        return L"";
    }
    return path.substr(lastDotPos + 1);
}

std::wstring sw::Path::GetDirectory(const std::wstring &path)
{
    size_t lastSlashPos = path.find_last_of(L"/\\");
    if (lastSlashPos == std::wstring::npos) {
        return L"";
    }
    return path.substr(0, lastSlashPos + 1);
}

std::wstring sw::Path::Combine(std::initializer_list<std::wstring> paths)
{
    std::wstring combinedPath;

    for (const auto &path : paths) {
        if (path.empty()) {
            continue;
        }

        bool startsWithSep = path.front() == L'/' || path.front() == L'\\';

        if (combinedPath.empty()) {
            // 第一段整段保留，包括可能的前导分隔符（绝对路径、UNC）
            combinedPath.append(path);
            continue;
        }

        bool endsWithSep = combinedPath.back() == L'/' || combinedPath.back() == L'\\';

        if (!endsWithSep && !startsWithSep) {
            combinedPath.append(L"\\");
            combinedPath.append(path);
        } else if (endsWithSep && startsWithSep) {
            combinedPath.append(path.substr(1)); // 避免双分隔符
        } else {
            combinedPath.append(path);
        }
    }

    return combinedPath;
}

std::wstring sw::Path::GetAbsolutePath(const std::wstring &path)
{
    // 获取文件路径的最大长度
    DWORD bufferSize = GetFullPathNameW(path.c_str(), 0, nullptr, nullptr);

    if (bufferSize == 0) {
        // GetFullPathNameW 返回0表示失败
        return L"";
    }

    // 为获取绝对路径分配内存
    std::wstring absolutePath;
    absolutePath.resize(bufferSize);

    // 获取绝对路径
    DWORD result = GetFullPathNameW(path.c_str(), bufferSize, &absolutePath[0], nullptr);
    if (result == 0 || result >= bufferSize) {
        // 获取绝对路径失败
        return L"";
    }

    // 移除不必要的空白字符
    absolutePath.resize(result);

    return absolutePath;
}
