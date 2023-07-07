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
        b.Description = std::to_wstring(i + 1);
        panel.AddChild(b);
    }

    panel.RegisterRoutedEvent(ButtonBase_Clicked, [&](UIElement& element, RoutedEventArgs& args) {
        args.handled = true;
        MsgBox::Show(&window, L"你点击了第" + element.Description + L"个按钮");
    });

    App::MsgLoop();
    return 0;
}
