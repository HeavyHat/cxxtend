FILE(GLOB sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_executable(tests.hh.tsk
    ${sources}
    )

target_link_libraries(tests.hh.tsk PUBLIC
    gtest
    gtest_main
    gmock
    cxxtend
    pthread)

set_target_properties(tests.hh.tsk PROPERTIES CXX_STANDARD 17)

add_test(NAME tests.hh.tsk
    COMMAND $<TARGET_FILE:tests.hh.tsk>
    --gtest_output=xml:tests.junit.xml
    )
