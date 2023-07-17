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

    DockLayout layout;
    window.Layout = &layout;

    static Button btns[10];
    for (int i = 0; i < 10; ++i) {
        Button& b = btns[i];
        b.VerticalAlignment = VerticalAlignment::Stretch;
        b.HorizontalAlignment = HorizontalAlignment::Stretch;
    }

    btns[0].LayoutTag = DockLayout::Left;
    window.AddChild(btns[0]);

    btns[1].LayoutTag = DockLayout::Right;
    window.AddChild(btns[1]);

    btns[2].LayoutTag = DockLayout::Top;
    window.AddChild(btns[2]);

    btns[3].LayoutTag = DockLayout::Bottom;
    window.AddChild(btns[3]);

    btns[4].LayoutTag = DockLayout::Bottom;
    window.AddChild(btns[4]);

    return App::MsgLoop();
}
