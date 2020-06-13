option(ENABLE_TEST_COVERAGE "Enable test coverage" OFF)

if(ENABLE_TEST_COVERAGE)
    add_compile_options(-O0 -g -fprofile-arcs -ftest-coverge)
    add_link_options(-fprofile-arcs -ftest-coverage)
endif()

add_subdirectory(unit)
add_subdirectory(benchmark)
