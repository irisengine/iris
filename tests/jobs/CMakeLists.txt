target_sources(unit_tests PRIVATE
    job_tests.cpp)

if(IRIS_JOBS_API MATCHES "FIBERS")
  target_sources(unit_tests PRIVATE
      concurrent_queue_tests.cpp
      counter_tests.cpp)
endif()