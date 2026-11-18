project(rlc VERSION 0.0.1)
set(PROJECT_VERSION_TAG "dev")

find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

set(PIPELINE_DIR "${PROJECT_SOURCE_DIR}/src/pipeline")
set(PIPELINE_PHASE_DIR "${PIPELINE_DIR}/phase")
set(LEX_PHASE_DIR "${PIPELINE_PHASE_DIR}/lex")
set(PARSE_PHASE_DIR "${PIPELINE_PHASE_DIR}/parse")

set(LEXER_PATH_TMPLT "${LEX_PHASE_DIR}/lexer.l")
set(PARSER_PATH_TMPLT "${PARSE_PHASE_DIR}/parser.y")

flex_target(
	lexer
	"${LEXER_PATH_TMPLT}"
	"${LEXER_PATH_TMPLT}.c"
	DEFINES_FILE "${LEXER_PATH_TMPLT}.h"
)

bison_target(
	parser
	"${PARSER_PATH_TMPLT}"
	"${PARSER_PATH_TMPLT}.c"
	DEFINES_FILE "${PARSER_PATH_TMPLT}.h"
)

add_flex_bison_dependency(lexer parser)

add_executable(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
	PRIVATE
		${PROJECT_SOURCE_DIR}/include
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
	${LEXER_PATH_TMPLT}.c
	${PARSER_PATH_TMPLT}.c
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

set(PROJECT_VERSION_STRING "${PROJECT_VERSION}")
if(PROJECT_VERSION_TAG)
	set(PROJECT_VERSION_STRING "${PROJECT_VERSION_STRING}-${PROJECT_VERSION_TAG}")
endif()

set(PROJECT_GIT_HASH "unknown")
set(PROJECT_GIT_BRANCH "unknown")
find_package(Git QUIET)
if(GIT_FOUND)
    execute_process(
        COMMAND "${GIT_EXECUTABLE}" rev-parse --short HEAD
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
        OUTPUT_VARIABLE PROJECT_GIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )

    execute_process(
        COMMAND "${GIT_EXECUTABLE}" rev-parse --abbrev-ref HEAD
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
        OUTPUT_VARIABLE PROJECT_GIT_BRANCH
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
endif()

configure_file(
	${PROJECT_SOURCE_DIR}/include/rlc/version.h.in
	${PROJECT_SOURCE_DIR}/include/rlc/version.h
	@ONLY
)
