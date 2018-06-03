ADD_SUBDIRECTORY(third_party/googletest)

ADD_SUBDIRECTORY("${CMAKE_CURRENT_SOURCE_DIR}/script")

ADD_EXECUTABLE(unit_tests ${SCRIPT_TEST_SRCS})
TARGET_LINK_LIBRARIES(unit_tests iris)
TARGET_LINK_LIBRARIES(unit_tests gtest_main)

INCLUDE_DIRECTORIES(unit_tests SYSTEM third_party/googletest/googletest/include)
INCLUDE_DIRECTORIES(unit_tests PUBLIC ${CMAKE_SOURCE_DIR}/include/script)

