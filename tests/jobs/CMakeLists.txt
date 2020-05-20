set(SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")

set(JOBS_TEST_SRCS
    "${SOURCE_ROOT}/concurrent_queue_tests.cpp"
    "${SOURCE_ROOT}/counter_tests.cpp"
    "${SOURCE_ROOT}/fiber_pool_tests.cpp"
    "${SOURCE_ROOT}/thread_tests.cpp"

    PARENT_SCOPE
)
