#pragma warning(disable:4819)
#include "Window.h"
#include "FillLayout.h"
#include "Panel.h"
#include "WrapLayout.h"

using namespace sw;

int WINAPI wWinMain(
    _In_     HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_     PWSTR     pCmdLine,
    _In_     INT       nCmdShow)
{
    static Window window;
    static FillLayout layout;
    window.Layout = &layout;
    window.StartupLocation = WindowStartupLocation::CenterScreen;
    window.Show();

    static Panel panel;
    panel.Margin = 5;
    panel.VerticalAlignment = VerticalAlignment::Stretch;
    panel.HorizontalAlignment = HorizontalAlignment::Stretch;
    panel.BackColor = Color::LightGray;
    window.AddChild(panel);

    static WrapLayout wl;
    panel.Layout = &wl;

    static Panel panels[10];
    for (int i = 0; i < 10; ++i)
    {
        Panel& p = panels[i];
        p.Margin = 5;
        p.Rect = Rect(0, 0, 100, 100);
        p.BackColor = Color::Gray;
        panel.AddChild(p);
    }

    App::MsgLoop();
    return 0;
}
