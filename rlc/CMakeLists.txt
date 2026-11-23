# Structure based on cmake-examples/02-sub-projects
# used under The MIT License (MIT)
# Copyright (c) 2015 Thom Troy <https://hopstorawpointers.blogspot.com/>
# (https://github.com/ttroy50/cmake-examples)
#
# Lexer & parser integration based on flex-bison-example
# used under The MIT License (MIT)
# Copyright (c) 2020 Ivano Bilenchi <https://ivanobilenchi.com>
# (https://github.com/IvanoBilenchi/flex-bison-example)

project(rlc VERSION 0.0.1)
set(PROJECT_VERSION_TAG "dev")

set(PIPELINE_DIR "${PROJECT_SOURCE_DIR}/src/pipeline")
set(PIPELINE_PHASE_DIR "${PIPELINE_DIR}/phase")
set(INLINE_PHASE_DIR "${PIPELINE_PHASE_DIR}/inliner")
set(LEX_PHASE_DIR "${PIPELINE_PHASE_DIR}/lex")
set(PARSE_PHASE_DIR "${PIPELINE_PHASE_DIR}/parse")


### Lexer & parser

find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

set(INLINER_PATH_TMPLT "${INLINE_PHASE_DIR}/inliner.l")
set(LEXER_PATH_TMPLT "${LEX_PHASE_DIR}/lexer.l")
set(PARSER_PATH_TMPLT "${PARSE_PHASE_DIR}/parser.y")

flex_target(
	inliner
	"${INLINER_PATH_TMPLT}"
	"${INLINER_PATH_TMPLT}.cpp"
)

flex_target(
	lexer
	"${LEXER_PATH_TMPLT}"
	"${LEXER_PATH_TMPLT}.cpp"
)

bison_target(
	parser
	"${PARSER_PATH_TMPLT}"
	"${PARSER_PATH_TMPLT}.cpp"
	DEFINES_FILE "${PARSER_PATH_TMPLT}.hpp"
	OPTIONS -Wcounterexamples
)

add_flex_bison_dependency(lexer parser)


### Executable: 'rlc'

add_executable(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
	PRIVATE
		${PROJECT_SOURCE_DIR}/include
		${PROJECT_SOURCE_DIR}/src
)

target_compile_options(${PROJECT_NAME}
	PRIVATE
		-Wall
		-Wcast-align
		-Wextra
		-Wfloat-equal
		-Wformat-nonliteral
		-Wformat-security
		-Wformat=2
		-Winit-self
		-Wmissing-declarations
		-Wmissing-format-attribute
		-Wmissing-include-dirs
		-Wmissing-noreturn
		-Wpointer-arith
		-Wshadow
		-Wswitch-default
		-Wwrite-strings
		-Werror
		-pedantic
		-fPIC
)

target_sources(${PROJECT_NAME}
	PRIVATE
		src/lib/io/source_file.cpp
		src/service/logging/logger_backend_terminal.cpp
		src/service/logging/logger_formatter_terminal.cpp
		src/service/logging/loggerf.cpp
		src/service/source/source_stack.cpp
		src/bootstrap/opts.cpp
		src/bootstrap/bootstrap.cpp
		src/pipeline/pipeline.cpp
		${INLINER_PATH_TMPLT}.cpp
		src/pipeline/phase/inliner/inliner.cpp
		${LEXER_PATH_TMPLT}.cpp
		src/pipeline/phase/lex/lexer.cpp
		${PARSER_PATH_TMPLT}.cpp
		src/pipeline/phase/parse/parser.cpp
)

target_link_libraries(${PROJECT_NAME}
	PUBLIC
		librelog
		m # Math lib
)


### Versioning

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
