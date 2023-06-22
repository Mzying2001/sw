#include "App.h"

const sw::ReadOnlyProperty<HINSTANCE> sw::App::Instance(
    []() -> HINSTANCE {
        return GetModuleHandleW(NULL);
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
