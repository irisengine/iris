include(GoogleTest)

mark_as_advanced(BUILD_GMOCK BUILD_GTEST gtest_hide_internal_symbols)

add_subdirectory("core")
add_subdirectory("graphics")
add_subdirectory("jobs")
add_subdirectory("networking")
add_subdirectory("platform")

add_executable(
  unit_tests ${CORE_TEST_SRCS} ${GRAPHICS_TEST_SRCS} ${JOBS_TEST_SRCS}
             ${PLATFORM_TEST_SRCS} ${NETWORKING_TEST_SRCS})

target_include_directories(unit_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(unit_tests iris gmock_main)
gtest_discover_tests(unit_tests)
