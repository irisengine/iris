set(SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(INCLUDE_ROOT "${CMAKE_SOURCE_DIR}/include/jobs/fiber")

set(JOBS_IMPL_SRCS
    "${SOURCE_ROOT}/counter.cpp" "${INCLUDE_ROOT}/counter.h"

    PARENT_SCOPE
)

