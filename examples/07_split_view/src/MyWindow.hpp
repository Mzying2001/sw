#pragma once

#include "SimpleWindow.h"
#include <cmath>

/**
 * @brief 自定义窗口类，继承自sw::Window
 */
class MyWindow : public sw::Window
{
    // 组合框，该元素悬浮于界面左边
    sw::GroupBox groupBox;

    // 分隔条，用户可拖动该分隔条调整组合框宽度
    sw::DockSplitter splitter;

    // 堆叠面板，用于摆放按钮
    sw::StackPanel stackPanel;

    // 按钮个数
    static constexpr int BTN_COUNT = 50;

    // 按钮数组
    sw::Button btnarr[BTN_COUNT];

    /**
     * @brief 初始化窗口和控件
     */
    void InitializeComponent()
    {
        // 设置窗口布局方式
        SetLayout<sw::DockLayout>();

        // 将组合框设置为悬浮并添加到窗口
        // 将Float属性设置为true即可将元素设为悬浮，悬浮的元素位置不会受到滚动条影响
        groupBox.Float = true;
        groupBox.Margin = {10, 10, 0, 10};
        groupBox.MinWidth = 100;
        groupBox.MaxWidth = 500;
        groupBox.VerticalAlignment = sw::VerticalAlignment::Stretch;
        AddChild(groupBox, sw::DockLayoutTag::Left);

        // 将分隔条添加到窗口
        splitter.Float = true;
        splitter.DrawSplitterLine = false;
        splitter.Orientation = sw::Orientation::Vertical;
        AddChild(splitter, sw::DockLayoutTag::Left);

        // 添加按钮到界面
        for (int i = 0; i < BTN_COUNT; ++i)
        {
            auto &btn = btnarr[i];
            btn.Text = L"Button" + std::to_wstring(i + 1);
            btn.Margin = {0, 10, 0, 0};
            btn.HorizontalAlignment = sw::HorizontalAlignment::Stretch;
            stackPanel.AddChild(btn);
        }

        // 将堆叠面板的垂直对齐方式设为Top，使之再垂直方向尺寸不会被限制
        stackPanel.Margin = {0, 0, 10, 10};
        stackPanel.VerticalAlignment = sw::VerticalAlignment::Top;
        AddChild(stackPanel);

        // 显示滚动条，当用户区大小不足以容纳所有按钮时滚动条会自动启用
        // 注：仅在设置了布局方式时可自动调整滚动条参数可控件位置，若未设置则需要自行调整
        VerticalScrollBar = true;
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
