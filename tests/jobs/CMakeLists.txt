target_sources(unit_tests PRIVATE
    concurrent_queue_tests.cpp
    thread_job_system_tests.cpp)

if(IRIS_ARCH MATCHES "X86_64")
    target_sources(unit_tests PRIVATE
        fiber_job_system_tests.cpp
        counter_tests.cpp)
endif()