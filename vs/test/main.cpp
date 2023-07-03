#pragma warning(disable:4819)
#include "Window.h"
#include "FillLayout.h"
#include "StackPanel.h"
#include "WrapPanel.h"
#include "Button.h"

using namespace sw;

int WINAPI wWinMain(
    _In_     HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_     PWSTR     pCmdLine,
    _In_     INT       nCmdShow)
{
    static Window window;
    window.StartupLocation = WindowStartupLocation::CenterScreen;
    window.Show();

    static FillLayout layout;
    window.Layout = &layout;

    static StackPanel panel;
    panel.Margin = 5;
    window.AddChild(panel);

    static Button btns[10];
    for (int i = 0; i < 10; ++i)
    {
        Button& b = btns[i];
        b.Margin = 5;
        b.Rect = Rect(0, 0, 100, 100);
        panel.AddChild(b);
    }

    App::MsgLoop();
    return 0;
}
