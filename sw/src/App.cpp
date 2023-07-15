#include "App.h"

const sw::ReadOnlyProperty<HINSTANCE> sw::App::Instance(
    []() -> const HINSTANCE & {
        static HINSTANCE _instance;
        _instance = GetModuleHandleW(NULL);
        return _instance;
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
