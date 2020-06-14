option(ENABLE_TEST_COVERAGE "Enable test coverage" OFF)

if(ENABLE_TEST_COVERAGE)
    add_compile_options(-O0 -g -fprofile-arcs -ftest-coverage)
    add_link_options(-fprofile-arcs -ftest-coverage)
    message(STATUS "Compiling with coverage flags.")
endif()

add_subdirectory(unit)
add_subdirectory(benchmark)
