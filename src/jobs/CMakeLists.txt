set(INCLUDE_ROOT "${CMAKE_SOURCE_DIR}/include/iris/jobs")

if(IRIS_JOBS_API MATCHES "FIBERS")
  add_subdirectory("fiber")
elseif(IRIS_JOBS_API MATCHES "THREADS")
  add_subdirectory("thread")
endif()

target_sources(iris PRIVATE
    ${INCLUDE_ROOT}/concurrent_queue.h
    ${INCLUDE_ROOT}/context.h
    ${INCLUDE_ROOT}/job.h
    ${INCLUDE_ROOT}/job_system.h
    job.cpp)

if(IRIS_ARCH MATCHES "X86_64")
target_sources(iris PRIVATE
    ${INCLUDE_ROOT}/arch/x86_64/context.h
    ${INCLUDE_ROOT}/arch/x86_64/functions.S)
endif()
