ADD_SUBDIRECTORY(third_party/googletest)

ADD_EXECUTABLE(unit_tests ${SCRIPT_TEST_SRCS})
TARGET_LINK_LIBRARIES(unit_tests iris)
TARGET_LINK_LIBRARIES(unit_tests gtest_main)

INCLUDE_DIRECTORIES(unit_tests SYSTEM third_party/googletest/googletest/include)

