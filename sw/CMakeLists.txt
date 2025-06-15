cmake_minimum_required(VERSION 3.10)

# 定义项目
project(sw)

# 静态库sw
add_library(sw STATIC)

# 设置C++标准
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 针对不同编译器设置特定的编译选项
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(sw PRIVATE -Wall -finput-charset=UTF-8)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options(sw PRIVATE /W3 /utf-8)
endif()

# 包含头文件目录
target_include_directories(sw PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/inc>
    $<INSTALL_INTERFACE:include>
)

# 指定源文件
file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp)

# 添加源文件
target_sources(sw PRIVATE ${SRC_FILES})

# 安装目标
install(TARGETS sw EXPORT swTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# 安装头文件
install(DIRECTORY ${PROJECT_SOURCE_DIR}/inc/ DESTINATION include/sw)

# 安装CMake配置文件
install(EXPORT swTargets
    FILE swTargets.cmake
    NAMESPACE sw::
    DESTINATION share/sw
)

# 用于生成配置文件
include(CMakePackageConfigHelpers)

# 生成swConfig.cmake
configure_package_config_file(
    ${PROJECT_SOURCE_DIR}/swConfig.cmake.in
    ${PROJECT_BINARY_DIR}/swConfig.cmake
    INSTALL_DESTINATION share/sw
)

# 生成版本文件
write_basic_package_version_file(
    ${PROJECT_BINARY_DIR}/swConfigVersion.cmake
    VERSION 0.0.1
    COMPATIBILITY AnyNewerVersion
)

# 安装配置文件
install(FILES
    ${PROJECT_BINARY_DIR}/swConfig.cmake
    ${PROJECT_BINARY_DIR}/swConfigVersion.cmake
    DESTINATION share/sw
)
