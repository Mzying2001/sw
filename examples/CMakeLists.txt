cmake_minimum_required(VERSION 3.10)

# 定义父项目
project(examples)

# 设置公共编译选项
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 全局定义UNICODE和_UNICODE
add_compile_definitions(UNICODE _UNICODE)

# 设置公共编译器选项
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(COMMON_COMPILE_OPTIONS -Wall -finput-charset=UTF-8)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(COMMON_COMPILE_OPTIONS /W3 /utf-8)
endif()

# 添加sw库
add_subdirectory(${CMAKE_SOURCE_DIR}/../sw sw_build)

# 设置公共链接库
set(COMMON_LINK_LIBRARIES sw user32 gdi32 comctl32)

# 自动包含所有子目录中的示例
file(GLOB EXAMPLE_DIRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} */)
foreach(example_dir ${EXAMPLE_DIRS})
    # 检查是否有CMakeLists.txt
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${example_dir}/CMakeLists.txt)
        message(STATUS "Adding example: ${example_dir}")
        add_subdirectory(${example_dir})
    endif()
endforeach()

# 添加目标汇总所有示例
add_custom_target(examples
    COMMENT "Building all examples"
    DEPENDS ${EXAMPLE_TARGETS}
)
