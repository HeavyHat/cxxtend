cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

project(Cxxtend
    VERSION 0.1
    LANGUAGES CXX)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
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

find_program(CLANG_TIDY NAMES "clang-tidy")
if(CLANG_TIDY)
    message(STATUS "Found clang-tidy at ${CLANG_TIDY}")

    add_custom_target(clang-tidy
        COMMENT "Running clang-tidy"
            COMMAND ${CLANG_TIDY} -p
            ${PROJECT_BINARY_DIR}/compile_commands.json --checks=all
            ${SRCS})
endif()


if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
    message(FATAL_ERROR "In-source builds not allowed. Please make a new
    directory (called a build directory) and run CMake from there.")
endif()

add_subdirectory(source)
enable_testing()
add_subdirectory(test)
add_subdirectory(docs)

add_compile_options($<$<BOOL:${MSVC}:/permissive->)

set (EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/include")

add_custom_target(tar ALL COMMAND
    ${CMAKE_COMMAND} -E tar "cfvzP" "${PROJECT_NAME}-${PROJECT_VERSION}.tar.gz" "${EXECUTABLE_OUTPUT_PATH}")
add_dependencies(tar cxxtend)
