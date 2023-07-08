#pragma warning(disable:4819)
#include <Windows.h>
#include "SimpleWindow.h"

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

    static WrapPanel panel;
    panel.Margin = 5;
    window.AddChild(panel);

    static Button btns[10];
    for (int i = 0; i < 10; ++i)
    {
        Button& b = btns[i];
        b.Margin = 5;
        panel.AddChild(b);
    }

    panel.RegisterRoutedEvent(ButtonBase_Clicked, [&](UIElement& element, RoutedEventArgs& args) {

        int index = -1;
        for (int i = 0; i < panel.ChildCount; ++i)
        {
            if (&panel[i] == &element)
            {
                index = i;
                break;
            }
        }

        args.handled = true;
        MsgBox::Show(&window, Utils::BuildStr(L"你点击了第", index + 1, L"个按钮"));
    });

    App::MsgLoop();
    return 0;
}
