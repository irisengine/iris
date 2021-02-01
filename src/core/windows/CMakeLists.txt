set(SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(DEFAULT_ROOT "${CMAKE_SOURCE_DIR}/src/core/default")
set(INCLUDE_ROOT "${CMAKE_SOURCE_DIR}/include/iris/core/macos")

set(CORE_SRCS
    ${CORE_SRCS} "${DEFAULT_ROOT}/start.cpp"
    "${DEFAULT_ROOT}/resource_loader.cpp" "${SOURCE_ROOT}/semaphore.cpp"
    "${SOURCE_ROOT}/window.cpp"
    PARENT_SCOPE)
