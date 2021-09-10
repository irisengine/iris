set(INCLUDE_ROOT "${CMAKE_SOURCE_DIR}/include/iris/jobs/thread")

target_sources(iris PRIVATE
    ${INCLUDE_ROOT}/thread_job_system.h
    ${INCLUDE_ROOT}/thread_job_system_manager.h
    thread_job_system.cpp
    thread_job_system_manager.cpp)

