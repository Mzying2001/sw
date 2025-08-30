#pragma once

#include "SimpleWindow.h"
#include <cmath>

/**
 * @brief 自定义窗口类，继承自sw::Window
 */
class MyWindow : public sw::Window
{
public:
    /**
     * @brief 定义一个委托，用于请求创建新窗口
     */
    sw::Action<> RequestNewWindow;

private:
    sw::Label lb;
    sw::Button btn;

    /**
     * @brief 初始化窗口和控件
     */
    void InitializeComponent()
    {
        Width = 500;
        Height = 350;
        SetLayout<sw::StackLayout>();

        lb.Text = L"Current Thread ID: " + std::to_wstring(GetThreadId());
        lb.HorizontalAlignment = sw::HorizontalAlignment::Center;

        btn.Text = L"New Window";
        btn.AutoSize = true;
        btn.AddHandler(sw::ButtonBase_Clicked, *this, &MyWindow::BtnClickedHandler);

        AddChildren(lb, btn);
    }

    /**
     * @brief 按钮点击事件处理函数
     */
    void BtnClickedHandler(sw::UIElement &sender, sw::RoutedEventArgs &e)
    {
        if (RequestNewWindow) {
            RequestNewWindow(); // 按钮点击时触发事件，通知外部创建新窗口
        }
    }

public:
    /**
     * @brief MyWindow构造函数
     */
    MyWindow()
    {
        InitializeComponent();
    }
};
