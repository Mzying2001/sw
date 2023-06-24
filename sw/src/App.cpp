#include "App.h"

const sw::ReadOnlyProperty<HINSTANCE> sw::App::Instance(
    []() -> const HINSTANCE & {
        static HINSTANCE _instance;
        _instance = GetModuleHandleW(NULL);
        return _instance;
    } //
);

void sw::App::MsgLoop()
{
    MSG msg{};
    while (GetMessageW(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
}

void sw::App::Quit(int exitCode)
{
    PostQuitMessage(exitCode);
}
