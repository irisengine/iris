set(SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(DEFAULT_ROOT "${CMAKE_SOURCE_DIR}/src/core/default")
set(INCLUDE_ROOT "${CMAKE_SOURCE_DIR}/include/iris/core/win32")

set(CORE_SRCS
    ${CORE_SRCS}
    "${DEFAULT_ROOT}/resource_loader.cpp"
    "${SOURCE_ROOT}/semaphore.cpp"
    "${SOURCE_ROOT}/start.cpp"
    "${SOURCE_ROOT}/thread.cpp"
    PARENT_SCOPE)
