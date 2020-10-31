set(SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")

set(CORE_TEST_SRCS
    "${SOURCE_ROOT}/matrix4_tests.cpp"
    "${SOURCE_ROOT}/quaternion_tests.cpp"
    "${SOURCE_ROOT}/real_tests.cpp"
    "${SOURCE_ROOT}/transform_tests.cpp"
    "${SOURCE_ROOT}/vector3_tests.cpp"

    PARENT_SCOPE
)

