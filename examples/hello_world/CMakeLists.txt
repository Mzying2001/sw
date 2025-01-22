add_executable(hello_world WIN32 main.cpp)

target_link_libraries(hello_world PRIVATE ${PROJECT_NAME})