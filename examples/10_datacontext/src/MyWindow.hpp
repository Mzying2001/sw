#pragma once

#include "SimpleWindow.h"
#include "ViewModel.hpp"

/**
 * @brief 自定义窗口类，继承自sw::Window
 */
class MyWindow : public sw::Window
{
private:
    sw::Label label;
    sw::Slider slider;
    sw::SpinBox spinBox;

    MyViewModel viewModel; // 数据模型实例

    /**
     * @brief 初始化窗口和控件
     */
    void InitializeComponent()
    {
        Width = 500;
        Height = 350;
        SetLayout<sw::StackLayout>();

        label.Margin = 10;
        slider.Margin = {10, 0, 10, 10};
        spinBox.Margin = {10, 0, 10, 10};
        AddChildren(slider, spinBox, label);

        /**
         * DataContext存在继承机制，子元素会自动继承父元素的DataContext
         * 可通过CurrentDataContext属性获取当前生效的DataContext
         * 当DataContext改变时，所有绑定到该DataContext的绑定都会自动更新
         */

        // 设置当前窗口的DataContext为viewModel实例
        DataContext = &viewModel;

        // 绑定slider的Value属性到viewModel的Value属性，双向绑定
        // 添加绑定时若没有指定源对象，则自动绑定到CurrentDataContext
        slider.AddBinding(sw::Binding::Create(
            &sw::Slider::Value, &MyViewModel::Value, sw::BindingMode::TwoWay));

        // 绑定spinBox的Value属性到viewModel的Value属性，双向绑定
        spinBox.AddBinding(sw::Binding::Create(
            &sw::SpinBox::Value, &MyViewModel::Value, sw::BindingMode::TwoWay));

        // 绑定文本到DisplayText
        label.AddBinding(sw::Binding::Create(
            &sw::Label::Text, &MyViewModel::DisplayText, sw::BindingMode::OneWay));
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
