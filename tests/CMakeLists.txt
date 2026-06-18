cmake_minimum_required(VERSION 3.10)

project(sw_tests)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_definitions(UNICODE _UNICODE)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(COMMON_COMPILE_OPTIONS -Wall -finput-charset=UTF-8)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(COMMON_COMPILE_OPTIONS /W3 /utf-8)
endif()

add_subdirectory(${CMAKE_SOURCE_DIR}/../sw sw_build)

enable_testing()

add_executable(sw_unit_tests
    common/TestMain.cpp
    unit/PropertyTests.cpp
    unit/DelegateEventTests.cpp
    unit/ReflectionVariantTests.cpp
    unit/CollectionTests.cpp
)

target_include_directories(sw_unit_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/common
)
target_compile_options(sw_unit_tests PRIVATE ${COMMON_COMPILE_OPTIONS})
target_link_libraries(sw_unit_tests PRIVATE sw)

add_test(NAME sw_unit_tests COMMAND sw_unit_tests)
