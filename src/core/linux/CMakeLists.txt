set(DEFAULT_ROOT "${PROJECT_SOURCE_DIR}/src/core/default")

target_sources(iris PRIVATE
    ${DEFAULT_ROOT}/resource_loader.cpp
    semaphore.cpp
    start.cpp
    static_buffer.cpp
    thread.cpp)
