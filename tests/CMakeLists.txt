add_subdirectory(third_party/googletest)

include(GoogleTest)

mark_as_advanced(
  BUILD_GMOCK
  BUILD_GTEST
  BUILD_SHARED_LIBS
  gmock_build_tests
  gtest_build_samples
  gtest_build_tests
  gtest_disable_pthreads
  gtest_force_shared_crt
  gtest_hide_internal_symbols)

add_subdirectory("core")
add_subdirectory("jobs")
add_subdirectory("platform")
add_subdirectory("networking")

add_executable(unit_tests ${CORE_TEST_SRCS} ${JOBS_TEST_SRCS}
                          ${PLATFORM_TEST_SRCS} ${NETWORKING_TEST_SRCS})

target_link_libraries(unit_tests iris gmock_main)
gtest_discover_tests(unit_tests)
