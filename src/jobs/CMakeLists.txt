set(SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(INCLUDE_ROOT "${CMAKE_SOURCE_DIR}/include/jobs")

set(JOB_SRCS
    "${INCLUDE_ROOT}/job.h"
    "${SOURCE_ROOT}/thread.cpp" "${INCLUDE_ROOT}/thread.h"

    PARENT_SCOPE
)

