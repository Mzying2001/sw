#pragma once

#include "SimpleWindow.h"

/**
 * @brief 自定义窗口类，继承自sw::Window
 */
class MyWindow : public sw::Window
{
    sw::FillLayout fillLayout; // 窗口布局方式对象
    sw::StackPanel stackPanel; // 堆叠面板，可将子控件依次排列
    sw::Label labelTitle;      // 标题标签
    sw::Label labelTips;       // 提示标签
    sw::Button button;         // 按钮

    int counter = 0;           // 记录按钮点击次数的成员变量



    /**
     * @brief 初始化窗口和控件
     */
    void InitializeComponent() {
        // 设置窗口布局
        this->Layout = &fillLayout;

        // 将stackPanel的垂直和水平对齐方式设置为居中
        stackPanel.HorizontalAlignment = sw::HorizontalAlignment::Center;
        stackPanel.VerticalAlignment = sw::VerticalAlignment::Center;

        labelTitle.FontSize = 30;                       // 设置标题标签字体大小为30
        labelTitle.Text = L"Hello, Simple Window!";     // 设置标题标签文字内容
        labelTips.Margin = sw::Thickness(0, 20, 0, 20); // 设置提示信息标签的上下边距为20

        // 设置按钮文本
        button.Text = L"Click me!";

        // 注册按钮单击事件，使用std::bind传递成员函数
        button.RegisterRoutedEvent(sw::ButtonBase_Clicked,
            std::bind(&MyWindow::ButtonClickedHandler, this, std::placeholders::_1, std::placeholders::_2));

        // 使用AddChild函数添加控件
        this->AddChild(stackPanel);
        stackPanel.AddChild(labelTitle);
        stackPanel.AddChild(labelTips);
        stackPanel.AddChild(button);
    }



    /**
     * @brief 按钮被单击时调用该函数
     */
    void ButtonClickedHandler(sw::UIElement& sender, sw::RoutedEventArgs& e) {
        // 默认路由事件会向上冒泡，将RoutedEventArgs的handled字段设为true可停止冒泡
        e.handled = true;
        // 直接修改Text属性即可更新标签的文本，使用Utils::BuildStr函数可以方便地构建字符串
        labelTips.Text = sw::Utils::BuildStr(L"You have clicked the button ", ++counter, L" times.");
    }



public:
    /**
     * @brief MyWindow构造函数
     */
    MyWindow() {
        this->InitializeComponent();
    }
};