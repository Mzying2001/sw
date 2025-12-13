#pragma once

#include "SimpleWindow.h"

/**
 * @brief 自定义值转换器，将int转换为std::wstring
 */
class MyCustomConverter : public sw::IValueConverter<int, std::wstring>
{
public:
    std::wstring Convert(int source) override
    {
        return L"Current Value: " + std::to_wstring(source);
    }
    int ConvertBack(const std::wstring &target) override
    {
        throw std::runtime_error("ConvertBack is not implemented.");
    }
};

/**
 * @brief 自定义窗口类，继承自sw::Window
 */
class MyWindow : public sw::Window
{
private:
    sw::Label label;
    sw::Slider slider;
    sw::SpinBox spinBox;

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

        // 创建slider与spinBox之间的双向绑定
        slider.AddBinding(sw::Binding::Create(
            &sw::Slider::Value, &spinBox, &sw::SpinBox::Value, sw::BindingMode::TwoWay));

        // 将label的文本绑定到spinBox的值，并使用自定义的转换器，转换器生命周期由绑定对象管理
        // 此外，框架还提供了一些常用的内置转换器可以直接使用，如sw::IntToStringConverter等
        label.AddBinding(sw::Binding::Create(
            &sw::Label::Text, &spinBox, &sw::SpinBox::Value, sw::BindingMode::OneWay, new MyCustomConverter));
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
