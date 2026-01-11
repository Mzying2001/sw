#pragma once

#include <windows.h>
#include <string>

namespace sw
{
    /**
     * @brief 系统标准图标样式
     * @brief https://learn.microsoft.com/en-us/windows/win32/menurc/about-icons
     */
    enum class StandardIcon {
        Application = 32512, // Default application icon
        Error       = 32513, // Error icon
        Question    = 32514, // Question mark icon
        Warning     = 32515, // Warning icon
        Information = 32516, // Information icon
        WinLogo     = 32517, // Windows logo icon
        Shield      = 32518, // Security shield icon
    };

    /**
     * @brief 用于获取图标句柄的工具类
     */
    class IconHelper
    {
    private:
        IconHelper() = delete;

    public:
        /**
         * @brief      获取系统标准图标句柄
         * @param icon 图标样式
         * @return     图标句柄
         */
        static HICON GetIconHandle(StandardIcon icon);

        /**
         * @brief            从指定模块中获取图标句柄
         * @param hInstance  DLL或EXE的模块句柄
         * @param resourceId 图标的资源序号
         * @return           成功则返回图标句柄，否则返回NULL
         */
        static HICON GetIconHandle(HINSTANCE hInstance, int resourceId);

        /**
         * @brief          从文件加载图标句柄
         * @param fileName 图标文件的路径
         * @return         成功则返回图标句柄，否则返回NULL
         */
        static HICON GetIconHandle(const std::wstring &fileName);
    };
}
