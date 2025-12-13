# `sw`

**中文** | [English](./README.en.md)

`sw` 即 `SimpleWindow`，是一个用于构建 Windows 桌面程序的 C++ GUI 框架，实现了对 Windows 标准控件的面向对象封装，适合用于构建简单的桌面应用程序。

## 特点

+ **基于 Windows API**：  
  SimpleWindow 是一个基于 Windows API 的框架，无其他依赖项。

+ **高 DPI 支持**：  
  原生支持高 DPI，使用 DIP（设备独立像素）作为默认长度单位。

+ **灵活的布局**：  
  实现了一套类似于 WPF 的布局系统，支持诸如 `GridLayout`、`StackLayout`、`DockLayout` 等各种布局。

+ **路由事件**：  
  采用路由事件机制，即事件可以冒泡，从源控件向上传播，可以在不同层次的控件上注册事件处理程序。

+ **属性绑定**：  
  实现了类似 C# 的属性语法，可以直观地获取和修改对象属性；此外还提供了属性绑定支持，可以实现 UI 与数据之间的自动同步。

## 使用说明

### 克隆项目

首先，将本项目克隆到本地：

```bash
git clone https://github.com/Mzying2001/sw.git
```

#### 方式一：使用 Visual Studio

1. 在解决方案中添加 `vs/sw.vcxproj` 项目的引用；
2. 在项目属性中，将路径 `sw/inc` 添加到附加包含目录（Additional Include Directories）。

#### 方式二：使用 CMake

1. 在 `CMakeLists.txt` 中添加：

    ```cmake
    add_subdirectory(path/to/sw)
    ```

2. 链接 `sw` 库，例如：

   ```cmake
   target_link_libraries(your_target PRIVATE sw)
   ```

### 使用单文件版本

[single_header](https://github.com/Mzying2001/sw/tree/main/single_header) 中提供了 `sw` 的单文件版本，由脚本自动生成。直接下载 `sw_all.h` 和 `sw_all.cpp` 并添加到项目中即可使用。

### 使用 vcpkg

`sw` 已支持 [vcpkg](https://github.com/microsoft/vcpkg) 包管理器，包名为 `mzying2001-sw`。

1. 安装 vcpkg 包：

   ```bash
   vcpkg install mzying2001-sw
   ```

2. 若使用 CMake，添加 `sw` 到项目中：

   ```cmake
   find_package(sw CONFIG REQUIRED)
   target_link_libraries(your_target PRIVATE sw::sw)
   ```

3. 配置完成后，可以在项目中直接包含 `sw` 的头文件，例如：

   ```cpp
   #include <sw/SimpleWindow.h>
   ```

> [!NOTE]
> `sw` 的代码文件均使用 UTF-8 编码，若文件编码与系统不一致可能会出现编译器警告或者错误，建议在使用时添加 `/utf-8`（对于 MSVC）或者 `-finput-charset=UTF-8`（对于 g++/clang）编译选项，或者自行修改文件编码。

## 快速入门

以下是一个 SimpleWindow 的 HelloWorld 程序，更多示例详见[这里](https://github.com/Mzying2001/sw/tree/main/examples)。

```cpp
#include "SimpleWindow.h"

int WinMain(HINSTANCE, HINSTANCE, LPSTR, INT)
{
    sw::Window wnd;
    sw::Button btn;

    // 修改窗口的布局方式，让按钮居中
    wnd.SetLayout<sw::FillLayout>();

    // 对Text属性赋值即可修改按钮文本
    btn.Text = L"Click Me";

    // 添加按钮单击事件处理函数，实现单击按钮弹出消息框
    btn.AddHandler(sw::ButtonBase_Clicked,
        [](sw::UIElement& sender, sw::RoutedEventArgs& e) {
            sw::MsgBox::Show(L"Hello, SimpleWindow!");
        });

    wnd.AddChild(btn);
    return wnd.ShowDialog();
}
```

## 开发文档

有关 SimpleWindow 的详细开发文档，请访问 [mzying2001.github.io/sw](https://mzying2001.github.io/sw/classes.html)。

## 许可证

SimpleWindow 框架是基于 MIT 许可证发布的，允许您在自己的项目中自由使用和修改它。

## 贡献

欢迎贡献者参与 SimpleWindow 的开发和改进。如果您发现 Bug 或有改进建议，请提出 Issue 或发送 Pull Request。

## Star 趋势

[![Stargazers over time](https://starchart.cc/Mzying2001/sw.svg?variant=adaptive)](https://starchart.cc/Mzying2001/sw)
