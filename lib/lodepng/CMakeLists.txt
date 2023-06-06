project(lodepng LANGUAGES C)

add_library(lodepng src/lodepng.c)

target_include_directories(lodepng PUBLIC include)