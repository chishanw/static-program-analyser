file(GLOB srcs "../autotester/src/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/*/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/*/*/*.cpp")
file(GLOB headers "../autotester/src/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/*/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/*/*.hpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/*/*/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/*/*/*.hpp")
add_executable(integration_testing ${srcs})


target_link_libraries(integration_testing spa)
