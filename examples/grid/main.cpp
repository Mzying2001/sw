#include "MyWindow.hpp"

int APIENTRY wWinMain(
    _In_     HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_     PWSTR     pCmdLine,
    _In_     INT       nCmdShow)
{
    // 创建自定义的窗口对象
    auto myWindow = std::make_unique<MyWindow>();

    // 显示窗口
    myWindow->Show();

    // 开始消息循环
    return sw::App::MsgLoop();
}
