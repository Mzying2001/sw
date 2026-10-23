cmake_minimum_required(VERSION 3.10)

# 定义项目
project(sw VERSION 0.2.1)

# 静态库sw
add_library(sw STATIC)

# 设置C++标准
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 针对不同编译器设置特定的编译选项
if(MSVC)
    # MSVC编译选项：需处理/utf-8选项冲突问题
    # 获取当前继承的编译选项
    get_target_property(_opts sw COMPILE_OPTIONS)

    # 标记是否存在/utf-8选项
    set(_has_utf8 FALSE)

    # 检查是否已存在/utf-8
    if(_opts)
        foreach(opt IN LISTS _opts)
            if(opt MATCHES "/[Uu][Tt][Ff]-8")
                set(_has_utf8 TRUE)
                break()
            endif()
        endforeach()
    endif()

    # vcpkg环境下默认就有/utf-8选项
    if (DEFINED VCPKG_TARGET_TRIPLET)
        set(_has_utf8 TRUE)
    endif()

    # 添加编译选项
    if (NOT _has_utf8)
        target_compile_options(sw PRIVATE /W3 /source-charset:utf-8)
    else()
        target_compile_options(sw PRIVATE /W3)
    endif()
else()
    # GCC/Clang编译选项
    target_compile_options(sw PRIVATE -Wall -finput-charset=UTF-8)
endif()

# 包含头文件目录
target_include_directories(sw PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/inc>
    $<INSTALL_INTERFACE:include>
)

# 添加源文件
target_sources(sw PRIVATE
    src/Animation.cpp
    src/App.cpp
    src/BmpBox.cpp
    src/Button.cpp
    src/ButtonBase.cpp
    src/Canvas.cpp
    src/CanvasLayout.cpp
    src/CheckableButton.cpp
    src/CheckBox.cpp
    src/Color.cpp
    src/ColorDialog.cpp
    src/ComboBox.cpp
    src/CommandLink.cpp
    src/Control.cpp
    src/Cursor.cpp
    src/DateTimePicker.cpp
    src/Dip.cpp
    src/DockLayout.cpp
    src/DockPanel.cpp
    src/DockSplitter.cpp
    src/FileDialog.cpp
    src/FillLayout.cpp
    src/FolderDialog.cpp
    src/Font.cpp
    src/FontDialog.cpp
    src/FrameworkElement.cpp
    src/Grid.cpp
    src/GridLayout.cpp
    src/GroupBox.cpp
    src/HotKeyControl.cpp
    src/HwndHost.cpp
    src/HwndWrapper.cpp
    src/Icon.cpp
    src/IconBox.cpp
    src/ImageList.cpp
    src/IPAddressControl.cpp
    src/ItemsControl.cpp
    src/Label.cpp
    src/Layer.cpp
    src/LayoutHost.cpp
    src/ListBox.cpp
    src/ListView.cpp
    src/Menu.cpp
    src/MenuItem.cpp
    src/MonthCalendar.cpp
    src/MsgBox.cpp
    src/NotifyIcon.cpp
    src/Panel.cpp
    src/PasswordBox.cpp
    src/Path.cpp
    src/Point.cpp
    src/ProgressBar.cpp
    src/Property.cpp
    src/RadioButton.cpp
    src/Rect.cpp
    src/Screen.cpp
    src/Size.cpp
    src/Slider.cpp
    src/SpinBox.cpp
    src/SplitButton.cpp
    src/Splitter.cpp
    src/StackLayout.cpp
    src/StackPanel.cpp
    src/StaticControl.cpp
    src/StatusBar.cpp
    src/SysLink.cpp
    src/TabControl.cpp
    src/TextBox.cpp
    src/TextBoxBase.cpp
    src/Thickness.cpp
    src/Timer.cpp
    src/ToolTip.cpp
    src/TreeView.cpp
    src/UIElement.cpp
    src/UniformGrid.cpp
    src/UniformGridLayout.cpp
    src/Utils.cpp
    src/Window.cpp
    src/WndBase.cpp
    src/WrapLayout.cpp
    src/WrapPanel.cpp
)

# 安装目标
install(TARGETS sw EXPORT swTargets
    ARCHIVE DESTINATION "lib"
    LIBRARY DESTINATION "lib"
    RUNTIME DESTINATION "bin"
)

# 安装头文件
install(DIRECTORY "${PROJECT_SOURCE_DIR}/inc/" DESTINATION "include/sw")

# 安装CMake配置文件
install(EXPORT swTargets
    FILE "swTargets.cmake"
    NAMESPACE sw::
    DESTINATION "share/sw"
)

# 用于生成配置文件
include(CMakePackageConfigHelpers)

# 生成swConfig.cmake
configure_package_config_file(
    "${PROJECT_SOURCE_DIR}/swConfig.cmake.in"
    "${PROJECT_BINARY_DIR}/swConfig.cmake"
    INSTALL_DESTINATION "share/sw"
)

# 生成版本文件
write_basic_package_version_file(
    "${PROJECT_BINARY_DIR}/swConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

# 安装配置文件
install(FILES
    "${PROJECT_BINARY_DIR}/swConfig.cmake"
    "${PROJECT_BINARY_DIR}/swConfigVersion.cmake"
    DESTINATION "share/sw"
)
