# Structure based on cmake-examples/02-sub-projects
# used under The MIT License (MIT)
# Copyright (c) 2015 Thom Troy <https://hopstorawpointers.blogspot.com/>
# (https://github.com/ttroy50/cmake-examples)

cmake_minimum_required(VERSION 3.20)
project(relog)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb3")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)

add_subdirectory(librelog)
add_subdirectory(rlc)
