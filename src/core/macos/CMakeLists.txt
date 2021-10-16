set(DEFAULT_ROOT "${PROJECT_SOURCE_DIR}/src/core/default")
set(INCLUDE_ROOT "${PROJECT_SOURCE_DIR}/include/iris/core/macos")

target_sources(iris PRIVATE
    ${DEFAULT_ROOT}/resource_loader.cpp
    ${INCLUDE_ROOT}/macos_ios_utility.h
    ${INCLUDE_ROOT}/utility.h
    macos_ios_utility.mm
    semaphore.cpp
    start.mm
    static_buffer.cpp
    thread.cpp)
