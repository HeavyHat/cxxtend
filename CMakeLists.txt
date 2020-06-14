cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

project(Cxxtend
    VERSION 1.0
    LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file( GLOB_RECURSE SRCS *.cpp *.h )
find_program(CLANG_FORMAT NAMES "clang-format" "clang-format-3.6")
if(CLANG_FORMAT)
    message(STATUS "Found clang-format at ${CLANG_FORMAT}")

    add_custom_target(format
            COMMENT "Running clang-format"
            COMMAND ${CLANG_FORMAT} -i -style=file -fallback-style=none
            ${SRCS})

    add_custom_target(check-format
            COMMENT "Checking clang-format"
            COMMAND ${CLANG_FORMAT} -style=file -fallback-style=none
            --output-replacements-xml
            ${SRCS}
            | grep -q "replacement offset")
endif()

if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
    message(FATAL_ERROR "In-source builds not allowed. Please make a new
    directory (called a build directory) and run CMake from there.")
endif()

add_subdirectory(source)
enable_testing()
add_subdirectory(test)

add_compile_options($<$<BOOL:${MSVC}:/permissive->)
