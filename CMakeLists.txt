cmake_minimum_required(VERSION 3.20)
project(rlc)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb3")
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(CMAKE_CXX_STANDARD 23)

add_executable(rlc)

target_include_directories(rlc PRIVATE src)

#target_compile_options(rlc PRIVATE -Wall -Wextra -Werror -pedantic)

# Link math lib
target_link_libraries(rlc -lm)

target_sources(rlc
	PRIVATE
	src/rlc.cpp
)