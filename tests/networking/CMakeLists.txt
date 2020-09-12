set(SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")

set(NETWORKING_TEST_SRCS
    "${SOURCE_ROOT}/data_buffer_serialiser_tests.cpp"
    "${SOURCE_ROOT}/packet_tests.cpp"
    "${SOURCE_ROOT}/reliable_ordered_channel_tests.cpp"
    "${SOURCE_ROOT}/unreliable_sequenced_channel_tests.cpp"
    "${SOURCE_ROOT}/unreliable_unordered_channel_tests.cpp"

    PARENT_SCOPE
)
