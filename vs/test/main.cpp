#pragma warning(disable:4819)
#include "Window.h"
#include "MsgBox.h"

using namespace sw;

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

    App::MsgLoop();
    return 0;
}
