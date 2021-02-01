set(SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(INCLUDE_ROOT "${CMAKE_SOURCE_DIR}/include/iris/jobs/fiber")

if(IRIS_PLATFORM MATCHES "MACOS")
  add_subdirectory("posix")
elseif(IRIS_PLATFORM MATCHES "WINDOWS")
endif()

set(JOBS_IMPL_SRCS
    ${JOBS_IMPL_SRCS} "${INCLUDE_ROOT}/fiber.h" "${SOURCE_ROOT}/job_system.cpp"
    "${SOURCE_ROOT}/counter.cpp" "${INCLUDE_ROOT}/counter.h"
    PARENT_SCOPE)
