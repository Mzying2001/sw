#include "Path.h"
#include <windows.h>

std::wstring sw::Path::GetFileName(const std::wstring &path)
{
    // Find the last occurrence of either '/' or '\'
    size_t lastSlashPos = path.find_last_of(L"/\\");

    // If no slash found or the last character is a slash (folder path)
    if (lastSlashPos == std::wstring::npos || lastSlashPos == path.length() - 1) {
        return L"";
    }

    // Extract the file name from the path and return it
    return path.substr(lastSlashPos + 1);
}

std::wstring sw::Path::GetFileNameWithoutExt(const std::wstring &path)
{
    // Find the last occurrence of either '/' or '\'
    size_t lastSlashPos = path.find_last_of(L"/\\");

    // If no slash found or the last character is a slash (folder path)
    if (lastSlashPos == std::wstring::npos || lastSlashPos == path.length() - 1) {
        return L"";
    }

    // Find the last occurrence of the dot (.) after the last slash
    size_t lastDotPos = path.find_last_of(L'.', lastSlashPos);

    // If no dot found or the dot is at the end of the string (file has no extension)
    if (lastDotPos == std::wstring::npos || lastDotPos == path.length() - 1) {
        // Extract the file name from the path and return it
        return path.substr(lastSlashPos + 1);
    }

    // Extract the file name without extension from the path and return it
    return path.substr(lastSlashPos + 1, lastDotPos - lastSlashPos - 1);
}

std::wstring sw::Path::GetExtension(const std::wstring &path)
{
    // Find the last occurrence of either '/' or '\'
    size_t lastSlashPos = path.find_last_of(L"/\\");

    // Find the last occurrence of the dot (.) after the last slash
    size_t lastDotPos = path.find_last_of(L'.');

    // If no dot found or the dot is at the end of the string (file has no extension)
    if (lastDotPos == std::wstring::npos || lastDotPos == path.length() - 1) {
        return L"";
    }

    // If no slash found or the last dot is before the last slash (file name has a dot)
    if (lastSlashPos == std::wstring::npos || lastDotPos < lastSlashPos) {
        return L"";
    }

    // Extract the extension from the path and return it
    return path.substr(lastDotPos + 1);
}

std::wstring sw::Path::GetDirectory(const std::wstring &path)
{
    // Find the last occurrence of either '/' or '\'
    size_t lastSlashPos = path.find_last_of(L"/\\");

    // If no slash found or the last character is a slash (folder path)
    if (lastSlashPos == std::wstring::npos || lastSlashPos == path.length() - 1) {
        return path;
    }

    // Return the directory part of the path along with the last slash
    return path.substr(0, lastSlashPos + 1);
}

std::wstring sw::Path::Combine(std::initializer_list<std::wstring> paths)
{
    std::wstring combinedPath;

    for (const auto &path : paths) {
        if (path.empty()) {
            continue;
        }

        if (!combinedPath.empty() && combinedPath.back() != L'/' && combinedPath.back() != L'\\') {
            combinedPath.append(L"\\");
        }

        if (path.front() == L'/' || path.front() == L'\\') {
            combinedPath.append(path.substr(1)); // Skip the first separator
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
