add_subdirectory(third_party/googletest)

add_subdirectory("jobs")
add_subdirectory("platform")

add_executable(unit_tests ${JOBS_TEST_SRCS} ${PLATFORM_TEST_SRCS})

target_link_libraries(unit_tests iris)
target_link_libraries(unit_tests gtest_main)

include_directories(unit_tests SYSTEM third_party/googletest/googletest/include)
include_directories(unit_tests PUBLIC ${CMAKE_SOURCE_DIR}/include)
include_directories(unit_tests PUBLIC ${CMAKE_SOURCE_DIR}/tests)

