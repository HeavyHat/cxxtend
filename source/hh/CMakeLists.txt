FILE(GLOB sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
FILE(GLOB headers CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/include/hh/*.h")

add_library(cxxtend
    ${sources}
    ${headers}
    )

target_include_directories(cxxtend PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/${PROJECT_NAME}-${PROJECT_VERSION}>
    )

target_link_libraries(cxxtend PUBLIC pthread)

set_target_properties(cxxtend PROPERTIES CXX_STANDARD 17)
