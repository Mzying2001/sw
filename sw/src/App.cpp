#include "App.h"
#include "Path.h"

/**
 * @brief 储存App::QuitMode的变量，默认为Auto
 */
static sw::AppQuitMode _appQuitMode = sw::AppQuitMode::Auto;

/**
 * @brief  获取当前exe文件路径
 */
static std::wstring _GetExePath();

/**
 * @brief 获取当前工作路径
 */
static std::wstring _GetCurrentDirectory();

/*================================================================================*/

const sw::ReadOnlyProperty<HINSTANCE> sw::App::Instance(
    []() -> HINSTANCE {
        static HINSTANCE hInstance = GetModuleHandleW(NULL);
        return hInstance;
    } //
);

const sw::ReadOnlyProperty<std::wstring> sw::App::ExePath(
    []() -> std::wstring {
        static std::wstring exePath = _GetExePath();
        return exePath;
    } //
);

const sw::ReadOnlyProperty<std::wstring> sw::App::ExeDirectory(
    []() -> std::wstring {
        static std::wstring exeDirectory = Path::GetDirectory(App::ExePath);
        return exeDirectory;
    } //
);

const sw::Property<std::wstring> sw::App::CurrentDirectory(
    // get
    []() -> std::wstring {
        return _GetCurrentDirectory();
    },
    // set
    [](const std::wstring &value) {
        SetCurrentDirectoryW(value.c_str());
    } //
);

const sw::Property<sw::AppQuitMode> sw::App::QuitMode(
    // get
    []() -> sw::AppQuitMode {
        return _appQuitMode;
    },
    // set
    [](const sw::AppQuitMode &value) {
        _appQuitMode = value;
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

void sw::App::QuitMsgLoop(int exitCode)
{
    PostQuitMessage(exitCode);
}

/*================================================================================*/

std::wstring _GetExePath()
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

std::wstring _GetCurrentDirectory()
{
    // 先获取路径的长度
    DWORD pathLength = GetCurrentDirectoryW(0, nullptr);
    if (pathLength == 0) {
        // 获取路径失败，返回空字符串
        return L"";
    }

    // 动态分配足够大的缓冲区
    std::unique_ptr<wchar_t[]> szStartUpPath = std::make_unique<wchar_t[]>(pathLength);

    // 获取当前工作目录（即程序启动的路径）
    DWORD actualLength = GetCurrentDirectoryW(pathLength, szStartUpPath.get());
    if (actualLength == 0 || actualLength > pathLength) {
        // 获取路径失败，返回空字符串
        return L"";
    }

    // 转换为 std::wstring
    std::wstring startUpPath(szStartUpPath.get());

    return startUpPath;
}
