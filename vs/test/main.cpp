#pragma warning(disable:4819)
#include "Window.h"
#include "Control.h"
#include "MsgBox.h"
#include "WrapLayout.h"
#include "FillLayout.h"

using namespace sw;

WrapLayout wrapLayout;

class Button : public Control
{
public:
    Button()
    {
        InitControl(L"BUTTON", L"按钮", WS_CHILD | WS_VISIBLE | BS_NOTIFY | BS_PUSHBUTTON);
    }

    virtual LRESULT WndProc(const ProcMsg& msg)
    {
        if (msg.uMsg == WM_MOUSEMOVE)
        {
            this->Text = L"MOVE";
        }
        else if (msg.uMsg == WM_MOUSELEAVE)
        {
            this->Text = L"按钮";
        }
        else if (msg.uMsg == WM_ParentReceivedCommand)
        {
            if (HIWORD(msg.wParam) == BN_CLICKED) {
                wrapLayout.orientation = Orientation(1 - (int)wrapLayout.orientation);
                this->NotifyLayoutUpdated();
            }
            /*if (HIWORD(msg.wParam) == BN_DOUBLECLICKED)
                MsgBox::Show(this, L"按钮被双击");*/
        }
        return this->UIElement::WndProc(msg);
    }
};

int WINAPI wWinMain(
    _In_     HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_     PWSTR     pCmdLine,
    _In_     INT       nCmdShow)
{
    Window window;
    window.StartupLocation = WindowStartupLocation::CenterScreen;
    window.Show();

    /*window.RegisterRoutedEvent(RoutedEventType::WindowClosing,
        [](UIElement& element, RoutedEventArgs& args) {
            bool& cancel = *reinterpret_cast<bool*>(args.param);
            MsgBox::ShowQuestion(&element, L"是否关闭？").OnNo([&]() {
                cancel = true;
                element.Text = L"已取消关闭";
            });
            args.handled = true;
        }
    );*/

    window.Layout = &wrapLayout;

    /*FillLayout fillLayout;
    window.Layout = &fillLayout;*/

    window.DisableLayout();

    static Button btns[9];
    for (int i = 0; i < 9; ++i) {
        Button& btn = btns[i];
        btn.Margin = 10;
        btn.Rect = Rect(10, 10, 100, 100);
        window.AddChild(btn);
    }

    Window window2;
    window2.Rect = Rect(0, 0, 300, 300);
    window2.Margin = 10;
    window.AddChild(window2);
    window2.Show();

    btns[0].VerticalAlignment = VerticalAlignment::Top;
    btns[1].VerticalAlignment = VerticalAlignment::Bottom;
    btns[2].HorizontalAlignment = HorizontalAlignment::Left;
    btns[3].HorizontalAlignment = HorizontalAlignment::Right;

    btns[4].VerticalAlignment = VerticalAlignment::Top;
    btns[4].HorizontalAlignment = HorizontalAlignment::Left;

    btns[5].VerticalAlignment = VerticalAlignment::Top;
    btns[5].HorizontalAlignment = HorizontalAlignment::Right;

    btns[6].VerticalAlignment = VerticalAlignment::Bottom;
    btns[6].HorizontalAlignment = HorizontalAlignment::Left;

    btns[7].VerticalAlignment = VerticalAlignment::Bottom;
    btns[7].HorizontalAlignment = HorizontalAlignment::Right;

    /*btns[8].HorizontalAlignment = HorizontalAlignment::Stretch;
    btns[9].VerticalAlignment = VerticalAlignment::Stretch;*/

    window.EnableLayout();

    App::MsgLoop();
    return 0;
}
