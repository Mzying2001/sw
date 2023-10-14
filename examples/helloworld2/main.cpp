#include "SimpleWindow.h"

int APIENTRY wWinMain(
    _In_     HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_     PWSTR     pCmdLine,
    _In_     INT       nCmdShow)
{
    // 窗口对象
    static sw::Window window;

    // 修改窗口布局方式为拉伸填充，调用SetLayout或者设置Layout属性即可修改窗口的布局方式
    window.SetLayout<sw::FillLayout>();

    // 创建一个StackPanel，该面板可以将控件从上到下或者从左到右依次排列
    static sw::StackPanel stackPanel;

    // 将stackPanel的垂直和水平对齐方式设置为居中
    stackPanel.HorizontalAlignment = sw::HorizontalAlignment::Center;
    stackPanel.VerticalAlignment = sw::VerticalAlignment::Center;

    // 创建两个标签
    static sw::Label labelTitle, labelTips;
    labelTitle.FontSize = 30;                       // 设置标题标签字体大小为30
    labelTitle.Text = L"Hello, Simple Window!";     // 设置标题标签文字内容
    labelTips.Margin = sw::Thickness(0, 20, 0, 20); // 设置提示信息标签的上下边距为20

    // 创建一个按钮，并设置按钮文本
    static sw::Button button;
    button.Text = L"Click me!";

    // 注册按钮单击事件，当按钮被单击是调用回调函数，这里传入一个lambda表达式
    button.RegisterRoutedEvent(sw::ButtonBase_Clicked, [](sw::UIElement&, sw::RoutedEventArgs) {
        static int counter = 0;
        // 直接修改Text属性即可更新标签的文本，使用Utils::BuildStr函数可以方便地构建字符串
        labelTips.Text = sw::Utils::BuildStr(L"You have clicked the button ", ++counter, L" times.");
        });

    // 使用AddChild函数添加控件
    window.AddChild(stackPanel);
    stackPanel.AddChild(labelTitle);
    stackPanel.AddChild(labelTips);
    stackPanel.AddChild(button);

    // 显示窗口
    window.Show();

    // 开始消息循环
    return sw::App::MsgLoop();
}
