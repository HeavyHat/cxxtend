FILE(GLOB sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_executable(cxxtendbenchmark.tsk
    ${sources}
    )

find_package(benchmark REQUIRED)

target_link_libraries(cxxtendbenchmark.tsk
    benchmark::benchmark
    benchmark::benchmark_main
    cxxtend
    pthread
    )

set_target_properties(cxxtendbenchmark.tsk PROPERTIES CXX_STANDARD 17)

add_custom_target(cxxtendbenchmark
    COMMAND $<TARGET_FILE:cxxtendbenchmark.tsk>
    DEPENDS cxxtendbenchmark.tsk
    COMMENT "Executing cxxtend benchmarks [console]"
    USES_TERMINAL)

add_custom_target(cxxtendbenchmark.json
    COMMAND $<TARGET_FILE:cxxtendbenchmark.tsk> --benchmark_format=json
    --benchmark_out=cxxtendbenchmark_results.json
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/benchmark_results
    DEPENDS cxxtendbenchmark.tsk
    COMMENT "Executing cxxtend benchmarks [json]")

add_custom_target(benchmark
    DEPENDS cxxtendbenchmark
    COMMENT "Executing all benchmarks [console]")

add_custom_target(benchmark.json
    DEPENDS cxxtendbenchmark.json
    COMMENT "Executing all benchmarks [json]")
