#pragma warning(disable:4819)
#include "Window.h"
#include "MsgBox.h"

using namespace sw;

class Button : public WndBase
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
            this->Text = L"LEAVE";
        }
        else if (msg.uMsg == WM_ParentReceivedCommand)
        {
            /*if (HIWORD(msg.wParam) == BN_CLICKED)
                MsgBox::Show(this, L"按钮被按下");*/
            if (HIWORD(msg.wParam) == BN_DOUBLECLICKED)
                MsgBox::Show(this, L"按钮被双击");
        }
        return this->WndBase::WndProc(msg);
    }
};

int WINAPI wWinMain(
    _In_     HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_     PWSTR     pCmdLine,
    _In_     INT       nCmdShow)
{
    Window window;
    window.StartupLocation = Window::CenterScreen;
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

    /*Button btn;
    btn.Rect = Rect(10, 10, 100, 100);
    btn.Parent = &window;*/

    Window window2;
    window2.Rect = Rect(50, 50, 500, 500);
    window2.Show();
    window.AddChild(&window2);

    App::MsgLoop();
    return 0;
}
