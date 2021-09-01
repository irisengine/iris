set(DEFAULT_ROOT "${CMAKE_SOURCE_DIR}/src/core/default")
set(INCLUDE_ROOT "${CMAKE_SOURCE_DIR}/include/iris/core/win32")

target_sources(iris PRIVATE
    ${DEFAULT_ROOT}/resource_loader.cpp
    ${SOURCE_ROOT}/semaphore.cpp
    ${SOURCE_ROOT}/start.cpp
    ${SOURCE_ROOT}/thread.cpp)
