cmake_minimum_required(VERSION 3.0)

project(day3)

set(CMAKE_CXX_STANDARD 17)

add_executable(day3 main.cpp)

install(TARGETS day3 RUNTIME DESTINATION bin)
