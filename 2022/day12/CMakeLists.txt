cmake_minimum_required(VERSION 3.0)

project(day12)

set(CMAKE_CXX_STANDARD 17)

add_executable(day12 main.cpp)

install(TARGETS day12 RUNTIME DESTINATION bin)
