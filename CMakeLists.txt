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