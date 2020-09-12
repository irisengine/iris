set(SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(INCLUDE_ROOT "${CMAKE_SOURCE_DIR}/include/iris/jobs")

if(IRIS_ARCH MATCHES "X86_64")
    set(ARCH_SRCS
        "${INCLUDE_ROOT}/arch/x86_64/context.h"
        "${INCLUDE_ROOT}/arch/x86_64/functions.S"
    )
endif()

if(IRIS_JOBS_API MATCHES "FIBERS")
    add_subdirectory("fiber")
elseif(IRIS_JOBS_API MATCHES "THREADS")
    add_subdirectory("thread")
endif()

set(JOB_SRCS
    ${ARCH_SRCS}
    ${JOBS_IMPL_SRCS}
    "${INCLUDE_ROOT}/concurrent_queue.h"
    "${INCLUDE_ROOT}/context.h"
    "${INCLUDE_ROOT}/job.h"
    "${INCLUDE_ROOT}/job_system.h"

    PARENT_SCOPE
)

