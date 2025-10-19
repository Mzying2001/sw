#include "App.h"
#include "Path.h"

namespace
{
    /**
     * @brief 程序退出消息循环的方式，默认为Auto
     */
    thread_local sw::AppQuitMode _appQuitMode = sw::AppQuitMode::Auto;

    /**
     * @brief  获取当前exe文件路径
     */
    std::wstring _GetExePath()
    {
        HMODULE hModule = GetModuleHandleW(NULL);

        std::wstring exePath;
        exePath.resize(MAX_PATH);

        while (true) {
            DWORD len =
                GetModuleFileNameW(hModule, &exePath[0], (DWORD)exePath.size());
            if (len == 0) {
                exePath.clear();
                break;
            } else if (len < exePath.size()) {
                exePath.resize(len);
                break;
            } else {
                exePath.resize(exePath.size() * 2);
            }
        }
        return exePath;
    }

    /**
     * @brief 获取当前工作路径
     */
    std::wstring _GetCurrentDirectory()
    {
        int len = (int)GetCurrentDirectoryW(0, NULL);
        if (len <= 0) {
            return std::wstring{};
        } else {
            std::wstring result;
            result.resize(len + 1);
            result.resize(GetCurrentDirectoryW((DWORD)result.size(), &result[0]));
            return result;
        }
    }
}

/**
 * @brief 当前线程消息循环中处理空句柄消息的回调函数
 */
thread_local sw::Action<MSG &> sw::App::NullHwndMsgHandler;

/**
 */

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
    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0) > 0) {
        if (msg.hwnd == NULL) {
            if (NullHwndMsgHandler)
                NullHwndMsgHandler(msg);
        } else {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }
    return (int)msg.wParam;
}

void sw::App::QuitMsgLoop(int exitCode)
{
    PostQuitMessage(exitCode);
}
