SET(SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
SET(INCLUDE_ROOT "${CMAKE_SOURCE_DIR}/include/log")

SET(LOG_SRCS
    "${SOURCE_ROOT}/basic_formatter.cpp" "${INCLUDE_ROOT}/basic_formatter.hpp"
    "${SOURCE_ROOT}/colour_formatter.cpp" "${INCLUDE_ROOT}/colour_formatter.hpp"
    "${SOURCE_ROOT}/stdout_outputter.cpp" "${INCLUDE_ROOT}/stdout_outputter.hpp"

    PARENT_SCOPE
)

