cmake_minimum_required(VERSION 2.9)

project(LAB1)

set(C_STANDARD C11)

add_executable(main src/main.c src/lift.c src/handler.c)

target_include_directories(main PRIVATE include/)
target_compile_definitions(main PRIVATE $<$<CONFIG:Debug>:DEBUG=1>)
target_link_libraries(main PRIVATE pthread)