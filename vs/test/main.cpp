#pragma warning(disable:4819)
#include "Window.h"
#include "MsgBox.h"
#include "WrapLayout.h"

using namespace sw;

class Button : public UIElement
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
            if (HIWORD(msg.wParam) == BN_CLICKED)
                MsgBox::Show(this, L"按钮被按下");
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

    window.RegisterRoutedEvent(RoutedEventType::WindowClosing,
        [](UIElement& element, RoutedEventArgs& args) {
            bool& cancel = *reinterpret_cast<bool*>(args.param);
            MsgBox::ShowQuestion(&element, L"是否关闭？").OnNo([&]() {
                cancel = true;
                element.Text = L"已取消关闭";
            });
            args.handled = true;
        }
    );

    WrapLayout wrapLayout;
    window.Layout = &wrapLayout;

    window.DisableLayout();

    static Button btns[10];
    for (int i = 0; i < 5; ++i) {
        Button& btn = btns[i];
        btn.Margin = 10;
        btn.Rect = Rect(10, 10, 100, 100);
        /*btn.Height = btn.Height + i * 10;*/
        window.AddChild(btn);
    }

    Window window2;
    window2.Rect = Rect(0, 0, 300, 300);
    window2.Margin = 10;
    window.AddChild(window2);
    window2.Show();

    for (int i = 5; i < 10; ++i) {
        Button& btn = btns[i];
        btn.Margin = 10;
        btn.Rect = Rect(10, 10, 100, 100);
        /*btn.Height = btn.Height + i * 10;*/
        window.AddChild(btn);
    }

    window.EnableLayout();

    btns[1].Visible = false;

    App::MsgLoop();
    return 0;
}
