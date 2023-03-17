set(LINUX_ROOT "${PROJECT_SOURCE_DIR}/src/core/linux")
set(INCLUDE_ROOT "${PROJECT_SOURCE_DIR}/include/iris/core/macos")

target_sources(iris PRIVATE
    ${INCLUDE_ROOT}/macos_ios_utility.h
    ${INCLUDE_ROOT}/utility.h
    ${LINUX_ROOT}/static_buffer.cpp
    macos_ios_utility.mm
    profiler.cpp
    semaphore.cpp
    start.mm
    thread.cpp)
