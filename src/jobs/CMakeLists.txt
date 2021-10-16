set(INCLUDE_ROOT "${PROJECT_SOURCE_DIR}/include/iris/jobs")

add_subdirectory("fiber")
add_subdirectory("thread")

target_sources(iris PRIVATE
    ${INCLUDE_ROOT}/concurrent_queue.h
    ${INCLUDE_ROOT}/context.h
    ${INCLUDE_ROOT}/job.h
    ${INCLUDE_ROOT}/job_system.h
    ${INCLUDE_ROOT}/job_system_manager.h)
