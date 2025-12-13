# 获取当前目录名作为项目名
get_filename_component(EXAMPLE_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

# 添加可执行文件
add_executable(${EXAMPLE_NAME} WIN32 src/main.cpp)

# 应用公共编译选项
target_compile_options(${EXAMPLE_NAME} PRIVATE ${COMMON_COMPILE_OPTIONS})

# 应用公共链接库
target_link_libraries(${EXAMPLE_NAME} PRIVATE ${COMMON_LINK_LIBRARIES})

# 添加到全局目标列表
set(EXAMPLE_TARGETS ${EXAMPLE_TARGETS} ${EXAMPLE_NAME} PARENT_SCOPE)
