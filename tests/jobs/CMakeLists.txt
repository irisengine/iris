set(SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")

if(IRIS_JOBS_API MATCHES "FIBERS")
    set(API_SPECIFIC_SRCS
        "${SOURCE_ROOT}/concurrent_queue_tests.cpp"
        "${SOURCE_ROOT}/counter_tests.cpp"
    )
endif()

set(JOBS_TEST_SRCS
    "${API_SPECIFIC_SRCS}"
    "${SOURCE_ROOT}/job_tests.cpp"

    PARENT_SCOPE
)
