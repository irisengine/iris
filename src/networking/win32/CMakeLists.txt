set(SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(INCLUDE_ROOT "${PROJECT_SOURCE_DIR}/include/iris/networking/windows")

set(NETWORKING_SRCS
    "${INCLUDE_ROOT}/winsock.h" "${SOURCE_ROOT}/winsock.cpp"
    PARENT_SCOPE)
