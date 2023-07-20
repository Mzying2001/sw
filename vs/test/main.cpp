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

    //static DockPanel panel;
    //panel.Margin = 5;
    //window.AddChild(panel);

    //static Button btns[10];
    //for (int i = 0; i < 10; ++i) {
    //    Button& b = btns[i];
    //    b.VerticalAlignment = VerticalAlignment::Stretch;
    //    b.HorizontalAlignment = HorizontalAlignment::Stretch;
    //}

    //panel.AddChild(btns[0], DockLayout::Left);
    //panel.AddChild(btns[1], DockLayout::Right);
    //panel.AddChild(btns[2], DockLayout::Top);
    //panel.AddChild(btns[3], DockLayout::Bottom);
    //panel.AddChild(btns[4] /* last child fill */);

    static Label label;
    window.AddChild(label);

    label.Rect = Rect(0, 0, 100, 100);
    label.Text = L"A very long text that will show complete words with ellipsis.";
    label.VerticalContentAlignment = VerticalAlignment::Center;
    label.HorizontalContentAlignment = HorizontalAlignment::Center;

    label.TextTrimming = TextTrimming::WordEllipsis;

    return App::MsgLoop();
}
