#include "SimpleWindow.h"

int APIENTRY WinMain(
    _In_     HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_     PSTR      pCmdLine,
    _In_     INT       nCmdShow)
{
    auto window     = std::make_unique<sw::Window>();       // 窗口对象
    auto stackPanel = std::make_unique<sw::StackPanel>();   // 堆叠面板，该面板可以将控件从上到下或者从左到右依次排列
    auto labelTitle = std::make_unique<sw::Label>();        // 显示标题的标签
    auto labelTips  = std::make_unique<sw::Label>();        // 显示提示的标签
    auto button     = std::make_unique<sw::Button>();       // 按钮



    // 修改窗口布局方式为拉伸填充，调用SetLayout或者设置Layout属性即可修改窗口的布局方式
    window->SetLayout<sw::FillLayout>();

    // 使用SetAlignment函数将stackPanel的垂直和水平对齐方式设置为居中
    stackPanel->SetAlignment(sw::HorizontalAlignment::Center, sw::VerticalAlignment::Center);

    labelTitle->FontSize = 30;                          // 设置标题标签字体大小为30
    labelTitle->Text     = L"Hello, Simple Window!";    // 设置标题标签文字内容
    labelTips->Margin    = sw::Thickness(0, 20);        // 设置提示信息标签的上下边距为20
    button->Text         = L"Click me!";                // 设置按钮文本

    // 使用AddChild函数添加控件
    window->AddChild(*stackPanel);
    stackPanel->AddChild(*labelTitle);
    stackPanel->AddChild(*labelTips);
    stackPanel->AddChild(*button);



    // 处理按钮单击事件，当按钮被单击时调用回调函数，这里传入一个lambda表达式
    button->AddHandler(sw::ButtonBase_Clicked, [&](sw::UIElement&, sw::RoutedEventArgs) {
        static int counter = 0;
        // 直接修改Text属性即可更新标签的文本，使用Utils::BuildStr函数可以方便地构建字符串
        labelTips->Text = sw::Utils::BuildStr(L"You have clicked the button ", ++counter, L" times.");
    });



    // 显示窗口
    window->Show();

    // 开始消息循环
    return sw::App::MsgLoop();
}
