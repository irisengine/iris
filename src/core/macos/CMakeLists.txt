set(SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(DEFAULT_ROOT "${CMAKE_SOURCE_DIR}/src/core/default")
set(INCLUDE_ROOT "${CMAKE_SOURCE_DIR}/include/iris/core/macos")

set(CORE_SRCS
    ${CORE_SRCS}
    "${DEFAULT_ROOT}/resource_loader.cpp"
    "${INCLUDE_ROOT}/macos_ios_utility.h"
    "${INCLUDE_ROOT}/utility.h"
    "${SOURCE_ROOT}/macos_ios_utility.mm"
    "${SOURCE_ROOT}/semaphore.cpp"
    "${SOURCE_ROOT}/start.mm"
    "${SOURCE_ROOT}/static_buffer.cpp"
    "${SOURCE_ROOT}/thread.cpp"
    PARENT_SCOPE)
