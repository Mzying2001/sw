cmake_minimum_required(VERSION 3.10)

project(sw)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -finput-charset=UTF-8 -std=c++14")
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3 /utf-8 /std:c++14")
endif()

include_directories("${PROJECT_SOURCE_DIR}/inc")

aux_source_directory("${PROJECT_SOURCE_DIR}/src" DIR_SRC)

add_library(sw STATIC ${DIR_SRC})
