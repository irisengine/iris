set(DEFAULT_ROOT "${PROJECT_SOURCE_DIR}/src/core/default")
set(LINUX_ROOT "${PROJECT_SOURCE_DIR}/src/core/linux")
set(INCLUDE_ROOT "${PROJECT_SOURCE_DIR}/include/iris/core/macos")

target_sources(iris PRIVATE
    ${DEFAULT_ROOT}/resource_loader.cpp
    ${INCLUDE_ROOT}/macos_ios_utility.h
    ${INCLUDE_ROOT}/utility.h
    ${LINUX_ROOT}/static_buffer.cpp
    macos_ios_utility.mm
    semaphore.cpp
    start.mm
    thread.cpp)
