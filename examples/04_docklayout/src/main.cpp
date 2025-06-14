#include "MyWindow.hpp"

int APIENTRY WinMain(
    _In_     HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_     PSTR      pCmdLine,
    _In_     INT       nCmdShow)
{
    // 创建自定义的窗口对象
    static MyWindow myWindow;

    // 显示窗口
    myWindow.Show();

    // 开始消息循环
    return sw::App::MsgLoop();
}
