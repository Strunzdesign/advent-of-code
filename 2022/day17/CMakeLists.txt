cmake_minimum_required(VERSION 3.0)

project(day17)

set(CMAKE_CXX_STANDARD 17)

add_executable(day17 main.cpp)

install(TARGETS day17 RUNTIME DESTINATION bin)
