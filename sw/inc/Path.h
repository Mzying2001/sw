#pragma once

#include <initializer_list>
#include <string>

namespace sw
{
    class Path
    {
    private:
        Path() = delete;

    public:
        /**
         * @brief      获取文件名
         * @param path 文件的路径
         * @return     文件名，包含扩展名
         */
        static std::wstring GetFileName(const std::wstring &path);

        /**
         * @brief      获取文件名
         * @param path 文件的路径
         * @return     文件名，不含扩展名
         */
        static std::wstring GetFileNameWithoutExt(const std::wstring &path);

        /**
         * @brief      获取扩展名
         * @param path 文件的路径
         * @return     文件的扩展名，不包含前面的点
         */
        static std::wstring GetExtension(const std::wstring &path);

        /**
         * @brief      获取文件所在路径
         * @param path 文件的路径
         * @return     文件所在路径
        */
        static std::wstring GetDirectory(const std::wstring &path);

        /**
         * @brief       对路径进行拼接
         * @param paths 要拼接的路径
         * @return      完整的路径
         */
        static std::wstring Combine(std::initializer_list<std::wstring> paths);
    };
}
