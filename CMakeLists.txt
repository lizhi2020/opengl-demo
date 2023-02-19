cmake_minimum_required(VERSION 3.21)

project(opengl-demo)

add_definitions("-DUNICODE")
add_executable(demo src/main.cpp)

add_subdirectory(glad)
target_link_libraries(demo PUBLIC glad)