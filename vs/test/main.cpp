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
        args.handled = true;
        int index = panel.IndexOf(element);
        MsgBox::Show(&window, Utils::BuildStr(L"你点击了第", index + 1, L"个按钮"));
    });

    window.RegisterRoutedEvent(UIElement_PositionChanged, [&](UIElement& element, RoutedEventArgs& args) {
        if (&element == &window) {
            Point pos(element.Left, element.Top);
            window.Text = Utils::BuildStr(pos);
        }
    });

    window.RegisterRoutedEvent(UIElement_TextChanged, [&](UIElement& element, RoutedEventArgs& args) {
        if (&element == &window) {
            btns[0].Text = window.Text;
        }
    });

    window.RegisterRoutedEvent(UIElement_SizeChanged, [&](UIElement& element, RoutedEventArgs& args) {
        if (&element == &window) {
            btns[1].Text = Utils::BuildStr(L"w=", window.Width);
            btns[2].Text = Utils::BuildStr(L"h=", window.Height);
        }
    });

    App::MsgLoop();
    return 0;
}
