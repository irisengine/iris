set(SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(INCLUDE_ROOT "${CMAKE_SOURCE_DIR}/include/networking")

add_subdirectory("posix")

set(NETWORKING_SRCS
    ${NETWORKING_SRCS}

    "${INCLUDE_ROOT}/accepting_socket.h"
    "${INCLUDE_ROOT}/socket.h"
    "${INCLUDE_ROOT}/udp_accepting_socket.h"
    "${INCLUDE_ROOT}/udp_socket.h"

    PARENT_SCOPE
)
