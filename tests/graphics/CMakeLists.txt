set(SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")

set(GRAPHICS_TEST_SRCS
    "${SOURCE_ROOT}/render_queue_builder_tests.cpp"
    "${SOURCE_ROOT}/render_command_tests.cpp"
    "${SOURCE_ROOT}/renderer_tests.cpp"
    PARENT_SCOPE)
