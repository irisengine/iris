set(SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(INCLUDE_ROOT "${CMAKE_SOURCE_DIR}/include/iris/log")

set(LOG_SRCS
    "${INCLUDE_ROOT}/log_level.h"
    "${INCLUDE_ROOT}/logger.h"
    "${SOURCE_ROOT}/basic_formatter.cpp" "${INCLUDE_ROOT}/basic_formatter.h"
    "${SOURCE_ROOT}/colour_formatter.cpp" "${INCLUDE_ROOT}/colour_formatter.h"
    "${SOURCE_ROOT}/emoji_formatter.cpp" "${INCLUDE_ROOT}/emoji_formatter.h"
    "${SOURCE_ROOT}/file_outputter.cpp" "${INCLUDE_ROOT}/file_outputter.h"
    "${SOURCE_ROOT}/stdout_outputter.cpp" "${INCLUDE_ROOT}/stdout_outputter.h"

    PARENT_SCOPE
)

