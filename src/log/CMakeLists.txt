set(SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(INCLUDE_ROOT "${CMAKE_SOURCE_DIR}/include/log")

set(LOG_SRCS
    "${INCLUDE_ROOT}/log_level.hpp"
    "${INCLUDE_ROOT}/logger.hpp"
    "${SOURCE_ROOT}/basic_formatter.cpp" "${INCLUDE_ROOT}/basic_formatter.hpp"
    "${SOURCE_ROOT}/colour_formatter.cpp" "${INCLUDE_ROOT}/colour_formatter.hpp"
    "${SOURCE_ROOT}/file_outputter.cpp" "${INCLUDE_ROOT}/file_outputter.hpp"
    "${SOURCE_ROOT}/stdout_outputter.cpp" "${INCLUDE_ROOT}/stdout_outputter.hpp"

    PARENT_SCOPE
)

