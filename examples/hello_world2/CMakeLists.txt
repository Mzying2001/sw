add_executable(hello_world2 WIN32 main.cpp)

target_link_libraries(hello_world2 PRIVATE ${PROJECT_NAME})