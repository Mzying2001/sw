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

if(MSVC)
    # 测试工程不依赖vcpkg；禁用VS全局vcpkg集成，避免额外库污染链接命令。
    set_target_properties(sw PROPERTIES VS_GLOBAL_VcpkgEnabled false)
endif()

enable_testing()

add_executable(sw_unit_tests
    TestMain.cpp
    unit/PropertyTests.cpp
    unit/DelegateEventTests.cpp
    unit/ReflectionVariantTests.cpp
    unit/CollectionTests.cpp
    unit/TestFrameworkTests.cpp
)

target_include_directories(sw_unit_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/support
)

target_compile_options(sw_unit_tests PRIVATE ${COMMON_COMPILE_OPTIONS})
target_link_libraries(sw_unit_tests PRIVATE sw)

if(MSVC)
    set_target_properties(sw_unit_tests PROPERTIES VS_GLOBAL_VcpkgEnabled false)
endif()

add_test(NAME sw_unit_tests COMMAND sw_unit_tests)
