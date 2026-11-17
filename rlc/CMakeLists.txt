project(rlc VERSION 0.0.1)

add_executable(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
	PRIVATE
		${PROJECT_SOURCE_DIR}/src
)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wcast-align
	-Wextra -Wfloat-equal -Wformat-nonliteral -Wformat-security
	-Wformat=2 -Winit-self -Wmissing-declarations
	-Wmissing-format-attribute -Wmissing-include-dirs
	-Wmissing-noreturn -Wpointer-arith -Wshadow
	-Wswitch-default -Wwrite-strings -Werror -pedantic -fPIC)

target_sources(${PROJECT_NAME}
	PRIVATE
	src/service/logging/logger_backend_terminal.cpp
	src/service/logging/logger_formatter_terminal.cpp
	src/service/logging/loggerf.cpp
	src/bootstrap/opts.cpp
	src/bootstrap/bootstrap.cpp
)

target_link_libraries(${PROJECT_NAME}
	PUBLIC
		librelog
		m # Math lib
)
