cmake_minimum_required(VERSION 3.0)

project(day15)

set(CMAKE_CXX_STANDARD 17)

add_executable(day15 main.cpp)

install(TARGETS day15 RUNTIME DESTINATION bin)
