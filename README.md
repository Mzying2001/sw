# `sw`

**中文** | [English](./README.en.md)

`sw` 即 `SimpleWindow`，是一个用于构建 Windows 桌面程序的 C++ GUI 框架，实现了对 Windows 标准控件的面向对象封装，适合用于构建简单的桌面应用程序。

## 特点

+ **基于 Windows API**：  
  SimpleWindow 是一个基于 Windows API 的框架，无其他依赖项。
+ **支持属性**：  
  实现了类似于 C# 的属性语法，可以直观地获取和修改对象属性。
+ **高 DPI 支持**：  
  原生支持高 DPI，使用 DIP（设备独立像素）作为默认长度单位。
+ **灵活的布局**：  
  实现了一套类似于 WPF 的布局系统，支持诸如 `GridLayout`、`StackLayout`、`DockLayout` 等各种布局。
+ **路由事件**：  
  采用路由事件机制，即事件可以冒泡，从源控件向上传播，可以在不同层次的控件上注册事件处理程序。

## 快速入门

以下是一个 SimpleWindow 的 HelloWorld 程序，更多示例详见[这里](./examples)。

```c++
#include "SimpleWindow.h"

int wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, INT nCmdShow)
{
    // 窗口对象
    sw::Window mainWindow;

    // 按钮对象
    sw::Button button;

    // 修改窗口的布局方式，让按钮居中
    mainWindow.SetLayout<sw::FillLayout>();

    // 对Text属性赋值即可修改按钮文本
    button.Text = L"Click Me";

    // 注册按钮单击事件处理函数，实现单击按钮弹出消息框
    button.RegisterRoutedEvent(sw::ButtonBase_Clicked,
        [&](sw::UIElement& sender, sw::RoutedEventArgs& e) {
            sw::MsgBox::Show(mainWindow, L"Hello, SimpleWindow!");
        });

    mainWindow.AddChild(button);
    mainWindow.Show();

    // 开始消息循环
    return sw::App::MsgLoop();
}
```

## 开发文档

有关 SimpleWindow 的详细开发文档，请访问 [mzying2001.github.io/sw](https://mzying2001.github.io/sw)。

## 许可证

SimpleWindow 框架是基于 MIT 许可证发布的，允许您在自己的项目中自由使用和修改它。

## 贡献

欢迎贡献者参与 SimpleWindow 的开发和改进。如果您发现 Bug 或有改进建议，请提出 Issue 或发送 Pull Request。

## Star 趋势

[![Stargazers over time](https://starchart.cc/Mzying2001/sw.svg)](https://starchart.cc/Mzying2001/sw)
