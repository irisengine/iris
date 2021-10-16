set(INCLUDE_ROOT "${PROJECT_SOURCE_DIR}/include/iris/jobs/fiber")

if(IRIS_PLATFORM MATCHES "MACOS")
  add_subdirectory("posix")
elseif(IRIS_PLATFORM MATCHES "WIN32")
  add_subdirectory("windows")
endif()

target_sources(iris PRIVATE
    ${INCLUDE_ROOT}/counter.h
    ${INCLUDE_ROOT}/fiber.h
    ${INCLUDE_ROOT}/fiber_job_system.h
    counter.cpp
    fiber_job_system.cpp
    fiber_job_system_manager.cpp)
