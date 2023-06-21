#include "Window.h"

using namespace sw;

int WINAPI wWinMain(
    _In_     HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_     PWSTR     pCmdLine,
    _In_     INT       nCmdShow)
{
    Window window;
    window.Show();

    App::MsgLoop();
    return 0;
}
