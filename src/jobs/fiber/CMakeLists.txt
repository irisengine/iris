set(INCLUDE_ROOT "${CMAKE_SOURCE_DIR}/include/iris/jobs/fiber")

if(IRIS_PLATFORM MATCHES "MACOS")
  add_subdirectory("posix")
elseif(IRIS_PLATFORM MATCHES "WINDOWS")
  add_subdirectory("windows")
endif()

target_sources(iris PRIVATE
    ${INCLUDE_ROOT}/counter.h
    ${INCLUDE_ROOT}/fiber.h
    counter.cpp
    job_system.cpp)
