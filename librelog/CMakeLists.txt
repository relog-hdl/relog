project(librelog VERSION 0.0.1)
set(PROJECT_VERSION_TAG "dev")


### Library: 'librelog'

add_library(${PROJECT_NAME} STATIC)

# Disable prefix so that artifact is not "liblibrelog.a"
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")

target_include_directories(${PROJECT_NAME}
	PUBLIC
		${PROJECT_SOURCE_DIR}/include
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
		src/librelog.cpp
)

target_link_libraries(${PROJECT_NAME}
	PUBLIC
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
	${PROJECT_SOURCE_DIR}/include/librelog/version.h.in
	${PROJECT_SOURCE_DIR}/include/librelog/version.h
	@ONLY
)
