project(librelog VERSION 0.0.1)

add_library(${PROJECT_NAME} STATIC)

set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")

target_include_directories(${PROJECT_NAME}
	PUBLIC
		${PROJECT_SOURCE_DIR}/include
)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wcast-align
	-Wextra -Wfloat-equal -Wformat-nonliteral -Wformat-security
	-Wformat=2 -Winit-self -Wmissing-declarations
	-Wmissing-format-attribute -Wmissing-include-dirs
	-Wmissing-noreturn -Wpointer-arith -Wshadow
	-Wswitch-default -Wwrite-strings -Werror -pedantic -fPIC)

target_sources(${PROJECT_NAME}
	PRIVATE
		src/librelog.cpp
)

target_link_libraries(${PROJECT_NAME}
	PUBLIC
		m # Math lib
)
