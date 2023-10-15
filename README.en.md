# `sw`

[中文](./README.md) | **English**

`sw`, short for `SimpleWindow`, is a C++ GUI framework for building Windows desktop applications. It provides object-oriented encapsulation of Windows standard controls and is suitable for creating simple desktop applications.

## Features

+ **Based on Windows API**:  
  SimpleWindow is a framework based on the Windows API with no additional dependencies.
+ **Supports Properties**:  
  It implements property syntax similar to C#, allowing for intuitive access and modification of object properties.
+ **High DPI Support**:  
  Native support for high DPI using DIP (Device Independent Pixels) as the default unit of measurement.
+ **Flexible Layout**:  
  Implements a layout system similar to WPF, supporting features like `Margin` and various layouts such as `StackLayout` and `DockLayout`.
+ **Routed Events**:  
  Adopts a routed event mechanism, where events can bubble up from the source control to propagate upwards. Event handlers can be registered on controls at different levels.

## Quick Start

Here is a SimpleWindow HelloWorld program. For more examples, see [here](./examples).

```c++
#include "SimpleWindow.h"

int wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, INT nCmdShow)
{
    // Window object
    static sw::Window mainWindow;

    // Button object
    static sw::Button button;

    // Modify the window layout to center the button
    mainWindow.SetLayout<sw::FillLayout>();

    // Modify the button text by assigning a value to the Text property
    button.Text = L"Click Me";

    // Register a button click event handler to show a message box when the button is clicked
    button.RegisterRoutedEvent(sw::ButtonBase_Clicked,
        [](sw::UIElement& sender, sw::RoutedEventArgs& e) {
            sw::MsgBox::Show(mainWindow, L"Hello, SimpleWindow!");
        });

    mainWindow.AddChild(button);
    mainWindow.Show();

    // Start the message loop
    return sw::App::MsgLoop();
}
```

## License

The SimpleWindow framework is released under the MIT license, allowing you to freely use and modify it in your own projects.

## Contribution

Contributors are welcome to participate in the development and improvement of SimpleWindow. If you find bugs or have suggestions for improvements, please raise an issue or submit a pull request.

## Stargazers over time

[![Stargazers over time](https://starchart.cc/Mzying2001/sw.svg)](https://starchart.cc/Mzying2001/sw)
