cmake_minimum_required(VERSION 3.14)

project(sw VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -finput-charset=UTF-8)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    add_compile_options(/W3 /utf-8)
endif()

file(GLOB SRC_FILES "src/*.cpp")
file(GLOB HEADER_FILES "include/*.h")

add_library(${PROJECT_NAME} STATIC ${SRC_FILES} ${HEADER_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC "include")

add_subdirectory(single_header)
add_subdirectory(examples)
#add_subdirectory(docs)


# 查找 Doxygen
find_package(Doxygen REQUIRED)
# 配置 Doxygen
if (DOXYGEN_FOUND)
    message("Doxygen found.")
    # 设置 Doxygen 输入目录
    set(DOXYGEN_INPUT "${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/src")

    # 设置 Doxygen 输出目录
    set(DOXYGEN_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/docs")

    # 添加自定义目标来生成文档
    add_custom_target(docs
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_SOURCE_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Generating API documentation with Doxygen"
    )
else()
    message(WARNING "Doxygen not found, documentation target will not be available")
endif()