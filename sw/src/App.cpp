#include "App.h"
#include "Path.h"

namespace
{
    /**
     * @brief 程序退出消息循环的方式，默认为Auto
     */
    sw::AppQuitMode _appQuitMode = sw::AppQuitMode::Auto;

    /**
     * @brief 消息循环中处理空句柄消息的回调函数
     */
    void (*_nullHwndMsgHandler)(const MSG &) = nullptr;

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
        std::wstring curdir;
        curdir.resize(GetCurrentDirectoryW(0, NULL));
        if (curdir.size())
            curdir.resize(GetCurrentDirectoryW((DWORD)curdir.size(), &curdir[0]));
        return curdir;
    }
}

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

const sw::Property<void (*)(const MSG &)> sw::App::NullHwndMsgHandler(
    // get
    []() -> void (*)(const MSG &) {
        return _nullHwndMsgHandler;
    },
    // set
    [](void (*const &value)(const MSG &)) {
        _nullHwndMsgHandler = value;
    } //
);

int sw::App::MsgLoop()
{
    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0) > 0) {
        if (msg.hwnd == NULL) {
            if (_nullHwndMsgHandler)
                _nullHwndMsgHandler(msg);
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
