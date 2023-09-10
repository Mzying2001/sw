#pragma once

#include "SimpleWindow.h"

/**
 * @brief 自定义窗口类，继承自sw::Window
 */
class MyWindow : public sw::Window
{
    // 窗口布局对象
    sw::StackLayout stackLayout;

    // 按钮个数
    static constexpr int BTN_COUNT = 10;

    // 按钮数组
    sw::Button btnarr[BTN_COUNT];



    /**
     * @brief 初始化窗口和控件
     */
    void InitializeComponent()
    {
        // 设置窗口布局
        this->Layout = &stackLayout;

        // 添加按钮到窗口
        for (int i = 0; i < BTN_COUNT; ++i) {
            sw::Button& btn = btnarr[i];
            btn.Text = sw::Utils::BuildStr(L"Button", i + 1);
            btn.Margin = sw::Thickness(10, 10, 10, 0);
            btn.HorizontalAlignment = sw::HorizontalAlignment::Stretch;
            this->AddChild(btn);
        }

        // 显示滚动条，当用户区大小不足以容纳所有按钮时滚动条会自动启用
        // 注：对于AbsoluteLayout（即未指定布局方式）需要自行调整滚动条参数和控件位置，其他布局方式可自动完成
        this->VerticalScrollBar = true;

        // 注册鼠标滚轮事件实现页面随鼠标滚动而滚动
        this->RegisterRoutedEvent<sw::MouseWheelEventArgs>(*this, &MyWindow::MouseWheelHandler);
    }



    /**
     * @brief 鼠标滚轮事件处理函数
     */
    void MouseWheelHandler(sw::UIElement& sender, sw::MouseWheelEventArgs& e)
    {
        if (e.wheelDelta != 0) {
            int direction = e.wheelDelta / std::abs(e.wheelDelta);
            this->ScrollVertical(-direction * 20);
        }
    }



public:
    /**
     * @brief MyWindow构造函数
     */
    MyWindow()
    {
        this->InitializeComponent();
    }
};
