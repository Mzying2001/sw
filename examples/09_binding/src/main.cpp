#include "MyWindow.hpp"

int APIENTRY WinMain(
    _In_     HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_     PSTR      pCmdLine,
    _In_     INT       nCmdShow)
{
    // 创建自定义的窗口对象并显示
    auto wnd = std::make_unique<MyWindow>();
    return wnd->ShowDialog();
}
