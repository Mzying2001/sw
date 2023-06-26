#pragma warning(disable:4819)
#include "Window.h"
#include "MsgBox.h"

using namespace sw;

class Button : public WndBase
{
public:
    Button()
    {
        InitControl(L"BUTTON", L"按钮", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON);
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
        return this->DefaultWndProc(msg);
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

    Button btn;
    btn.Rect = Rect(10, 10, 100, 100);
    btn.Parent = &window;

    auto hbtn2 = CreateWindowExW(0, L"BUTTON", L"按钮", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        10, 120, 100, 100, window.Handle, NULL, hInstance, NULL);

    App::MsgLoop();
    return 0;
}
