set(SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(INCLUDE_ROOT "${CMAKE_SOURCE_DIR}/include/jobs")

if(IRIS_ARCH MATCHES "X86_64")
    set(ARCH_SRCS
        "${INCLUDE_ROOT}/arch/x86_64/context.h"
        "${INCLUDE_ROOT}/arch/x86_64/functions.S"
    )
endif()

if(IRIS_JOBS_API MATCHES "fibers")
    add_subdirectory("fiber")
endif()

set(JOB_SRCS
    ${ARCH_SRCS}
    ${JOBS_IMPL_SRCS}
    "${INCLUDE_ROOT}/context.h"
    "${INCLUDE_ROOT}/job.h"
    "${SOURCE_ROOT}/job_system.cpp" "${INCLUDE_ROOT}/job_system.h"
    "${SOURCE_ROOT}/thread.cpp" "${INCLUDE_ROOT}/thread.h"

    PARENT_SCOPE
)

