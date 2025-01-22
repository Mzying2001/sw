#include "SimpleWindow.h"

int APIENTRY wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ PWSTR pCmdLine,
    _In_ INT nCmdShow)
{
    // 创建一个窗口对象
    sw::Window w;

    // 设置窗口标题
    w.Text = L"Hello, World!";

    // 显示窗口
    w.Show();

    // 开始消息循环
    return sw::App::MsgLoop();
}
