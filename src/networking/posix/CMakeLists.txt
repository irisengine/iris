set(SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(INCLUDE_ROOT "${CMAKE_SOURCE_DIR}/include/networking/posix")

set(NETWORKING_SRCS
    "${INCLUDE_ROOT}/auto_socket.h"
    "${SOURCE_ROOT}/udp_socket.cpp"

    PARENT_SCOPE
)
