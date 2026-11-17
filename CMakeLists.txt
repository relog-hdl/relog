cmake_minimum_required(VERSION 3.20)
project(relog)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb3")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)

add_subdirectory(librelog)
add_subdirectory(rlc)
