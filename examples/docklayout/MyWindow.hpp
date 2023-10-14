#pragma once

#include "SimpleWindow.h"

/**
 * @brief 自定义窗口类，继承自sw::Window
 */
class MyWindow : public sw::Window
{
    // 按钮对象
    sw::Button btn1, btn2, btn3, btn4, btn5;



    /**
     * @brief 初始化窗口和控件
     */
    void InitializeComponent()
    {
        // 设置窗口布局方式为DockLayout
        this->SetLayout<sw::DockLayout>();

        // 修改按钮的对齐方式，使按钮填充其所在位置
        btn1.VerticalAlignment = sw::VerticalAlignment::Stretch;
        btn2.VerticalAlignment = sw::VerticalAlignment::Stretch;
        btn3.VerticalAlignment = sw::VerticalAlignment::Stretch;
        btn4.VerticalAlignment = sw::VerticalAlignment::Stretch;
        btn5.VerticalAlignment = sw::VerticalAlignment::Stretch;
        btn1.HorizontalAlignment = sw::HorizontalAlignment::Stretch;
        btn2.HorizontalAlignment = sw::HorizontalAlignment::Stretch;
        btn3.HorizontalAlignment = sw::HorizontalAlignment::Stretch;
        btn4.HorizontalAlignment = sw::HorizontalAlignment::Stretch;
        btn5.HorizontalAlignment = sw::HorizontalAlignment::Stretch;

        /**
        * 可以通过设置控件的LayoutTag属性来指定Dock
        * LayoutTag是用于记录元素在布局中附加信息的标记
        * 对于不同的布局方式LayoutTag可能有不同的含义
        */

        // 修改LayoutTag来指定Dock
        btn1.LayoutTag = sw::DockLayout::Top;
        this->AddChild(btn1);

        // 可通过AddChild函数传参的方式指定Dock，该函数内部先修改LayoutTag再添加
        this->AddChild(btn4, sw::DockLayout::Bottom);
        this->AddChild(btn3, sw::DockLayout::Left);
        this->AddChild(btn2, sw::DockLayout::Right);

        // DockLayout默认最后一个元素会填充剩余空间，此时指定的Dock会被忽略
        // 若要最后一个元素不填充，将dockLayout的lastChildFill设为false即可（对于DockPanel则是LastChildFill属性）
        this->AddChild(btn5);
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
