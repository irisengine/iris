set(SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(INCLUDE_ROOT "${CMAKE_SOURCE_DIR}/include/iris/networking/windows")

set(CORE_SRCS
    "${INCLUDE_ROOT}/winsock.h" "${SOURCE_ROOT}/winsock.cpp"
    PARENT_SCOPE)
