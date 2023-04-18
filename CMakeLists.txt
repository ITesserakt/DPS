cmake_minimum_required(VERSION 2.9)

project(Lab3)

add_executable(main src/main.c)

target_include_directories(main PRIVATE include)
