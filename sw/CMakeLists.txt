cmake_minimum_required(VERSION 3.10)

# 定义项目
project(sw)

# 静态库sw
add_library(sw STATIC)

# 设置C++标准为C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 针对不同编译器设置特定的编译选项
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -finput-charset=UTF-8)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    add_compile_options(/W3 /utf-8)
endif()

# 包含头文件目录
target_include_directories(sw PUBLIC "${PROJECT_SOURCE_DIR}/inc")

# 指定源文件
file(GLOB SRC_FILES "${PROJECT_SOURCE_DIR}/src/*.cpp")

# 添加源文件
target_sources(sw PRIVATE ${SRC_FILES})
