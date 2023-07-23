#include "App.h"

const sw::ReadOnlyProperty<HINSTANCE> sw::App::Instance(
    []() -> const HINSTANCE & {
        static HINSTANCE hInstance = NULL;
        if (hInstance == NULL)
            hInstance = GetModuleHandleW(NULL);
        return hInstance;
    } //
);

const sw::ReadOnlyProperty<std::wstring> sw::App::ExePath(
    []() -> const std::wstring & {
        static std::wstring exePath;
        if (exePath.empty())
            exePath = App::_GetExePath();
        return exePath;
    } //
);

int sw::App::MsgLoop()
{
    MSG msg{};
    while (GetMessageW(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return (int)msg.wParam;
}

void sw::App::Quit(int exitCode)
{
    PostQuitMessage(exitCode);
}

std::wstring sw::App::_GetExePath()
{
    DWORD bufferSize = MAX_PATH; // 初始缓冲区大小
    PWSTR szExePath  = nullptr;
    DWORD pathLength = 0;

    // 循环直到获取到完整路径或达到一个合理的最大缓冲区大小
    while (true) {

        // 动态分配足够大的缓冲区
        delete[] szExePath;
        szExePath = new WCHAR[bufferSize];

        // 获取当前执行的 EXE 程序的路径
        pathLength = GetModuleFileNameW(nullptr, szExePath, bufferSize);

        // 判断是否缓冲区太小
        if (pathLength == 0 || pathLength >= bufferSize) {
            // 获取失败或缓冲区太小，增加缓冲区大小
            bufferSize *= 2;
        } else {
            // 成功获取完整路径
            break;
        }
    }

    // 转换为 std::wstring
    std::wstring exePath(szExePath);

    // 释放动态分配的内存
    delete[] szExePath;

    return exePath;
}
