#pragma warning(disable:4819)
#include "Window.h"
#include "FillLayout.h"
#include "StackPanel.h"
#include "WrapPanel.h"
#include "Button.h"

using namespace sw;

void GetSystemDefaultFont(LOGFONT& font) {
    NONCLIENTMETRICSW ncm;
    ncm.cbSize = sizeof(ncm);

    if (SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, ncm.cbSize, &ncm, 0)) {
        font = ncm.lfMessageFont;
    }
}

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

    LOGFONTW font;
    GetSystemDefaultFont(font);
    HFONT hFont = CreateFontIndirectW(&font);
    for (int i = 0; i < 10; ++i)
    {
        Button& b = btns[i];
        SendMessageW(b.Handle, WM_SETFONT, (WPARAM)hFont, TRUE);
    }
    //bool b = DeleteObject(hFont);

    App::MsgLoop();
    return 0;
}
