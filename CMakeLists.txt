cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

project(Cxxtend
    VERSION 1.0
    LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
    message(FATAL_ERROR "In-source builds not allowed. Please make a new
    directory (called a build directory) and run CMake from there.")
endif()

add_subdirectory(source)
enable_testing()
add_subdirectory(test)

add_compile_options($<$<BOOL:${MSVC}:/permissive->)
