#include "Cursor.h"

HCURSOR sw::CursorHelper::GetCursorHandle(StandardCursor cursor)
{
    return LoadCursorW(NULL, MAKEINTRESOURCEW(cursor));
}

HCURSOR sw::CursorHelper::GetCursorHandle(HINSTANCE hInstance, const std::wstring &cursorName)
{
    return LoadCursorW(hInstance, cursorName.c_str());
}

HCURSOR sw::CursorHelper::GetCursorHandle(const std::wstring &fileName)
{
    return (HCURSOR)LoadImageW(NULL, fileName.c_str(), IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE);
}
