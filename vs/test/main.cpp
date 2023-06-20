#include "Window.h"

using namespace sw;

class MyWindow : public Window
{
public:
    MyWindow()
    {
    }
};

int WINAPI wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ PWSTR pCmdLine,
    _In_ INT nCmdShow)
{
    MyWindow window;
    window.Show();

    MSG msg{};
    while (GetMessageW(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
}
