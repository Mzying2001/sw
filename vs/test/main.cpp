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

    static StackPanel panel;
    panel.VerticalAlignment = VerticalAlignment::Center;
    panel.HorizontalAlignment = HorizontalAlignment::Center;
    window.AddChild(panel);

    static Label label;
    label.Text = L"Hello, Simple Window!";
    label.FontSize = 30;
    panel.AddChild(label);

    static Label label2;
    label2.Margin = Thickness(0, 20, 0, 20);
    panel.AddChild(label2);

    static Button button;
    button.Text = L"Click me!";
    panel.AddChild(button);

    button.RegisterRoutedEvent(ButtonBase_Clicked, [](UIElement&, RoutedEventArgs&) {
        static int count = 0;
        label2.Text = Utils::BuildStr(L"You have clicked button ", ++count, L" times.");
    });

    return App::MsgLoop();
}
