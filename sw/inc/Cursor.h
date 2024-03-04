#pragma once

#include <Windows.h>
#include <string>

namespace sw
{
    /**
     * @brief 系统标准鼠标样式
     * @brief https://learn.microsoft.com/en-us/windows/win32/menurc/about-cursors
     */
    enum class StandardCursor {
        Arrow       = 32512, // Normal select
        IBeam       = 32513, // Text select
        Wait        = 32514, // Busy
        Cross       = 32515, // Precision select
        UpArrow     = 32516, // Alternate select
        Handwriting = 32631, // Handwriting
        SizeNWSE    = 32642, // Diagonal resize 1
        SizeNESW    = 32643, // Diagonal resize 2
        SizeWE      = 32644, // Horizontal resize
        SizeNS      = 32645, // Vertical resize
        SizeAll     = 32646, // Move
        No          = 32648, // Unavailable
        Hand        = 32649, // Link select
        AppStarting = 32650, // Working in background
        Help        = 32651, // Help select
        Pin         = 32671, // Location select
        Person      = 32672, // Person select
    };

    /**
     * @brief 用于获取鼠标句柄的工具类
     */
    class CursorHelper
    {
    private:
        CursorHelper() = delete;

    public:
        /**
         * @brief        获取系统标准鼠标样式句柄
         * @param cursor 鼠标样式
         * @return       鼠标句柄
         */
        static HCURSOR GetCursorHandle(StandardCursor cursor);

        /**
         * @brief            从指定模块中获取鼠标句柄
         * @param hInstance  DLL或EXE的模块句柄
         * @param resourceId 鼠标的资源序号
         * @return           成功则返回鼠标句柄，否则返回NULL
         */
        static HCURSOR GetCursorHandle(HINSTANCE hInstance, int resourceId);

        /**
         * @brief          从文件加载鼠标句柄
         * @param fileName 鼠标文件路径
         * @return         成功则返回鼠标句柄，否则返回NULL
         */
        static HCURSOR GetCursorHandle(const std::wstring &fileName);
    };
}
