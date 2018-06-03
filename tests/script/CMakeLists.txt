SET(SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")

SET(SCRIPT_TEST_SRCS
    "${SOURCE_ROOT}/chunk_tests.cpp"
    "${SOURCE_ROOT}/cactus_stack_tests.cpp"
    "${SOURCE_ROOT}/compiler_error_tests.cpp"
    "${SOURCE_ROOT}/compiler_tests.cpp"
    "${SOURCE_ROOT}/lexer_tests.cpp"
    "${SOURCE_ROOT}/parser_error_tests.cpp"
    "${SOURCE_ROOT}/parser_tests.cpp"
    "${SOURCE_ROOT}/value_tests.cpp"
    "${SOURCE_ROOT}/script_tests.cpp"
    "${SOURCE_ROOT}/virtual_machine_tests.cpp"
    "${SOURCE_ROOT}/virtual_machine_error_tests.cpp"

    PARENT_SCOPE
)


