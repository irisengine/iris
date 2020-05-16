set(SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(INCLUDE_ROOT "${CMAKE_SOURCE_DIR}/include/jobs/fiber")

set(JOBS_IMPL_SRCS
    "${INCLUDE_ROOT}/fiber_state.h"
    "${SOURCE_ROOT}/counter.cpp" "${INCLUDE_ROOT}/counter.h"
    "${SOURCE_ROOT}/fiber.cpp" "${INCLUDE_ROOT}/fiber.h"

    PARENT_SCOPE
)

