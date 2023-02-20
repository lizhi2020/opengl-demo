cmake_minimum_required(VERSION 3.21)

# todo: cmake format

project(opengl-demo)

add_definitions("-DUNICODE")

set(base src/main.cpp)
add_executable(demo1
    ${base}
    src/demo1.cpp
)

add_subdirectory(glad)
target_link_libraries(demo1 PUBLIC glad)