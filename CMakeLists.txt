cmake_minimum_required(VERSION 2.9)

project(Lab3 C)

set(C_STANDARD C99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

add_executable(server src/server.c src/socket.c src/shared.c src/game.c src/command.c) 
add_executable(client src/client.c src/socket.c src/shared.c src/game.c src/command.c)

target_include_directories(client PRIVATE include)
target_include_directories(server PRIVATE include)
target_compile_definitions(client PRIVATE $<$<CONFIG:Debug>:DEBUG=1>)
target_compile_definitions(server PRIVATE $<$<CONFIG:Debug>:DEBUG=1>)