cmake_minimum_required(VERSION 2.9)

project(Lab4 C)

set(C_STANDARD C99)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

add_subdirectory(lib/heatmap)
add_subdirectory(lib/lodepng)

add_executable(main src/main.c src/config.c src/matrix.c)

target_include_directories(main PRIVATE include)

find_package(MPI REQUIRED)
target_include_directories(main PRIVATE ${MPI_INCLUDE_PATH})
target_link_libraries(main PRIVATE ${MPI_C_LIBRARIES})
target_link_libraries(main PRIVATE heatmap)
target_link_libraries(main PRIVATE m)
target_link_libraries(main PRIVATE lodepng)